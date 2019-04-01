#include "MapReader.h"
#include <assert.h>
#include "json.hpp"
#include "kibble\json\jsonextra.hpp"

#include <iostream>
#include <fstream>

kibble::MapReader* kibble::MapReader::sm_instance = nullptr;

kibble::MapReader::MapReader():m_selectedMapId(-1)
{
}

kibble::MapReader::~MapReader()
{
}

void kibble::MapReader::addMap(nlohmann::json * p_json)
{
	std::string name = p_json->operator[]("name");
	std::string mapfile = p_json->operator[]("map");
	std::string imagePath = p_json->operator[]("image");
	std::string description = p_json->operator[]("description");

	Map m = { name,mapfile,imagePath,description };
	m_mapList.push_back(m);
}

void kibble::MapReader::createInstance()
{
	assert(sm_instance == nullptr);
	sm_instance = new MapReader();
}

void kibble::MapReader::destroyInstance()
{
	assert(sm_instance != nullptr);
	delete sm_instance;
	sm_instance = nullptr;
}

void kibble::MapReader::loadAllMap(const std::string & p_masterJsonName)
{
	nlohmann::json masterFile = jsonIn(MAP_DIRECTORY + p_masterJsonName);
	auto found = masterFile.find("files");
	if (found != masterFile.end())
	{
		auto end = masterFile["files"].end();
		for (nlohmann::json::iterator it = masterFile["files"].begin(); it != end; ++it)
		{
			sm_instance->addMap(&*it);
		}
	}
}

std::vector<kibble::MapReader::Map> kibble::MapReader::getMapList() const
{
	return m_mapList;
}

std::vector<std::pair<int, int>> kibble::MapReader::getMapData(int* p_dimX, int* p_dimZ, int* p_id) const
{
	std::string mapName;
	if (*p_id < 0 || *p_id >= m_mapList.size())//get random map, if id is out of bound
	{
		*p_id = rand() % m_mapList.size();
	}

	mapName = m_mapList[*p_id].map;

	//then read map
	std::ifstream file(MAP_DIRECTORY + mapName);
	assert(file.is_open());

	file >> *p_dimX;
	file >> *p_dimZ;

	std::vector<std::pair<int,int>> list;
	for (int i = 0; i < (*p_dimX) * (*p_dimZ); ++i)
	{
		int l;
		file >> l;
		list.push_back(std::make_pair(l, 0));
	}

	for (int i = 0; i < (*p_dimX) * (*p_dimZ); ++i)
	{
		int l;
		file >> l;
		list[i].second = l;
	}

	file.close();

	return list;
}
