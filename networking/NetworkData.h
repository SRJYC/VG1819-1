#pragma once
#include <string.h>

// For ResizablePacket
#include <vector>
#include <unordered_map>
#include "kitten\K_GameObject.h"
#include "unit\Unit.h"

#define MAX_PACKET_SIZE 1000000
#define MAX_CHAR_BUFSIZE 512

#define BASIC_PACKET_SIZE sizeof(Packet)
#define SKIP_TURN_PACKET_SIZE sizeof(SkipTurnPacket)
#define UNIT_PACKET_SIZE sizeof(UnitPacket)
#define STARTING_COMMANDERS_PACKET_SIZE sizeof(StartingCommandersPacket)
#define TEST_PACKET_SIZE sizeof(TestPacket)

enum PacketTypes {

	INIT_CONNECTION = 0,
	SERVER_SHUTDOWN,
	CLIENT_DISCONNECT,
	SEND_CLIENT_ID,
	ABILITY_PACKET,
	SUMMON_UNIT,
	SKIP_TURN,
	GAME_TURN_START,
	STARTING_COMMANDER_DATA,
	DESYNCED
};

struct UnitInfo
{
	int clientId;	// Who owns the unit
	int unitId;		// Kibble unit ID
	int posX, posY; // Tile coordinates
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
	int m_unitId;

	void serialize(Buffer& p_buffer)
	{
		writeInt(p_buffer, m_packetType);
		writeInt(p_buffer, m_clientId);
		writeInt(p_buffer, m_unitId);
	}

	void deserialize(Buffer& p_buffer)
	{
		m_packetType = readInt(p_buffer);
		m_clientId = readInt(p_buffer);
		m_unitId = readInt(p_buffer);
	}
};

struct StartingCommandersPacket : Packet
{
	UnitInfo commander0;
	UnitInfo commander1;

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

class AbilityPacket
{
	typedef std::vector<unit::Unit*> TargetUnits;
	typedef std::unordered_map<std::string, int> IntValues;
	typedef std::vector<kitten::K_GameObject*>  TargetTiles;
public:
	int m_packetType;
	int m_clientId;
	int m_sourceUnit;

	int m_abilityNameLength;
	std::string m_abilityName = "";

	void print();

	void serialize(Buffer& p_buffer);
	void deserialize(Buffer& p_buffer);

	void addTargetUnits(TargetUnits p_targets);
	void addIntValues(IntValues p_values);
	void addTargetTiles(TargetTiles p_targetTilesGO);

	const TargetUnits& getTargetUnits();
	const IntValues& getIntValues();
	const TargetTiles& getTargetTiles();

	int getSize();
	int getBytes() { return m_totalBytes; }

private:	
	int m_sumKeysLength = 0;
	int m_totalBytes = 0;

	int m_numTargetUnits;
	std::vector<int> m_targets;
	TargetUnits m_targetObj;

	int m_numIntValues;
	IntValues m_intValue;

	int m_numTargetTiles;
	std::vector<std::pair<int, int>> m_targetTiles;
	TargetTiles m_targetTilesGO;

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
