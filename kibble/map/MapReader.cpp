#include "MapReader.h"
#include <assert.h>
#include "json.hpp"
#include "kibble\json\jsonextra.hpp"
#include "_Project\VictorumAnalyitics.h"

#include <iostream>
#include <fstream>

kibble::MapReader* kibble::MapReader::sm_instance = nullptr;

kibble::MapReader::MapReader()
{
}

kibble::MapReader::~MapReader()
{
}

void kibble::MapReader::addMap(const std::string & p_mapName)
{
	m_mapList.push_back(p_mapName);
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
		auto end = masterFile["files"].cend();
		for (auto it = masterFile["files"].cbegin(); it != end; ++it)
		{
			sm_instance->addMap(*it);
		}
	}
}

std::vector<std::pair<int, int>> kibble::MapReader::getMap(int* p_dimX, int* p_dimZ, int* p_id)
{
	std::string mapName;
	if (*p_id < 0 || *p_id >= m_mapList.size())//get random map, if id is out of bound
	{
		*p_id = rand() % m_mapList.size();
	}

	mapName = m_mapList[*p_id];

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
