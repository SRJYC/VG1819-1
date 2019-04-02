#pragma once
#include "networking\dedicated_server\NetworkingCommon.h"
#include <string.h>

// For ResizablePacket
#include <vector>
#include <unordered_map>

#define MAX_PACKET_SIZE 1000000
#define MAX_CHAR_BUFSIZE 512
#define MAX_TEXTCHAT_LINE_SIZE 41	  // ~ the max number of chars that fit in the textchat textbox
#define MAX_TYPABLE_LINES 3
#define MAX_TEXTCHAT_MSG_SIZE (MAX_TYPABLE_LINES * MAX_TEXTCHAT_LINE_SIZE)

#define BASIC_PACKET_SIZE sizeof(Packet)
#define TEXTCHAT_MESSAGE_PACKET_SIZE sizeof(TextChatMessagePacket)
#define SKIP_TURN_PACKET_SIZE sizeof(SkipTurnPacket)
#define UNIT_PACKET_SIZE sizeof(UnitPacket)
#define STARTING_COMMANDERS_PACKET_SIZE sizeof(StartingCommandersPacket)
#define SERVER_INFO_PACKET_SIZE sizeof(ServerInfoPacket)
#define TEST_PACKET_SIZE sizeof(TestPacket)
#define MAP_DATA_PACKET_SIZE sizeof(MapDataPacket)

enum PacketTypes 
{
	INIT_CONNECTION = 0,
	SERVER_SHUTDOWN,
	CLIENT_DISCONNECT,
	SEND_CLIENT_ID,
	ABILITY_PACKET,
	CAST_TIME_ABILITY_PACKET,
	SUMMON_UNIT,
	SKIP_TURN,
	GAME_TURN_START,
	STARTING_COMMANDER_DATA,
	DESYNCED,
	JOIN_GAME,
	SERVER_FULL,
	SESSIONS_FULL,
	PING_SOCKET,
	TEXTCHAT_MESSAGE,
	READY_CHECK,
	QUICKPLAY,
	UPDATE_SERVER_INFO,
	QUICKPLAY_FOUND_GAME,
	SESSION_ENDED,
	MAP_DATA
};

struct UnitPrimitiveData
{
	int m_kibbleID = -1;
	int m_HP = -1, m_maxHP = -1;
	int m_IN = -1, m_baseIN = -1;
	int m_MV = -1, m_baseMV = -1;
	int m_cost = -1, m_baseCost = -1;
};

struct UnitNetworkInfo
{
	int clientId;	// Who owns the unit
	int unitId;		// Kibble unit ID
	int posX, posY; // Tile coordinates
};

struct UnitPos
{
	int posX;
	int posY;
};

class Buffer
{
	friend class AbilityPacket;
	friend struct Packet;
public:
	char* m_data;		// pointer to buffer data
	int m_size;			// size of buffer data in bytes
	int getIndex() { return m_index; }
private:
	int m_index = 0;	// index of next byte to be read/written
};

// Use to get packet type first, then deserialize into appropriate packet
struct Packet {

	unsigned int m_packetType;
	int m_clientId;

	void writeInt(Buffer& p_buffer, int p_value)
	{
		assert(p_buffer.m_index + sizeof(int) <= p_buffer.m_size);

		*((int*)(p_buffer.m_data + p_buffer.m_index)) = p_value;
		p_buffer.m_index += sizeof(int);
	}

	void writeChar(Buffer &buffer, char value)
	{
		assert(buffer.m_index + sizeof(char) <= buffer.m_size);

		*((char*)(buffer.m_data + buffer.m_index)) = value;
		buffer.m_index += sizeof(char);
	}

	int readInt(Buffer& p_buffer)
	{
		//assert(buffer.m_index + sizeof(int) <= buffer.m_size);

		int value = *((int*)(p_buffer.m_data + p_buffer.m_index));
		p_buffer.m_index += sizeof(int);

		return value;
	}

