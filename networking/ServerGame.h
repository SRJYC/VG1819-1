#pragma once
#include "networking/ServerNetwork.h"
#include "networking/NetworkData.h"
#include <vector>

namespace networking
{
	class ServerGame
	{

	private:
		static ServerGame* sm_serverGameInstance;
		ServerGame();
		~ServerGame();
	public:

		static void createInstance();
		static void destroyInstance();
		static ServerGame* getInstance();
		static bool isNetworkValid() { return sm_networkValid; }

		void setupNetwork();
		void shutdownNetwork();

		void update();
		void receiveFromClients();
		
		void sendSummonedUnitPacket(unsigned int p_iClientId, UnitPacket p_packet);

		void flagShutdown(bool p_flag) { m_shutdown = p_flag; }

	private:

		// IDs for the clients connecting for table in ServerNetwork 
		unsigned int client_id;

		// The ServerNetwork object 
		ServerNetwork* m_network;

		// data buffer
		char m_network_data[MAX_PACKET_SIZE];
		bool m_shutdown;

		static bool sm_networkValid;

		// Vector to hold the starting data of all player's Commanders
		std::vector<UnitPacket> m_commanders;
	};
}