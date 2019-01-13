/*
* ServerGame
* The Server that handles game data and updates connected
* clients
*/
#include "ServerGame.h"

#include "ClientGame.h"
#include "kitten\event_system\EventManager.h"

namespace networking
{
	//unsigned int ServerGame::client_id;

	ServerGame* ServerGame::sm_serverGameInstance = nullptr;
	bool ServerGame::sm_networkValid = false;

	// Creates the singleton instance.
	void ServerGame::createInstance()
	{
		assert(sm_serverGameInstance == nullptr);
		sm_serverGameInstance = new ServerGame();
	}

	// Destroys the singleton instance.
	void ServerGame::destroyInstance()
	{
		assert(sm_serverGameInstance != nullptr);
		delete sm_serverGameInstance;
		sm_serverGameInstance = nullptr;
		sm_networkValid = false;
	}

	// Access to singleton instance.
	ServerGame* ServerGame::getInstance()
	{
		//assert(sm_serverGameInstance);
		return sm_serverGameInstance;
	}

	ServerGame::ServerGame() : m_shutdown(false)
	{
		// id's to assign clients for our table
		client_id = 0;

		// set up the server network to listen 
		setupNetwork();
	}

	ServerGame::~ServerGame()
	{
		if (m_network != nullptr)
		{
			delete m_network;
		}
	}

	void ServerGame::setupNetwork()
	{
		m_network = new ServerNetwork();

		if (!m_network->init())
		{
			printf("Server Network init error: %s\n", m_network->getError().c_str());

			delete m_network;
			m_network = nullptr;

			sm_networkValid = false;
		}
		else
		{
			sm_networkValid = true;
		}

	}

	void ServerGame::shutdownNetwork()
	{
		// Shutdown ServerNetwork
		if (m_network != nullptr)
		{
			char data[BASIC_PACKET_SIZE];

			Buffer buffer;
			buffer.m_data = data;
			buffer.m_size = BASIC_PACKET_SIZE;

			Packet packet;
			packet.m_packetType = SERVER_SHUTDOWN;
			packet.serialize(buffer);

			m_network->sendToAll(data, BASIC_PACKET_SIZE);

			delete m_network;
			m_network = nullptr;
		}

		sm_networkValid = false;
	}

	void ServerGame::update()
	{
		// get new clients
		if (m_network->acceptNewClient(client_id))
		{
			printf("[Client: %d] has been connected to the server\n", client_id);

			client_id++;
		}

		receiveFromClients();

		if (m_shutdown)
		{
			shutdownNetwork();
		}
	}

