#include "GameSession.h"

namespace networking
{
	GameSession::GameSession(int p_sessionId, int p_maxPlayers, ServerNetwork* p_network)
		: 
		m_sessionId(p_sessionId), 
		m_maxPlayers(p_maxPlayers),
		m_network(p_network),
		m_sessionClientId(0),
		m_state(SessionState::Inactive)
	{
		printf("GameSession:%d created\n", m_sessionId);
	}

	GameSession::~GameSession()
	{
		
	}

	void GameSession::shutdown()
	{
		removeAllPlayers();
		m_sessionClientId = 0;
		m_state = SessionState::Inactive;
	}

	void GameSession::update()
	{
		receiveDataFromPlayers();
	}

	void GameSession::receiveDataFromPlayers()
	{
		// go through all clients to see if they are trying to send data
		std::map<int, ClientInfo*>::iterator iter;

		for (iter = m_currentPlayers.begin(); iter != m_currentPlayers.end(); /* no increment */)
		{
			// Get the values and then increment early in the case that something is removed from the map
			int sessionClientId = iter->first;
			ClientInfo* client = iter->second;
			++iter;

			int data_length = m_network->receiveData(client, m_networkData);
			/*if (data_length == SOCKET_ERROR)
			{
				removePlayer(client);
				continue;
			}*/
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
				defaultBuffer.m_data = &(m_networkData[i]);
				defaultBuffer.m_size = BASIC_PACKET_SIZE;

				Packet defaultPacket;
				defaultPacket.deserialize(defaultBuffer);

				switch (defaultPacket.m_packetType) {
				case CLIENT_DISCONNECT:
				{
					i += BASIC_PACKET_SIZE;
					printf("Server received CLIENT_DISCONNECT from [Client: %d]\n", sessionClientId);
					removePlayer(client);

					break;
				}
				case ABILITY_PACKET:
				{
					printf("Server received ABILITY_PACKET from [Client: %d]\n", sessionClientId);
					Buffer buffer;
					buffer.m_data = &(m_networkData[i]);
					buffer.m_size = MAX_PACKET_SIZE;

					AbilityPacket packet;
					packet.deserialize(buffer);
					int packetTotalBytes = packet.getBytes();
					i += packetTotalBytes;

					char* data = new char[packetTotalBytes];
					Buffer newBuffer;
					newBuffer.m_data = data;
					newBuffer.m_size = packetTotalBytes;
					packet.serialize(newBuffer);
					sendToOthers(client, data, packetTotalBytes);
					delete[] data;

					break;
				}
				case CAST_TIME_ABILITY_PACKET:
				{
					printf("Server received CAST_TIME_ABILITY_PACKET from [Client: %d]\n", sessionClientId);
					Buffer buffer;
					buffer.m_data = &(m_networkData[i]);
					buffer.m_size = MAX_PACKET_SIZE;

					AbilityPacket packet;
					packet.deserialize(buffer);
					int packetTotalBytes = packet.getBytes();
					i += packetTotalBytes;

					char* data = new char[packetTotalBytes];
					Buffer newBuffer;
					newBuffer.m_data = data;
					newBuffer.m_size = packetTotalBytes;
					packet.serialize(newBuffer);
					sendToOthers(client, data, packetTotalBytes);
					delete[] data;

					break;
				}
				case SUMMON_UNIT:
				{
					printf("Server received SUMMON_UNIT packet from [Client: %d]\n", sessionClientId);
					Buffer buffer;
					buffer.m_data = &(m_networkData[i]);
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
					sendToOthers(client, data, UNIT_PACKET_SIZE);

					break;
				}
				case READY_CHECK:
				{
					printf("Server received READY_CHECK packet from [Client: %d]\n", sessionClientId);

					if (m_clientsReadyChecked < m_maxPlayers)
					{
						m_clientsReadyChecked++;

						if (m_clientsReadyChecked == m_maxPlayers)
						{
							char data[BASIC_PACKET_SIZE];
							Buffer newBuffer;
							newBuffer.m_data = data;
							newBuffer.m_size = BASIC_PACKET_SIZE;

							Packet basicPacket;
							basicPacket.m_packetType = READY_CHECK;
							basicPacket.m_clientId = -1;

							basicPacket.serialize(newBuffer);
							sendToAll(data, BASIC_PACKET_SIZE);
						}
					}
					i += BASIC_PACKET_SIZE;
					break;
				}
				case STARTING_COMMANDER_DATA:
				{
					printf("Server received STARTING_COMMANDER_DATA packet from [Client: %d]\n", sessionClientId);
					Buffer buffer;
					buffer.m_data = &(m_networkData[i]);
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
						sendToAll(data, STARTING_COMMANDERS_PACKET_SIZE);
					}
					i += UNIT_PACKET_SIZE;
					break;
				}
				case TEXTCHAT_MESSAGE:
				{
					i += TEXTCHAT_MESSAGE_PACKET_SIZE;
					printf("Server received TEXTCHAT_MESSAGE packet from [Client: %d]\n", sessionClientId);
					sendToOthers(client, defaultBuffer.m_data, TEXTCHAT_MESSAGE_PACKET_SIZE);
					break;
				}
				case SKIP_TURN:
				{
					i += SKIP_TURN_PACKET_SIZE;
					printf("Server received SKIP_TURN packet from [Client: %d]\n", sessionClientId);
					sendToOthers(client, defaultBuffer.m_data, SKIP_TURN_PACKET_SIZE);
					break;
				}
				case PING_SOCKET:
				{
					i += BASIC_PACKET_SIZE;
					// Server pinged to see if it is still active
					break;
				}
				case GAME_TURN_START:
				case DESYNCED:
				{
					i += BASIC_PACKET_SIZE;
					printf("Server received BasicPacket PacketType: %d from [Client: %d]\n", defaultPacket.m_packetType, sessionClientId);

					sendToAll(defaultBuffer.m_data, BASIC_PACKET_SIZE);
					break;
				}
				default:
					printf("error in packet types received from [Client %d], value: %d\n", sessionClientId, defaultPacket.m_packetType);
					i += (unsigned int)data_length;
					break;
				}
			}
		}
	}

	void GameSession::sendToClient(ClientInfo* p_client, char* p_packets, int p_totalSize)
	{
		m_network->sendToSocket(p_client, p_packets, p_totalSize);
	}

	void GameSession::sendToOthers(ClientInfo* p_client, char* p_packets, int p_totalSize)
	{
		for (auto it = m_currentPlayers.begin(); it != m_currentPlayers.end(); ++it)
		{
			if ((*it).second != p_client)
			{
				m_network->sendToSocket((*it).second, p_packets, p_totalSize);
			}
		}
	}

	void GameSession::sendToAll(char* p_packets, int p_totalSize)
	{
		for (auto it = m_currentPlayers.begin(); it != m_currentPlayers.end(); ++it)
		{
			m_network->sendToSocket((*it).second, p_packets, p_totalSize);			
		}
	}

	// Returns true if player added, or false if already in session or no space in session
	bool GameSession::addPlayer(ClientInfo* p_info)
	{
		if (m_currentPlayers.size() < m_maxPlayers)
		{
			auto result = m_currentPlayers.insert(std::make_pair(m_sessionClientId, p_info));
			if (result.second)
			{
				p_info->m_gameSessionClientId = m_sessionClientId;
				p_info->m_gameSessionId = m_sessionId;

				// Send a packet to the client to notify them what their Session ID is
				char packetData[BASIC_PACKET_SIZE];

				Buffer buffer;
				buffer.m_data = packetData;
				buffer.m_size = BASIC_PACKET_SIZE;

				Packet packet;
				packet.m_packetType = SEND_CLIENT_ID;
				packet.m_clientId = m_sessionClientId;

				packet.serialize(buffer);
				m_network->sendToSocket(p_info, packetData, BASIC_PACKET_SIZE);

				m_sessionClientId++;
				printf("Player added to GameSession:%d\n", m_sessionId);
			}
			else
			{
				printf("Player not added to GameSession:%d\n", m_sessionId);
			}
			checkPlayers();
			return result.second;
		}
		else
		{
			printf("Player not added to GameSession:%d, session full\n", m_sessionId);
			return false;
		}
	}

	// Returns std::map::erase, the number of elements erased
	// Since values are unique, it is either 0 or 1, which means false or true
	bool GameSession::removePlayer(ClientInfo* p_info)
	{
		if (p_info->m_gameSessionId == m_sessionId)
		{
			int erased = m_currentPlayers.erase(p_info->m_gameSessionClientId);
			p_info->m_gameSessionClientId = -1;
			p_info->m_gameSessionId = -1;
			checkPlayers();
			printf("Player removed from GameSession:%d\n", m_sessionId);
			return erased;
		}
		printf("Player not removed from GameSession:%d, they were not from this GameSession\n", m_sessionId);
		return false;
	}

	void GameSession::removeAllPlayers()
	{
		for (auto it = m_currentPlayers.begin(); it != m_currentPlayers.end(); ++it)
		{
			ClientInfo* client = (*it).second;

			char packetData[BASIC_PACKET_SIZE];
			Buffer buffer;
			buffer.m_data = packetData;
			buffer.m_size = BASIC_PACKET_SIZE;

			Packet packet;
			packet.m_packetType = SERVER_SHUTDOWN;
			packet.m_clientId = -1;

			packet.serialize(buffer);
			m_network->sendToSocket(client, packetData, BASIC_PACKET_SIZE);
		}
		m_currentPlayers.clear();
		m_state = SessionState::Inactive;
		printf("All players removed from GameSession:%d\n", m_sessionId);
	}

	void GameSession::checkPlayers()
	{
		switch (m_state)
		{
			case SessionState::Searching:
			{
				if (m_currentPlayers.size() == m_maxPlayers)
				{
					m_state = SessionState::Active;
					printf("GameSession:%d set to Active\n", m_sessionId);
				}
				break;
			}
			case SessionState::Active:
			{
				if (m_currentPlayers.size() < m_maxPlayers)
				{
					removeAllPlayers();
					m_state = SessionState::Inactive;
					printf("GameSession:%d set to Inactive\n", m_sessionId);
				}
				break;
			}
			case SessionState::Inactive:
			{
				if (m_currentPlayers.size() > 0)
				{
					m_state = SessionState::Searching;
					printf("GameSession:%d set to Searching\n", m_sessionId);
				}
				break;
			}
		}
	}
}