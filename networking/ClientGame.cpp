// ClientGame
//
// Initializes ClientNetwork and sets up anything that needs to be setup on init connection
// Has update loop to always check for incoming data from the server
//
// @Ken

#include "networking\ClientGame.h"
#include <assert.h>
#include <iostream>
#include <unordered_map>

// Kibble
#include "kibble\databank\databank.hpp"

// Units
#include "kitten\K_GameObjectManager.h"
#include "unit\UnitSpawn.h"
#include "unit\unitComponent\UnitMove.h"
#include "unit\UnitMonitor.h"

// Unit movement
#include "board\BoardManager.h"
#include "unit\unitComponent\UnitMove.h"

#include "components\DeckInitializingComponent.h"

namespace networking
{
	ClientGame* ClientGame::sm_clientGameInstance = nullptr;
	bool ClientGame::sm_networkValid = false;
	int ClientGame::sm_iClientId = -1;

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

	ClientGame::ClientGame(const std::string &p_strAddr) : m_bGameTurnStart(false)
	{
		setupNetwork(p_strAddr);
	}


	ClientGame::~ClientGame()
	{
		if (m_network != nullptr)
		{
			delete m_network;
		}
		sm_networkValid = false;
		sm_iClientId = -1;
	}

	void ClientGame::setupNetwork(const std::string &p_strAddr)
	{
		m_network = new ClientNetwork();

		if (m_network->init(p_strAddr))
		{ 
			sm_networkValid = true;

			// Client connects and sends INIT_CONNECTION packet
			char packet_data[BASIC_PACKET_SIZE];

			Buffer buffer;
			buffer.m_data = packet_data;
			buffer.m_size = BASIC_PACKET_SIZE;

			Packet packet;
			packet.m_packetType = INIT_CONNECTION;
			packet.serialize(buffer);

			NetworkServices::sendMessage(m_network->m_connectSocket, packet_data, BASIC_PACKET_SIZE);
		}
		else
		{
			printf("Client Network init error: %s\n", m_network->getError().c_str());

			delete m_network;
			m_network = nullptr;

			sm_networkValid = false;
		}
	}

	void ClientGame::disconnectFromNetwork(bool p_bServerShutdown)
	{
		// If Server sent disconnect then no need to send packet to server
		if (!p_bServerShutdown)
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
		}

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
		int data_length = m_network->receivePackets(m_network_data);

		if (data_length <= 0)
		{
			//no data recieved
			return;
		}
		//printf("Bytes received: %d\n", data_length);
		int i = 0;
		PacketTypes packetType;

		while (i < (unsigned int)data_length)
		{
			//printf("Start of Loop Count i: %d\n", i);
			Buffer defaultBuffer;
			defaultBuffer.m_data = &(m_network_data[i]);
			defaultBuffer.m_size = BASIC_PACKET_SIZE;

			Packet defaultPacket;
			defaultPacket.deserialize(defaultBuffer);

			packetType = (PacketTypes)defaultPacket.m_packetType;
			switch (packetType) {

			case PacketTypes::SEND_CLIENT_ID:
			{
				i += BASIC_PACKET_SIZE;
				sm_iClientId = defaultPacket.m_clientId;

				// Send starting data
				char commanderData[UNIT_PACKET_SIZE];
				Buffer commanderDataBuffer;
				commanderDataBuffer.m_data = commanderData;
				commanderDataBuffer.m_size = UNIT_PACKET_SIZE;

				UnitPacket commanderDataPacket;
				commanderDataPacket.m_packetType = STARTING_COMMANDER_DATA;
				commanderDataPacket.m_clientId = sm_iClientId;
				commanderDataPacket.m_unitId = DeckInitializingComponent::getActiveInstance()->getDeckData()->commanderID;

				// Manually setting Commander spawn points closer for easier testing
				kitten::K_GameObject* tile = BoardManager::getInstance()->getTile(9 - (sm_iClientId * 4), 8);
				//kitten::K_GameObject* tile = BoardManager::getInstance()->getSpawnPoint(sm_iClientId);
				commanderDataPacket.m_posX = tile->getComponent<TileInfo>()->getPosX();
				commanderDataPacket.m_posY = tile->getComponent<TileInfo>()->getPosY();

				commanderDataPacket.serialize(commanderDataBuffer);
				NetworkServices::sendMessage(m_network->m_connectSocket, commanderData, UNIT_PACKET_SIZE);

				break;
			}
			case PacketTypes::SERVER_SHUTDOWN:
			{
				printf("[Client: %d] received SERVER_SHUTDOWN packet from server\n", sm_iClientId);

				i += BASIC_PACKET_SIZE;
				disconnectFromNetwork(true);

				// Display disconnect screen; Server received manual disconnect from server
				kitten::Event* eventData = new kitten::Event(kitten::Event::End_Game_Screen);
				eventData->putInt(GAME_END_RESULT, PLAYER_DISCONNECTED);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::End_Game_Screen, eventData);
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

				if (checkSync(packet.m_sourceUnit))
				{
					useAbility(packet);
				} 
				else
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

				if (checkSync(skipTurnPacket.m_unitId))
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
				printf("[Client: %d] received packet GAME_TURN_START from server\n", sm_iClientId);
				i += BASIC_PACKET_SIZE;

				if (!m_bGameTurnStart)
				{
					unit::InitiativeTracker::getInstance()->gameTurnStart();
					m_bGameTurnStart = true;
				}
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

				UnitInfo commander0 = commanderPacket.commander0;
				kitten::K_GameObject* commanderGO0 = summonUnit(commander0.clientId, commander0.unitId, commander0.posX, commander0.posY);

				UnitInfo commander1 = commanderPacket.commander1;
				kitten::K_GameObject* commanderGO1 = summonUnit(commander1.clientId, commander1.unitId, commander1.posX, commander1.posY);

				if (commander0.clientId == sm_iClientId)
				{
					m_commander = commanderGO0->getComponent<unit::Unit>();
				}
				else
				{
					m_commander = commanderGO1->getComponent<unit::Unit>();
				}
				kitten::Event* eventData = new kitten::Event(kitten::Event::Client_Commander_Loaded);
				eventData->putGameObj(COMMANDER_GO_KEY, &m_commander->getGameObject());
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Client_Commander_Loaded, eventData);

				unit::InitiativeTracker::getInstance()->gameTurnStart();
				m_bGameTurnStart = true;

				i += STARTING_COMMANDERS_PACKET_SIZE;
				break;
			}
			case PacketTypes::DESYNCED:
			{
				printf("[Client: %d] received DESYNCED packet from server\n", sm_iClientId);
				i += BASIC_PACKET_SIZE;

				kitten::Event* eventData = new kitten::Event(kitten::Event::End_Game_Screen);
				eventData->putInt(GAME_END_RESULT, CLIENT_DESYNCED);
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::End_Game_Screen, eventData);

