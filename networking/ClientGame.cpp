// ClientGame
//
// Initializes ClientNetwork and sets up anything that needs to be setup on init connection
// Has update loop to always check for incoming data from the server
//
// @Ken

#include "networking\ClientGame.h"
#include "kitten/K_Time.h"
#include <assert.h>
#include <iostream>
#include <unordered_map>
#include <sstream>

// Kibble
#include "kibble\databank\databank.hpp"
#include "kibble\map\MapReader.h"

// Units
#include "kitten\K_GameObjectManager.h"
#include "unit\UnitSpawn.h"
#include "unit\unitComponent\UnitMove.h"
#include "unit\UnitMonitor.h"

// Unit movement
#include "board\BoardManager.h"
#include "unit\unitComponent\UnitMove.h"

// Other
#include "_Project\UniversalSounds.h"
#include "components\DeckInitializingComponent.h"
#include "kitten\event_system\EventManager.h"

#define PING_PACKET_DELAY 5.0f

namespace networking
{
	ClientGame* ClientGame::sm_clientGameInstance = nullptr;
	bool ClientGame::sm_networkValid = false;
	int ClientGame::sm_iClientId = -1;

	std::string ClientGame::sm_dedicatedServerAddress = "localhost";

	// Creates the singleton instance.
	void ClientGame::createInstance(const std::string &p_strAddr)
	{
		assert(sm_clientGameInstance == nullptr);
		sm_clientGameInstance = new ClientGame(p_strAddr);
	}

	// Destroys the singleton instance.
	void ClientGame::destroyInstance()
	{
		assert(sm_clientGameInstance != nullptr);
		delete sm_clientGameInstance;
		sm_clientGameInstance = nullptr;
	}

	// Access to singleton instance.
	ClientGame* ClientGame::getInstance()
	{
		//assert(sm_clientGameInstance);
		return sm_clientGameInstance;
	}

