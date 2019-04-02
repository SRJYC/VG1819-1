#include "networking\dedicated_server\NetworkData.h"
#include <sstream>

std::string AbilityPacket::getFormattedAbilityInfo()
{
	std::stringstream message;
	message << "\tAbility Name: ";
	for (int i = 0; i < m_abilityNameLength; ++i)
	{
		message << m_abilityName[i];
	}

	if (m_unit.m_kibbleID != -1)
	{
		message << "\n\tSummoned Unit Kibble ID: " << m_unit.m_kibbleID << "\n";
	}

	message << "\tClient ID: " << m_clientId << "\n";

	message << "\tSource Unit Pos: (" << m_sourceUnit.posX << ", " << m_sourceUnit.posY << ")\n";

	message << "\tNumber of Target Units: " << m_numTargetUnits << "\n";
	message << "\tTarget Units Kibble IDs:\n\t\t";
	for (int i = 0; i < m_numTargetUnits; ++i)
	{
		UnitPos pos = m_targets[i];
		message << "(Pos: " << pos.posX << ", " << pos.posY << "), ";
	}
	message << "\n\tNumber of Int Values: " << m_numIntValues << "\n";
	message << "\tInt Values:\n";
	for (auto it = m_intValue.begin(); it != m_intValue.end(); ++it)
	{
		message << "\t\t(Key: ";
		for (int i = 0; i < it->first.size(); ++i)
		{
			message << it->first.at(i);
		}
		message << ", Value: " << it->second << ")\n";
	}

	message << "\tNumber of Target Tiles: " << m_numTargetTiles << "\n";
	message << "\tTiles:\n";
	for (int i = 0; i < m_numTargetTiles; ++i)
	{
		std::pair<int, int> tilePos = m_targetTiles[i];
		int posX = tilePos.first;
		int posY = tilePos.second;
		message << "\t\t(" << posX << ", " << posY << ")\n";
	}

	if (m_clickedObjectPos.first > -1)
	{
		message << "\tm_clickedObjectPos: " << m_clickedObjectPos.first << ", " << m_clickedObjectPos.second;
	}

	return message.str();
}

// If an assert is failed, make sure you are using a fresh Buffer for serializing
// and a fresh one for deserializing
void AbilityPacket::writeInt(Buffer& p_buffer, int p_value)
{
	assert(p_buffer.m_index + sizeof(int) <= p_buffer.m_size);
	
	*((int*)(p_buffer.m_data + p_buffer.m_index)) = p_value;
	p_buffer.m_index += sizeof(int);
}

void AbilityPacket::writeChar(Buffer &p_buffer, char p_value)
{
	assert(p_buffer.m_index + sizeof(char) <= p_buffer.m_size);

	*((char*)(p_buffer.m_data + p_buffer.m_index)) = p_value;
	p_buffer.m_index += sizeof(char);
}

int AbilityPacket::readInt(Buffer& p_buffer)
{
	// For ResizeablePacket, Can't check against buffer size when reading as the size is set
	// from a non constant value (getsize()), so we don't know that actual size that the buffer should be
	// unless we set it to the number of bytes read
	//assert(buffer.m_index + sizeof(int) <= buffer.m_size);

	int value;
	value = *((int*)(p_buffer.m_data + p_buffer.m_index));
	p_buffer.m_index += sizeof(int);

	return value;
}

char AbilityPacket::readChar(Buffer &p_buffer)
{
	//assert(buffer.m_index + sizeof(char) <= buffer.m_size);

	char value;
	value = *((char*)(p_buffer.m_data + p_buffer.m_index));
	p_buffer.m_index += sizeof(char);

	return value;
}