	void ServerGame::receiveFromClients()
	{
		// go through all clients to see if they are trying to send data
		std::map<unsigned int, SOCKET>::iterator iter;

		for (iter = m_network->m_sessions.begin(); iter != m_network->m_sessions.end(); iter++)
		{
			int data_length = m_network->receiveData(iter->first, m_network_data);

			if (data_length <= 0)
			{
				//no data recieved
				continue;
			}

			int i = 0;
			while (i < (unsigned int)data_length)
			{
				// Take all incoming packets as Packet initially to read the packetType
				Buffer defaultBuffer;
				defaultBuffer.m_data = &(m_network_data[i]);
				defaultBuffer.m_size = BASIC_PACKET_SIZE;

				Packet defaultPacket;
				defaultPacket.deserialize(defaultBuffer);

				switch (defaultPacket.m_packetType) {

					case INIT_CONNECTION:
					{
						i += BASIC_PACKET_SIZE;
						printf("Server received init packet from [Client: %d]\n", iter->first);

						// Send a packet to the client to notify them what their ID is
						int clientId = iter->first;
						char packetData[BASIC_PACKET_SIZE];

						Buffer buffer;// = NetworkServices::createBuffer(BASIC_PACKET_SIZE);
						buffer.m_data = packetData;
						buffer.m_size = BASIC_PACKET_SIZE;

						Packet packet;
						packet.m_packetType = SEND_CLIENT_ID;
						packet.m_clientId = clientId;

						packet.serialize(buffer);
						m_network->sendToClient(clientId, packetData, BASIC_PACKET_SIZE);
						break;
					}
					case CLIENT_DISCONNECT:
					{
						i += BASIC_PACKET_SIZE;
						unsigned int clientId = iter->first;
						printf("Server received CLIENT_DISCONNECT from [Client: %d]\n", clientId);						
						m_network->removeClient(clientId);
						kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Disconnect_From_Network, nullptr);

						// Display disconnect screen; Server received manual disconnect from client
						kitten::Event* eventData = new kitten::Event(kitten::Event::End_Game_Screen);
						eventData->putInt(GAME_END_RESULT, PLAYER_DISCONNECTED);
						kitten::EventManager::getInstance()->triggerEvent(kitten::Event::End_Game_Screen, eventData);

						break;
					}
					case ABILITY_PACKET:
					{
						printf("Server received ABILITY_PACKET from [Client: %d]\n", iter->first);
						Buffer buffer;
						buffer.m_data = &(m_network_data[i]);
						buffer.m_size = MAX_PACKET_SIZE;

						AbilityPacket packet;
						packet.deserialize(buffer);
						int packetTotalBytes = packet.getBytes();
						i += packetTotalBytes;
						packet.print();

						char* data = new char[packetTotalBytes];
						Buffer newBuffer;
						newBuffer.m_data = data;
						newBuffer.m_size = packetTotalBytes;
						packet.serialize(newBuffer);
						m_network->sendToOthers(iter->first, data, packetTotalBytes);
						delete[] data;

						break;
					}
					case SUMMON_UNIT:
					{
						printf("Server received SUMMON_UNIT packet from [Client: %d]\n", iter->first);
						Buffer buffer;
						buffer.m_data = &(m_network_data[i]);
						buffer.m_size = UNIT_PACKET_SIZE;

						UnitPacket summonUnitPacket;
						summonUnitPacket.deserialize(buffer);
						i += UNIT_PACKET_SIZE;
						printf("Server sending Unit index: %d, posX: %d, posY: %d\n", summonUnitPacket.m_unitId, summonUnitPacket.m_posX, summonUnitPacket.m_posY);
						
						char data[UNIT_PACKET_SIZE];
						Buffer newBuffer;
						newBuffer.m_data = data;
						newBuffer.m_size = UNIT_PACKET_SIZE;
						summonUnitPacket.serialize(newBuffer);
						m_network->sendToOthers(iter->first, data, UNIT_PACKET_SIZE);

						//sendSummonedUnitPacket(iter->first, summonUnitPacket);

						break;
					}
					case STARTING_COMMANDER_DATA:
					{
						printf("Server received STARTING_COMMANDER_DATA packet from [Client: %d]\n", iter->first);
						Buffer buffer;
						buffer.m_data = &(m_network_data[i]);
						buffer.m_size = UNIT_PACKET_SIZE;

						UnitPacket commanderDataPacket;
						commanderDataPacket.deserialize(buffer);

						m_commanders.push_back(commanderDataPacket);
						if (m_commanders.size() == 2)
						{
							char data[STARTING_COMMANDERS_PACKET_SIZE];
							Buffer newBuffer;
							newBuffer.m_data = data;
							newBuffer.m_size = STARTING_COMMANDERS_PACKET_SIZE;

							StartingCommandersPacket commandersPacket;
							commandersPacket.m_packetType = STARTING_COMMANDER_DATA;
							commandersPacket.m_clientId = -1; // -1 to signal originally from the Server

							commandersPacket.commander0.clientId = m_commanders[0].m_clientId;
							commandersPacket.commander0.unitId = m_commanders[0].m_unitId;
							commandersPacket.commander0.posX = m_commanders[0].m_posX;
							commandersPacket.commander0.posY = m_commanders[0].m_posY;

							commandersPacket.commander1.clientId = m_commanders[1].m_clientId;
							commandersPacket.commander1.unitId = m_commanders[1].m_unitId;
							commandersPacket.commander1.posX = m_commanders[1].m_posX;
							commandersPacket.commander1.posY = m_commanders[1].m_posY;

							commandersPacket.serialize(newBuffer);
							m_network->sendToAll(data, STARTING_COMMANDERS_PACKET_SIZE);
						}
						i += UNIT_PACKET_SIZE;
						break;
					}
					case SKIP_TURN:
					{
						i += SKIP_TURN_PACKET_SIZE;
						printf("Server received SKIP_TURN packet from [Client: %d]\n", iter->first);

						m_network->sendToOthers(iter->first, defaultBuffer.m_data, SKIP_TURN_PACKET_SIZE);
						break;
					}
					case GAME_TURN_START:
					case DESYNCED:
					{
						i += BASIC_PACKET_SIZE;
						printf("Server received BasicPacket PacketType: %d from [Client: %d]\n", defaultPacket.m_packetType, iter->first);

						m_network->sendToAll(defaultBuffer.m_data, BASIC_PACKET_SIZE);
						break;
					}
					default:
						printf("error in packet types received from client %d\n", iter->first);
						i += (unsigned int)data_length;						
						break;
				}
			}
		}
	}

	void ServerGame::sendSummonedUnitPacket(unsigned int p_iClientId, UnitPacket p_packet)
	{
		char packet_data[UNIT_PACKET_SIZE];

		//p_packet.serialize(packet_data);

		m_network->sendToOthers(p_iClientId, packet_data, UNIT_PACKET_SIZE);
	}
}