	ClientGame::ClientGame(const std::string &p_strAddr) : m_bGameTurnStart(false), m_timeElapsed(0.0f)
	{
		setupNetwork(p_strAddr);
		m_log = new NetworkLog(CLIENT_LOG);

		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Board_Loaded,
			this,
			std::bind(&ClientGame::boardLoadedListener, this, std::placeholders::_1, std::placeholders::_2));
	}
	
	ClientGame::~ClientGame()
	{
		if (m_network != nullptr)
		{
			delete m_network;
		}

		if (m_log != nullptr)
		{
			delete m_log;
			m_log = nullptr;
		}

		sm_networkValid = false;
		sm_iClientId = -1;

		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Board_Loaded, this);
	}

	void ClientGame::setupNetwork(const std::string &p_strAddr)
	{
		printf("Attempting to join: %s\n", p_strAddr.c_str());
		m_network = new ClientNetwork();

		if (m_network->init(p_strAddr))
		{ 
			sm_networkValid = true;
		}
		else
		{
			printf("Client Network init error: %s\n", m_network->getError().c_str());

			delete m_network;
			m_network = nullptr;

			sm_networkValid = false;
		}
	}

	void ClientGame::connectToDedicatedServer()
	{
		setupNetwork(sm_dedicatedServerAddress);
	}

	void ClientGame::disconnectFromNetwork()
	{
		// Send a packet to alert server that client is disconnecting
		char data[BASIC_PACKET_SIZE];

		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = BASIC_PACKET_SIZE;

		Packet packet;
		packet.m_packetType = CLIENT_DISCONNECT;
		packet.m_clientId = sm_iClientId;		

		packet.serialize(buffer);
		NetworkServices::sendMessage(m_network->m_connectSocket, data, BASIC_PACKET_SIZE);

		// Shutdown ClientNetwork
		if (m_network != nullptr)
		{
			delete m_network;
			m_network = nullptr;
		}

		sm_networkValid = false;
	}

	void ClientGame::update()
	{
		// Ping the host to ensure they haven't disconnnected
		m_timeElapsed += kitten::K_Time::getInstance()->getDeltaTime();		
		if (m_timeElapsed > PING_PACKET_DELAY)
		{
			m_timeElapsed = 0.0f;
			int result = sendBasicPacket(PING_SOCKET);

			if (result == SOCKET_ERROR)
			{
				// Enable end game screen/update join game screen to indicate lost connection
				kitten::Event* eventData = new kitten::Event(kitten::Event::Network_End_Game);
				eventData->putInt(GAME_END_RESULT, PLAYER_DISCONNECTED);
				kitten::EventManager::getInstance()->queueEvent(kitten::Event::Network_End_Game, eventData);
			}
		}

		int data_length = m_network->receivePackets(m_network_data);

		if (data_length <= 0)
		{
			//no data recieved
			return;
		}

		int i = 0;
		PacketTypes packetType;

		while (i < (unsigned int)data_length)
		{
			Buffer defaultBuffer;
			defaultBuffer.m_data = &(m_network_data[i]);
			defaultBuffer.m_size = BASIC_PACKET_SIZE;

			Packet defaultPacket;
			defaultPacket.deserialize(defaultBuffer);

			packetType = (PacketTypes)defaultPacket.m_packetType;
			switch (packetType) {

			case PacketTypes::SEND_CLIENT_ID:
			{
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received SEND_CLIENT_ID (" << defaultPacket.m_clientId << ")";
				m_log->logMessage(message.str());

				printf("[Client: %d] received SEND_CLIENT_ID (%d) packet from server\n", sm_iClientId, defaultPacket.m_clientId);
				sm_iClientId = defaultPacket.m_clientId;

				if (m_boardLoaded)
				{
					sendStartingData();
					m_boardLoaded = false;
				}

				i += BASIC_PACKET_SIZE;
				break;
			}
			case PacketTypes::NO_MAP_ID:
			{
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received NO_MAP_ID (" << defaultPacket.m_clientId << ")";
				m_log->logMessage(message.str());

				printf("[Client: %d] received NO_MAP_ID packet from server\n", sm_iClientId);

				kitten::Event* eventData = new kitten::Event(kitten::Event::Network_Host_Not_Ready);
				eventData->putString(NETWORK_MESSAGE_KEY, "Host not ready, waiting for map ID...");
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Network_Host_Not_Ready, eventData);

				i += BASIC_PACKET_SIZE;
				break;
			}
			case PacketTypes::MAP_DATA:
			{
				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = MAP_DATA_PACKET_SIZE;

				MapDataPacket packet;
				packet.deserialize(buffer);

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received MAP_DATA (" << packet.m_mapId << ")";
				m_log->logMessage(message.str());

				printf("[Client: %d] received MAP_DATA (map ID: %d) packet from server\n", sm_iClientId, packet.m_mapId);

				kibble::MapReader::getInstance()->selectMap(packet.m_mapId);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::P2P_Start_Game, nullptr);

				i += MAP_DATA_PACKET_SIZE;
				break;
			}
			case PacketTypes::SERVER_SHUTDOWN:
			{
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received SERVER_SHUTDOWN";
				m_log->logMessage(message.str());

				printf("[Client: %d] received SERVER_SHUTDOWN packet from server\n", sm_iClientId);

				i += BASIC_PACKET_SIZE;
				disconnectFromNetwork();

				// Display disconnect screen; Server received manual disconnect from server
				kitten::Event* eventData = new kitten::Event(kitten::Event::Network_End_Game);
				eventData->putInt(GAME_END_RESULT, PLAYER_DISCONNECTED);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Network_End_Game, eventData);
				break;
			}
			case PacketTypes::SESSION_ENDED:
			{
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received SESSION_ENDED";
				m_log->logMessage(message.str());

				printf("[Client: %d] received SESSION_ENDED packet from server\n", sm_iClientId);

				i += BASIC_PACKET_SIZE;

				// Display disconnect screen; Server received manual disconnect from server
				kitten::Event* eventData = new kitten::Event(kitten::Event::Network_End_Game);
				eventData->putInt(GAME_END_RESULT, PLAYER_DISCONNECTED);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Network_End_Game, eventData);
				break;
			}
			case PacketTypes::ABILITY_PACKET:
			{
				printf("[Client: %d] received ABILITY_PACKET packet from server\n", sm_iClientId);

				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = MAX_PACKET_SIZE;

				AbilityPacket packet;
				packet.deserialize(buffer);
				i += packet.getBytes();

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received ability:";
				m_log->logMessage(message.str());

				std::string abilityInfo = packet.getFormattedAbilityInfo();
				m_log->logMessage(abilityInfo);

				if (checkSync(packet.m_sourceUnit.posX, packet.m_sourceUnit.posY))
				{
					useAbility(packet);
				} 
				else
				{
					sendDesyncedPacket();
				}
				
				break;
			}
			case PacketTypes::CAST_TIME_ABILITY_PACKET:
			{
				printf("[Client: %d] received CAST_TIME_ABILITY_PACKET packet from server\n", sm_iClientId);

				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = MAX_PACKET_SIZE;

				AbilityPacket packet;
				packet.deserialize(buffer);
				i += packet.getBytes();

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received CastTime ability, setting cast for:";
				m_log->logMessage(message.str());

				std::string abilityInfo = packet.getFormattedAbilityInfo();
				m_log->logMessage(abilityInfo);

				if (checkSync(packet.m_sourceUnit.posX, packet.m_sourceUnit.posY))
				{
					setCastTime(packet);
				} else
				{
					sendDesyncedPacket();
				}
				break;
			}
			case PacketTypes::SUMMON_UNIT:
			{
				printf("[Client: %d] received CLIENT_SUMMON_UNIT packet from server\n", sm_iClientId);

				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = UNIT_PACKET_SIZE;

				UnitPacket summonUnitPacket;
				summonUnitPacket.deserialize(buffer);
				i += UNIT_PACKET_SIZE;

				summonUnit(summonUnitPacket.m_clientId, summonUnitPacket.m_unitId, summonUnitPacket.m_posX, summonUnitPacket.m_posY);

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received SUMMON_UNIT, Unit ID:" << summonUnitPacket.m_unitId << ", X:" 
					<< summonUnitPacket.m_posX << ", Y:" << summonUnitPacket.m_posY;
				m_log->logMessage(message.str());
				break;
			}
			case PacketTypes::SKIP_TURN:
			{
				printf("[Client: %d] received packet SKIP_TURN from server\n", sm_iClientId);

				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = SKIP_TURN_PACKET_SIZE;

				SkipTurnPacket skipTurnPacket;
				skipTurnPacket.deserialize(buffer);
				i += SKIP_TURN_PACKET_SIZE;				

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received SKIP_TURN\n";
				unit::Unit* unit = getUnitFromPos(skipTurnPacket.m_unit.posX, skipTurnPacket.m_unit.posY);
				message << "\tUnit Kibble ID: " << unit->m_kibbleID << "\n";
				message << "\tUnit Pos : (" << skipTurnPacket.m_unit.posX << ", " << skipTurnPacket.m_unit.posY << ")";				
				m_log->logMessage(message.str());

				if (checkSync(skipTurnPacket.m_unit.posX, skipTurnPacket.m_unit.posY))
				{
					unit::Unit* currentUnit = unit::InitiativeTracker::getInstance()->getCurrentUnit()->getComponent<unit::Unit>();
					m_bServerCalling = true;
					currentUnit->playerSkipTurn();
					m_bServerCalling = false;
				}
				else
				{
					sendDesyncedPacket();
				}

				break;
			}
			case PacketTypes::GAME_TURN_START:
			{
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received GAME_TURN_START";
				m_log->logMessage(message.str());

				printf("[Client: %d] received packet GAME_TURN_START from server\n", sm_iClientId);
				i += BASIC_PACKET_SIZE;

				if (!m_bGameTurnStart)
				{
					unit::InitiativeTracker::getInstance()->gameTurnStart();
					m_bGameTurnStart = true;
				}
				break;
			}
			case PacketTypes::READY_CHECK:
			{
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received READY_CHECK";
				m_log->logMessage(message.str());
				printf("[Client: %d] received READY_CHECK packet from server\n", sm_iClientId);

				if (!m_bGameTurnStart)
				{
					kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Players_Ready, nullptr);

					kitten::Event* eventData = new kitten::Event(kitten::Event::Client_Commander_Loaded);
					eventData->putGameObj(COMMANDER_GO_KEY, &m_commander->getGameObject());
					kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Client_Commander_Loaded, eventData);
					m_bGameTurnStart = true;
				}
				i += BASIC_PACKET_SIZE;
				break;
			}
			case PacketTypes::STARTING_COMMANDER_DATA:
			{
				printf("[Client: %d] received STARTING_COMMANDER_DATA packet from server\n", sm_iClientId);

				// Get the other player's Commander data and summon it
				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = STARTING_COMMANDERS_PACKET_SIZE;

				StartingCommandersPacket commanderPacket;
				commanderPacket.deserialize(buffer);		

				UnitNetworkInfo commander0 = commanderPacket.commander0;
				kitten::K_GameObject* commanderGO0 = summonUnit(commander0.clientId, commander0.unitId, commander0.posX, commander0.posY);

				UnitNetworkInfo commander1 = commanderPacket.commander1;
				kitten::K_GameObject* commanderGO1 = summonUnit(commander1.clientId, commander1.unitId, commander1.posX, commander1.posY);

				if (commander0.clientId == sm_iClientId)
				{
					m_commander = commanderGO0->getComponent<unit::Unit>();
				}
				else
				{
					m_commander = commanderGO1->getComponent<unit::Unit>();
				}

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received STARTING_COMMANDER_DATA\n";
				message << "\tCommander 0 - Unit ID:" << commander0.clientId << ", X:" << commander0.posX << ", Y:" << commander0.posY << "\n";
				message << "\tCommander 1 - Unit ID:" << commander1.clientId << ", X:" << commander1.posX << ", Y:" << commander1.posY;
				m_log->logMessage(message.str());

				// The other player has joined and we received their Commander data
				// Queue event to update ReadyCheck component to indicate other player has joined
				kitten::EventManager::getInstance()->queueEvent(kitten::Event::Player_Joined, nullptr);
				UniversalSounds::playSound("fanfare");

				i += STARTING_COMMANDERS_PACKET_SIZE;
				break;
			}
			case PacketTypes::TEXTCHAT_MESSAGE:
			{
				printf("[Client: %d] received TEXTCHAT_MESSAGE packet from server\n", sm_iClientId);
				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = TEXTCHAT_MESSAGE_PACKET_SIZE;

				TextChatMessagePacket messagePacket;
				messagePacket.deserialize(buffer);

				kitten::Event* eventData = new kitten::Event(kitten::Event::TextChat_Receive_Message);
				eventData->putString(TEXTCHAT_MESSAGE_KEY, messagePacket.getMessage());
				eventData->putInt(PLAYER_ID, messagePacket.m_clientId);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::TextChat_Receive_Message, eventData);

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received TEXTCHAT_MESSAGE from Client: " << messagePacket.m_clientId;
				message << "\tMessage: " << messagePacket.getMessage();
				m_log->logMessage(message.str());

				i += TEXTCHAT_MESSAGE_PACKET_SIZE;
				break;
			}
			case PacketTypes::UPDATE_SERVER_INFO:
			{
				Buffer buffer;
				buffer.m_data = &(m_network_data[i]);
				buffer.m_size = SERVER_INFO_PACKET_SIZE;

				ServerInfoPacket serverInfoPacket;
				serverInfoPacket.deserialize(buffer);

				std::stringstream message;
				message << "Client:" << sm_iClientId << " received UPDATE_SERVER_INFO";
				message << "\n\t Player Count: " << serverInfoPacket.m_playerCount;
				message << "\n\t Active Game Sessions: " << serverInfoPacket.m_clientId;
				m_log->logMessage(message.str());

				printf("[Client: %d] received UPDATE_SERVER_INFO packet from server\n", sm_iClientId);
				i += SERVER_INFO_PACKET_SIZE;

				// Send the Server info to the Quickplay class
				kitten::Event* eventData = new kitten::Event(kitten::Event::Update_Server_Info);
				eventData->putInt(SERVER_STATUS_KEY, -1);
				eventData->putInt(SERVER_PLAYER_COUNT_KEY, serverInfoPacket.m_playerCount);
				eventData->putInt(SERVER_ACTIVE_SESSIONS_KEY, serverInfoPacket.m_activeSessions);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Update_Server_Info, eventData);

				break;
			}
			case PacketTypes::QUICKPLAY_FOUND_GAME:
			{
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Quickplay_Start_Game, nullptr);

				i += BASIC_PACKET_SIZE;
				break;
			}
			case PacketTypes::DESYNCED:
			{
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received DESYNCED";
				m_log->logMessage(message.str());

				printf("[Client: %d] received DESYNCED packet from server\n", sm_iClientId);
				i += BASIC_PACKET_SIZE;

				kitten::Event* eventData = new kitten::Event(kitten::Event::Network_End_Game);
				eventData->putInt(GAME_END_RESULT, CLIENT_DESYNCED);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Network_End_Game, eventData);

				break;
			}
			case PacketTypes::SERVER_FULL:
			{
				printf("[Client: %d] received SERVER_FULL packet from server\n", sm_iClientId);

				kitten::Event* eventData = new kitten::Event(kitten::Event::Network_Host_Not_Ready);
				eventData->putString(NETWORK_MESSAGE_KEY, "Server Full");
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Network_Host_Not_Ready, eventData);

				i += BASIC_PACKET_SIZE;
				break;
			}
			case PacketTypes::SESSIONS_FULL:
			{
				printf("[Client: %d] received SESSIONS_FULL packet from server\n", sm_iClientId);

				// Send the Server info to the Quickplay class
				kitten::Event* eventData = new kitten::Event(kitten::Event::Update_Server_Info);
				eventData->putInt(SERVER_STATUS_KEY, SERVER_STATUS_FULL);
				eventData->putInt(SERVER_PLAYER_COUNT_KEY, -1);
				eventData->putInt(SERVER_ACTIVE_SESSIONS_KEY, -1);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Update_Server_Info, eventData);

				i += BASIC_PACKET_SIZE;
				break;
			}
			default:
				std::stringstream message;
				message << "Client:" << sm_iClientId << " received error in packet types, received: " << packetType;
				m_log->logMessage(message.str());

				printf("[Client: %d] received %d; error in packet types\n", sm_iClientId, packetType);
				i += (unsigned int)data_length;
				break;
			}
		}
	}

	void ClientGame::useAbility(AbilityPacket& p_packet)
	{
		std::string strAbilityName = p_packet.m_abilityName;
		printf("[Client: %d] using ability: %s\n", sm_iClientId, strAbilityName.c_str());

		ability::AbilityInfoPackage* info = new ability::AbilityInfoPackage();
		info->m_sourceClientId = p_packet.m_clientId;
		p_packet.insertIntoPackage(info);

		ability::AbilityManager::getInstance()->findAbility(strAbilityName)->effect(info);
	}

	void ClientGame::sendAbilityPacket(const std::string & p_strAbilityName, ability::AbilityInfoPackage * p_info)
	{
		AbilityPacket packet;
		packet.m_packetType = ABILITY_PACKET;
		packet.m_clientId = sm_iClientId;		
		packet.m_abilityNameLength = p_strAbilityName.size();
		packet.m_abilityName = p_strAbilityName;
		packet.extractFromPackage(p_info);

		char* data = new char[packet.getSize()];
		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = packet.getSize();
		packet.serialize(buffer);
		printf("[Client: %d] sending ABILITY_PACKET\n", sm_iClientId);
		//packet.print();

		NetworkServices::sendMessage(m_network->m_connectSocket, data, packet.getSize());
		delete[] data;

		std::stringstream message;
		message << "Client:" << sm_iClientId << " sending an ability:";
		m_log->logMessage(message.str());

		std::string abilityInfo = packet.getFormattedAbilityInfo();
		m_log->logMessage(abilityInfo);
	}

	void ClientGame::setCastTime(AbilityPacket& p_packet)
	{
		std::string strAbilityName = p_packet.m_abilityName;
		printf("[Client: %d] setting cast for: %s\n", sm_iClientId, strAbilityName.c_str());

		ability::AbilityInfoPackage* info = new ability::AbilityInfoPackage();
		info->m_sourceClientId = p_packet.m_clientId;
		p_packet.insertIntoPackage(info);

		unit::AbilityDescription* ad = info->m_source->m_ADMap[p_packet.m_abilityName];
		info->m_source->setCast(ad, info);
	}

	void ClientGame::sendCastTimeAbilityPacket(unit::AbilityDescription * p_ad, ability::AbilityInfoPackage * p_info)
	{
		AbilityPacket packet;
		packet.m_packetType = CAST_TIME_ABILITY_PACKET;
		packet.m_clientId = sm_iClientId;
		packet.m_abilityNameLength = p_ad->m_stringValue[ABILITY_NAME].size();
		packet.m_abilityName = p_ad->m_stringValue[ABILITY_NAME];
		packet.extractFromPackage(p_info);

		char* data = new char[packet.getSize()];
		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = packet.getSize();
		packet.serialize(buffer);
		printf("[Client: %d] sending CAST_TIME_ABILITY_PACKET\n", sm_iClientId);

		NetworkServices::sendMessage(m_network->m_connectSocket, data, packet.getSize());
		delete[] data;

		std::stringstream message;
		message << "Client:" << sm_iClientId << " sending a CastTime ability:";
		m_log->logMessage(message.str());

		std::string abilityInfo = packet.getFormattedAbilityInfo();
		m_log->logMessage(abilityInfo);
	}

	void ClientGame::sendStartingData()
	{
		char commanderData[UNIT_PACKET_SIZE];
		Buffer commanderDataBuffer;
		commanderDataBuffer.m_data = commanderData;
		commanderDataBuffer.m_size = UNIT_PACKET_SIZE;

		UnitPacket commanderDataPacket;
		commanderDataPacket.m_packetType = STARTING_COMMANDER_DATA;
		commanderDataPacket.m_clientId = sm_iClientId;
		commanderDataPacket.m_unitId = DeckInitializingComponent::getActiveInstance()->getDeckData()->commanderID;

		// Manually setting Commander spawn points closer for easier testing
		//kitten::K_GameObject* tile = BoardManager::getInstance()->getTile(9 - (sm_iClientId * 4), 8);
		kitten::K_GameObject* tile = BoardManager::getInstance()->getSpawnPoint(sm_iClientId);
		commanderDataPacket.m_posX = tile->getComponent<TileInfo>()->getPosX();
		commanderDataPacket.m_posY = tile->getComponent<TileInfo>()->getPosY();

		commanderDataPacket.serialize(commanderDataBuffer);
		NetworkServices::sendMessage(m_network->m_connectSocket, commanderData, UNIT_PACKET_SIZE);

		std::stringstream message;
		message << "Client:" << sm_iClientId << " sending STARTING_COMMANDER_DATA\n";
		message << "\tUnit ID:" << commanderDataPacket.m_unitId << ", X:" << commanderDataPacket.m_posX << ", Y:" << commanderDataPacket.m_posY;
		m_log->logMessage(message.str());

		// Only the host map matters
		if (sm_iClientId == 0)
		{
			char mapData[MAP_DATA_PACKET_SIZE];
			Buffer mapDataBuffer;
			mapDataBuffer.m_data = mapData;
			mapDataBuffer.m_size = MAP_DATA_PACKET_SIZE;

			MapDataPacket mapDataPacket;
			mapDataPacket.m_packetType = MAP_DATA;
			mapDataPacket.m_clientId = sm_iClientId;
			mapDataPacket.m_mapId = BoardManager::getInstance()->getMapId();

			mapDataPacket.serialize(mapDataBuffer);
			NetworkServices::sendMessage(m_network->m_connectSocket, mapData, MAP_DATA_PACKET_SIZE);
		}
	}

	void ClientGame::boardLoadedListener(kitten::Event::EventType p_type, kitten::Event* p_event)
	{
		// Board loaded before getting ID from the server, so set flag so we can sendStartingData when we get our ID
		if (sm_iClientId < 0)
		{
			m_boardLoaded = true;
		}
		else // Otherwise, we have already gotten our ID, now that the board is loaded, we can send our starting data
		{
			sendStartingData();
		}
	}

	bool ClientGame::checkSync(int p_x, int p_y)
	{
		TileInfo* sourceTile = BoardManager::getInstance()->getTile(p_x, p_y)->getComponent<TileInfo>();
		kitten::K_GameObject* sourceUnit = sourceTile->getUnit();

		kitten::K_GameObject* currentUnit = unit::InitiativeTracker::getInstance()->getCurrentUnit();

		if (sourceUnit == currentUnit)
			return true;
		else
			return false;
	}

	void ClientGame::sendDesyncedPacket()
	{
		char data[BASIC_PACKET_SIZE];

		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = BASIC_PACKET_SIZE;

		Packet packet;
		packet.m_packetType = DESYNCED;
		packet.m_clientId = networking::ClientGame::getClientId();

		packet.serialize(buffer);
		NetworkServices::sendMessage(m_network->m_connectSocket, data, BASIC_PACKET_SIZE);

		std::stringstream message;
		message << "Client:" << sm_iClientId << " sending DESYNCED";
		m_log->logMessage(message.str());
	}

	void ClientGame::sendSkipTurnPacket(unit::Unit* p_unit)
	{
		char data[SKIP_TURN_PACKET_SIZE];

		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = SKIP_TURN_PACKET_SIZE;

		SkipTurnPacket packet;
		packet.m_packetType = SKIP_TURN;
		packet.m_clientId = networking::ClientGame::getClientId();
		packet.m_unit = getPosFromUnit(p_unit);

		packet.serialize(buffer);
		NetworkServices::sendMessage(m_network->m_connectSocket, data, SKIP_TURN_PACKET_SIZE);

		std::stringstream message;
		message << "Client:" << sm_iClientId << " sending SKIP_TURN\n";
		message << "\tUnit Pos: (" << packet.m_unit.posX << ", " << packet.m_unit.posY << ")";
		m_log->logMessage(message.str());
	}

	kitten::K_GameObject* ClientGame::summonUnit(int p_iClientId, int p_iUnitId, int p_iPosX, int p_iPosY)
	{
		// Create the unit GO and set its position
		kitten::K_GameObject* unitGO = unit::UnitSpawn::getInstance()->spawnUnitObject(p_iUnitId);

		// Reset the client ID to the client that summoned the unit
		unitGO->getComponent<unit::UnitMove>()->setTile(p_iPosX, p_iPosY);
		unitGO->getComponent<unit::Unit>()->m_clientId = p_iClientId;

		std::stringstream message;
		message << "Client:" << sm_iClientId << " summoning unit (ClientGame::summonUnit, not through Summon ability)\n";
		message << "\tUnit ID:" << p_iUnitId << ", X:" << p_iPosX << ", Y:" << p_iPosY;
		m_log->logMessage(message.str());
		return unitGO;
	}

	void ClientGame::sendSummonUnitPacket(int p_iUnitId, int p_iPosX, int p_iPosY)
	{
		char data[UNIT_PACKET_SIZE];

		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = UNIT_PACKET_SIZE;

		UnitPacket packet;
		packet.m_packetType = PacketTypes::SUMMON_UNIT;
		packet.m_clientId = sm_iClientId;
		packet.m_unitId = p_iUnitId;
		packet.m_posX = p_iPosX;
		packet.m_posY = p_iPosY;
		
		packet.serialize(buffer);
		NetworkServices::sendMessage(m_network->m_connectSocket, data, UNIT_PACKET_SIZE);

		std::stringstream message;
		message << "Client:" << sm_iClientId << " sending SUMMON_UNIT\n";
		message << "\tUnit ID:" << p_iUnitId << ", X:" << p_iPosX << ", Y:" << p_iPosY;
		m_log->logMessage(message.str());
	}

	void ClientGame::sendTextChatMessagePacket(const std::string& p_message)
	{
		char data[TEXTCHAT_MESSAGE_PACKET_SIZE];

		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = TEXTCHAT_MESSAGE_PACKET_SIZE;

		TextChatMessagePacket packet;
		packet.m_packetType = TEXTCHAT_MESSAGE;
		packet.m_clientId = sm_iClientId;
		packet.addMessage(p_message);
		packet.serialize(buffer);
		int result = NetworkServices::sendMessage(m_network->m_connectSocket, data, TEXTCHAT_MESSAGE_PACKET_SIZE);

		std::stringstream message;
		message << "Client:" << sm_iClientId << " sending TEXTCHAT_MESSAGE\n";
		message << "\tMessage: " << p_message;
		m_log->logMessage(message.str());
	}

	int ClientGame::sendBasicPacket(PacketTypes p_packetType)
	{
		char data[BASIC_PACKET_SIZE];

		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = BASIC_PACKET_SIZE;

		Packet packet;
		packet.m_packetType = p_packetType;
		packet.m_clientId = sm_iClientId;

		packet.serialize(buffer);

		if (p_packetType != PacketTypes::PING_SOCKET)
		{
			std::stringstream message;
			message << "Client:" << sm_iClientId << " sending basic packetType: " << p_packetType;
			m_log->logMessage(message.str());
		}

		return NetworkServices::sendMessage(m_network->m_connectSocket, data, BASIC_PACKET_SIZE);
	}
}