				break;
			}
			default:
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
		info->m_source = getUnitGameObject(p_packet.m_sourceUnit)->getComponent<unit::Unit>();
		info->m_targets = p_packet.getTargetUnits();
		info->m_intValue = p_packet.getIntValues();
		info->m_targetTilesGO = p_packet.getTargetTiles();
		info->m_sourceClientId = p_packet.m_clientId;

		ability::AbilityManager::getInstance()->findAbility(strAbilityName)->effect(info);
	}

	void ClientGame::sendAbilityPacket(const std::string & p_strAbilityName, ability::AbilityInfoPackage * p_info)
	{
		AbilityPacket packet;
		packet.m_packetType = ABILITY_PACKET;
		packet.m_clientId = sm_iClientId;
		packet.m_sourceUnit = getUnitGameObjectIndex(&p_info->m_source->getGameObject());
		packet.addTargetUnits(p_info->m_targets);
		packet.addIntValues(p_info->m_intValue);
		packet.addTargetTiles(p_info->m_targetTilesGO);
		packet.m_abilityNameLength = p_strAbilityName.size();
		packet.m_abilityName = p_strAbilityName;

		char* data = new char[packet.getSize()];
		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = packet.getSize();
		packet.serialize(buffer);
		printf("[Client: %d] sending ABILITY_PACKET\n", sm_iClientId);
		//packet.print();

		NetworkServices::sendMessage(m_network->m_connectSocket, data, packet.getSize());
		delete[] data;
	}

	bool ClientGame::checkSync(int p_unitId)
	{
		kitten::K_GameObject* sourceUnit = getUnitGameObject(p_unitId);
		unit::Unit* source = sourceUnit->getComponent<unit::Unit>();

		kitten::K_GameObject* currentUnit = unit::InitiativeTracker::getInstance()->getCurrentUnit();
		unit::Unit* host = currentUnit->getComponent<unit::Unit>();

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
		packet.m_unitId = getUnitGameObjectIndex(&p_unit->getGameObject());

		packet.serialize(buffer);
		NetworkServices::sendMessage(m_network->m_connectSocket, data, SKIP_TURN_PACKET_SIZE);
	}

	kitten::K_GameObject* ClientGame::summonUnit(int p_iClientId, int p_iUnitId, int p_iPosX, int p_iPosY)
	{
		// Create the unit GO and set its position
		kitten::K_GameObject* unitGO = unit::UnitSpawn::getInstance()->spawnUnitObject(p_iUnitId);

		// Reset the client ID to the client that summoned the unit
		unitGO->getComponent<unit::UnitMove>()->setTile(p_iPosX, p_iPosY);
		unitGO->getComponent<unit::Unit>()->m_clientId = p_iClientId;

		//unit::UnitMonitor::getInstanceSafe()->printUnit(testDummy);
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
	}

	void ClientGame::sendBasicPacket(PacketTypes p_packetType)
	{
		char data[BASIC_PACKET_SIZE];

		Buffer buffer;
		buffer.m_data = data;
		buffer.m_size = BASIC_PACKET_SIZE;

		Packet packet;
		packet.m_packetType = p_packetType;
		packet.m_clientId = sm_iClientId;

		packet.serialize(buffer);
		NetworkServices::sendMessage(m_network->m_connectSocket, data, BASIC_PACKET_SIZE);
	}

	int ClientGame::getUnitGameObjectIndex(kitten::K_GameObject* p_unit)
	{
		for (auto it = m_unitGOList.begin(); it != m_unitGOList.end(); ++it)
		{
			if (it->second == p_unit)
			{
				return it->first;
			}
		}
		return -1; // Not found
	}

	kitten::K_GameObject* ClientGame::getUnitGameObject(int p_iIndex)
	{
		auto it = m_unitGOList.find(p_iIndex);
		if (it != m_unitGOList.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void ClientGame::addUnitGameObject(kitten::K_GameObject* p_unit)
	{
		m_unitGOList.insert(std::make_pair(m_iUnitIndex, p_unit));
		m_iUnitIndex++;
	}

	void ClientGame::removeUnitGameObject(int p_iUnitIndex)
	{
		auto it = m_unitGOList.find(p_iUnitIndex);
		if (it != m_unitGOList.end())
		{
			m_unitGOList.erase(it);
		}
	}
}