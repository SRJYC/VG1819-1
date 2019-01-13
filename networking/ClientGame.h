#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "networking\ClientNetwork.h"
#include "networking\NetworkData.h"

#include "kitten\K_GameObject.h"
#include <map>

// Ability 
#include "ability\AbilityInfoPackage.h"

namespace networking
{
	class ClientGame
	{
	private:
		ClientNetwork* m_network;

		char m_network_data[MAX_PACKET_SIZE];

		bool m_bServerCalling = false;
		bool m_bGameTurnStart = false;

		// Unit GO list so clients can have a reference to the same unit GO without having the same mem address
		std::map<int, kitten::K_GameObject*> m_unitGOList;
		int m_iUnitIndex = 0;

		unit::Unit* m_commander;

		static int sm_iClientId;
		static bool sm_networkValid;
		static ClientGame* sm_clientGameInstance;

		ClientGame(const std::string &p_strAddr);
		~ClientGame();

	public:
		static void createInstance(const std::string &p_strAddr = "127.0.0.1");
		static void destroyInstance();
		static ClientGame* getInstance();
		static bool isNetworkValid() { return sm_networkValid; }

		void setupNetwork(const std::string &p_strAddr = "127.0.0.1");
		void disconnectFromNetwork(bool p_bServerShutdown = false);

		void update();

		void useAbility(AbilityPacket& p_packet);
		void sendAbilityPacket(const std::string & p_strAbilityName, ability::AbilityInfoPackage * p_info);
		
		bool checkSync(int p_unitId);
		void sendDesyncedPacket();

		void sendSkipTurnPacket(unit::Unit* p_unit);

		kitten::K_GameObject* summonUnit(int p_iClientId, int p_iUnitId, int p_iPosX, int p_iPosY);
		void sendSummonUnitPacket(int p_iUnitId, int p_iPosX, int p_iPosY);

		void sendBasicPacket(PacketTypes p_packetType);

		int getUnitGameObjectIndex(kitten::K_GameObject* p_unit);
		kitten::K_GameObject* getUnitGameObject(int p_iIndex);
		void addUnitGameObject(kitten::K_GameObject* p_unit);
		void removeUnitGameObject(int p_iUnitIndex);

		unit::Unit* getCommander() { return m_commander; }

		static int getClientId() { return sm_iClientId; }
		bool isServerCalling() { return m_bServerCalling; }
		void setServerCalling(bool p_value) { m_bServerCalling = p_value; }
		bool isGameTurnStarted() { return m_bGameTurnStart; }
	};
}