void AbilityPacket::serialize(Buffer& p_buffer)
{
	writeInt(p_buffer, this->m_packetType);
	writeInt(p_buffer, this->m_clientId);
	writeInt(p_buffer, this->m_totalBytes);
	writeInt(p_buffer, this->m_sourceUnit.posX);
	writeInt(p_buffer, this->m_sourceUnit.posY);

	writeInt(p_buffer, this->m_numTargetUnits);
	for (int i = 0; i < m_numTargetUnits; ++i)
	{
		auto pos = m_targets[i];
		writeInt(p_buffer, pos.posX);
		writeInt(p_buffer, pos.posY);
	}

	writeInt(p_buffer, this->m_numIntValues);
	for (auto it = m_intValue.begin(); it != m_intValue.end(); ++it)
	{
		// For each key, value pair in the map, we need to:
		// Get the key, the length of the key, and then write each char in the key to the buffer
		// Finally, write the value tied to the key to the buffer
		const std::string& strKey = it->first;
		int keyLength = strKey.size();
		writeInt(p_buffer, keyLength);
		for (int i = 0; i < keyLength; ++i)
		{
			writeChar(p_buffer, strKey[i]);
		}
		writeInt(p_buffer, it->second);
	}

	writeInt(p_buffer, this->m_numTargetTiles);
	for (int i = 0; i < m_numTargetTiles; ++i)
	{
		std::pair<int,int> tilePos = m_targetTiles[i];
		int posX = tilePos.first;
		int posY = tilePos.second;
		writeInt(p_buffer, posX);
		writeInt(p_buffer, posY);
	}

	// String Values
	writeInt(p_buffer, this->m_numStringValues);
	auto end = m_stringValue.end();
	for (auto it = m_stringValue.begin(); it != end; ++it)
	{
		// Get the key, write the length of the key, and then write each char in the key to the buffer
		const std::string& strKey = it->first;
		int keyLength = strKey.size();
		writeInt(p_buffer, keyLength);
		for (int i = 0; i < keyLength; ++i)
		{
			writeChar(p_buffer, strKey[i]);
		}

		// Get the value, write the length of the value, and then write each char of the value to the buffer
		const std::string& strValue = it->second;
		int valueLength = strValue.size();
		writeInt(p_buffer, valueLength);
		for (int i = 0; i < valueLength; ++i)
		{
			writeChar(p_buffer, strValue[i]);
		}
	}

	// AbilityName
	writeInt(p_buffer, m_abilityNameLength);
	for (int i = 0; i < m_abilityNameLength; ++i)
	{
		writeChar(p_buffer, m_abilityName[i]);
	}

	// Unit Data
	writeInt(p_buffer, m_unit.m_kibbleID);
	writeInt(p_buffer, m_unit.m_HP);
	writeInt(p_buffer, m_unit.m_maxHP);
	writeInt(p_buffer, m_unit.m_IN);
	writeInt(p_buffer, m_unit.m_baseIN);
	writeInt(p_buffer, m_unit.m_MV);
	writeInt(p_buffer, m_unit.m_baseMV);
	writeInt(p_buffer, m_unit.m_cost);
	writeInt(p_buffer, m_unit.m_baseCost);

	// AbilityInfoPacket::m_clickedObject
	writeInt(p_buffer, m_clickedObjectPos.first);
	writeInt(p_buffer, m_clickedObjectPos.second);
}