	char readChar(Buffer &p_buffer)
	{
		//assert(buffer.m_index + sizeof(char) <= buffer.m_size);

		char value = *((char*)(p_buffer.m_data + p_buffer.m_index));
		p_buffer.m_index += sizeof(char);

		return value;
	}

	void serialize(Buffer& p_buffer) {
		writeInt(p_buffer, m_packetType);
		writeInt(p_buffer, m_clientId);
	}

	void deserialize(Buffer& p_buffer) {
		m_packetType = readInt(p_buffer);
		m_clientId = readInt(p_buffer);
	}
};

struct MapDataPacket : Packet
{
	int m_mapId = -1;

	void serialize(Buffer& p_buffer)
	{
		Packet::serialize(p_buffer);
		writeInt(p_buffer, m_mapId);
	}

	void deserialize(Buffer& p_buffer)
	{
		Packet::deserialize(p_buffer);
		m_mapId = readInt(p_buffer);
	}
};

struct TextChatMessagePacket : Packet
{
private:
	int m_messageLength = -1;
	char m_message[MAX_TEXTCHAT_MSG_SIZE];

public:
	std::string getMessage()
	{
		std::string message = std::string(m_message, m_messageLength);

		return message;
	}

	void addMessage(const std::string& p_message)
	{
		m_messageLength = MIN(p_message.length(), MAX_TEXTCHAT_MSG_SIZE);
		for (int i = 0; i < m_messageLength; ++i)
		{
			m_message[i] = p_message[i];
		}
	}

	void serialize(Buffer& p_buffer)
	{
		Packet::serialize(p_buffer);
		writeInt(p_buffer, m_messageLength);

		for (int i = 0; i < m_messageLength; ++i)
		{
			writeChar(p_buffer, m_message[i]);
		}
	}

	void deserialize(Buffer& p_buffer)
	{
		Packet::deserialize(p_buffer);
		m_messageLength = readInt(p_buffer);

		for (int i = 0; i < m_messageLength; ++i)
		{
			m_message[i] = readChar(p_buffer);
		}
	}
};

struct UnitPacket : Packet
{
	int m_unitId;
	int m_posX, m_posY;

	void serialize(Buffer& p_buffer) 
	{
		Packet::serialize(p_buffer);
		writeInt(p_buffer, m_unitId);
		writeInt(p_buffer, m_posX);
		writeInt(p_buffer, m_posY);
	}

	void deserialize(Buffer& p_buffer)
	{
		Packet::deserialize(p_buffer);
		m_unitId = readInt(p_buffer);
		m_posX = readInt(p_buffer);
		m_posY = readInt(p_buffer);
	}
};

struct SkipTurnPacket : Packet
{
	UnitPos m_unit;

	void serialize(Buffer& p_buffer)
	{
		Packet::serialize(p_buffer);
		writeInt(p_buffer, m_unit.posX);
		writeInt(p_buffer, m_unit.posY);
	}

	void deserialize(Buffer& p_buffer)
	{
		Packet::deserialize(p_buffer);
		m_unit.posX = readInt(p_buffer);
		m_unit.posY = readInt(p_buffer);
	}
};

struct StartingCommandersPacket : Packet
{
	UnitNetworkInfo commander0;
	UnitNetworkInfo commander1;

	void serialize(Buffer& p_buffer)
	{
		Packet::serialize(p_buffer);
		writeInt(p_buffer, commander0.clientId);
		writeInt(p_buffer, commander0.unitId);
		writeInt(p_buffer, commander0.posX);
		writeInt(p_buffer, commander0.posY);

		writeInt(p_buffer, commander1.clientId);
		writeInt(p_buffer, commander1.unitId);
		writeInt(p_buffer, commander1.posX);
		writeInt(p_buffer, commander1.posY);
	}

	void deserialize(Buffer& p_buffer)
	{
		Packet::deserialize(p_buffer);
		commander0.clientId = readInt(p_buffer);
		commander0.unitId = readInt(p_buffer);
		commander0.posX = readInt(p_buffer);
		commander0.posY = readInt(p_buffer);

		commander1.clientId = readInt(p_buffer);
		commander1.unitId = readInt(p_buffer);
		commander1.posX = readInt(p_buffer);
		commander1.posY = readInt(p_buffer);
	}
};

struct ServerInfoPacket : Packet
{
	int m_playerCount = -1;
	int m_activeSessions = -1;

	void serialize(Buffer& p_buffer)
	{
		Packet::serialize(p_buffer);
		writeInt(p_buffer, m_playerCount);
		writeInt(p_buffer, m_activeSessions);
	}

	void deserialize(Buffer& p_buffer)
	{
		Packet::deserialize(p_buffer);
		m_playerCount = readInt(p_buffer);
		m_activeSessions = readInt(p_buffer);
	}
};


class AbilityPacket
{
	//typedef std::vector<unit::Unit*> TargetUnits; // Not used by server
	typedef std::unordered_map<std::string, int> IntValues;
	//typedef std::vector<kitten::K_GameObject*>  TargetTiles; // Not used by server
	typedef std::unordered_map<std::string, std::string> StringValues;
public:
	int m_packetType = ABILITY_PACKET;
	int m_clientId;
	UnitPos m_sourceUnit;	// Reference source unit by its position on the board

	int m_abilityNameLength;
	std::string m_abilityName = "";

	//void print() // Not used by server

	std::string getFormattedAbilityInfo();

	void serialize(Buffer& p_buffer);
	void deserialize(Buffer& p_buffer);

	/* Not used by server
	void extractFromPackage(ability::AbilityInfoPackage* p_package);
	void insertIntoPackage(ability::AbilityInfoPackage* p_package);

	void addTargetUnits(const TargetUnits& p_targets);
	void addIntValues(const IntValues& p_values);
	void addTargetTiles(const TargetTiles& p_targetTilesGO);
	void addStringValues(const StringValues& p_stringValues);
	void addUnitData(unit::Unit* p_unit);

	const TargetUnits& getTargetUnits() const;
	const IntValues& getIntValues() const;
	const TargetTiles& getTargetTiles() const;
	const StringValues& getStringValues() const;
	unit::Unit* getUnit();
	*/

	int getSize();
	int getBytes() { return m_totalBytes; }

private:	
	UnitPrimitiveData m_unit;

	int m_sumKeysLength = 0;
	int m_totalBytes = 0;

	int m_numTargetUnits;
	std::vector<UnitPos> m_targets;		// Reference targets by their position on the board
	//TargetUnits m_targetObj;

	int m_numIntValues;
	IntValues m_intValue;

	int m_numTargetTiles;
	std::vector<std::pair<int, int>> m_targetTiles;
	//TargetTiles m_targetTilesGO; // Not used by server

	// Number of entries in m_stringValue
	int m_numStringValues = 0;
	// Sum of the length of all values in m_stringValue
	int m_sumStringValuesLength = 0;
	StringValues m_stringValue;

	std::pair<int, int> m_clickedObjectPos = { -1, -1 };

	void writeInt(Buffer &p_buffer, int p_value);
	void writeChar(Buffer &p_buffer, char p_value);
	int readInt(Buffer &p_buffer);
	char readChar(Buffer &p_buffer);
	
};

struct TestPacket : Packet {

	int num;
	char msg[MAX_CHAR_BUFSIZE];

	void serialize(char* data) {
		int *q = (int*)data;
		*q = this->m_packetType;   q++;
		*q = this->m_clientId;	q++;
		*q = this->num;			q++;

		char *p = (char*)q;
		int i = 0;
		for (int i = 0; i < MAX_CHAR_BUFSIZE; i++) {
			*p = this->msg[i];
			p++;
		}
	}

	void deserialize(char* data) {
		int *q = (int*)data;
		this->m_packetType = *q;		q++;
		this->m_clientId = *q;		q++;
		this->num = *q;				q++;

		char *p = (char*)q;
		int i = 0;
		for (int i = 0; i < MAX_CHAR_BUFSIZE; i++) {
			this->msg[i] = *p;
			p++;
		}
	}
};