void AbilityPacket::deserialize(Buffer& p_buffer)
{
	this->m_packetType = readInt(p_buffer);
	this->m_clientId = readInt(p_buffer);
	this->m_totalBytes = readInt(p_buffer);

	this->m_sourceUnit.posX = readInt(p_buffer);
	this->m_sourceUnit.posY = readInt(p_buffer);

	m_numTargetUnits = readInt(p_buffer);	
	for (int i = 0; i < m_numTargetUnits; ++i)
	{
		UnitPos unit;
		unit.posX = readInt(p_buffer);
		unit.posY = readInt(p_buffer);
		m_targets.push_back(unit);
	}

	m_numIntValues = readInt(p_buffer);
	for (int i = 0; i < m_numIntValues; ++i)
	{
		int keyLength = readInt(p_buffer);
		std::string key = "";
		for (int j = 0; j < keyLength; ++j)
		{
			key += readChar(p_buffer);
		}
		int value = readInt(p_buffer);
		m_intValue.insert({ key, value });
	}

	m_numTargetTiles = readInt(p_buffer);
	for (int i = 0; i < m_numTargetTiles; ++i)
	{
		int posX = readInt(p_buffer);
		int posY = readInt(p_buffer);
		m_targetTiles.push_back(std::make_pair(posX, posY));
	}

	m_numStringValues = readInt(p_buffer);
	for (int i = 0; i < m_numStringValues; ++i)
	{
		// Read the length of the key from the buffer
		int keyLength = readInt(p_buffer);
		std::string key = "";
		for (int j = 0; j < keyLength; ++j)
		{
			// Construct the key by reading each char
			key += readChar(p_buffer);
		}

		// Read the length of the value from the buffer
		int valueLength = readInt(p_buffer);
		std::string value = "";
		for (int j = 0; j < valueLength; ++j)
		{
			// Construct the value by reading each char
			value += readChar(p_buffer);
		}

		m_stringValue.insert({ key, value });
	}

	m_abilityNameLength = readInt(p_buffer);	
	for (int i = 0; i < m_abilityNameLength; ++i)
	{
		m_abilityName += readChar(p_buffer);
	}

	// Unit Data
	m_unit.m_kibbleID = readInt(p_buffer);
	m_unit.m_HP = readInt(p_buffer);
	m_unit.m_maxHP = readInt(p_buffer);
	m_unit.m_IN = readInt(p_buffer);
	m_unit.m_baseIN = readInt(p_buffer);
	m_unit.m_MV = readInt(p_buffer);
	m_unit.m_baseMV = readInt(p_buffer);
	m_unit.m_cost = readInt(p_buffer);
	m_unit.m_baseCost = readInt(p_buffer);

	// AbilityInfoPacket::m_clickedObject
	m_clickedObjectPos.first = readInt(p_buffer);
	m_clickedObjectPos.second = readInt(p_buffer);
}

int AbilityPacket::getSize()
{
	// sizeof all member int variables
	int intVariablesSize =
		sizeof(m_packetType)
		+ sizeof(m_clientId)
		+ sizeof(m_sourceUnit)
		+ sizeof(m_numTargetUnits)
		+ sizeof(m_numIntValues)
		+ sizeof(m_numTargetTiles)
		+ sizeof(m_numStringValues)
		+ sizeof(m_abilityNameLength)
		+ sizeof(m_unit)
		+ sizeof(m_clickedObjectPos);

	// sizeof all int values in TargetUnits vector
	int targetUnitsSize = sizeof(int) * 2 * m_numTargetUnits;

	// sizeof all string values and int values in IntValues map
	int intValuesSize = (sizeof(int) * m_numIntValues) + ((sizeof(char) * m_sumKeysLength)) + (sizeof(int) * m_numIntValues);

	// sizeof all int values in TargetTiles vector, note it's a vector of int pairs
	int targetTilesSize = sizeof(int) * 2 * m_numTargetTiles;

	// sizeof all the keys and values in the StringValues map
	// sizeof m_numStringValues int written to buffer + the integers written to buffer for each key and value length + the size of all values written
	// note: sum of key lengths were added to m_sumKeysLength which have been calculated in intValuesSize above
	int stringValuesSize = sizeof(int) + (2 * sizeof(int) * m_numStringValues) + (sizeof(char) * m_sumStringValuesLength);

	// sizeof ability name
	int abilityNameSize = m_abilityNameLength * sizeof(char);

	m_totalBytes = intVariablesSize + targetUnitsSize + intValuesSize + targetTilesSize + stringValuesSize + abilityNameSize + sizeof(int);

	return m_totalBytes;
}