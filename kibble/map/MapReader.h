#pragma once

#include "json.hpp"
#include <string>
#include <vector>

#define MAP_DIRECTORY "data/map/map/"

namespace kibble
{
	class MapReader
	{
		struct Map
		{
			std::string name;
			std::string map;
			std::string imgPath;
			std::string description;
		};

	private:
		MapReader();
		~MapReader();
		static MapReader* sm_instance;

		std::vector<Map> m_mapList;
		int m_selectedMapId;

		void addMap(nlohmann::json* p_json);
	public:
		static void createInstance();
		static void destroyInstance();
		static MapReader* getInstance() { return sm_instance; };

		static void loadAllMap(const std::string& p_masterJsonName);

		std::vector<Map> getMapList() const;
		std::vector<std::pair<int,int> > getMapData(int* p_dimX, int* p_dimZ, int* p_id) const;

		void selectMap(int p_id) { m_selectedMapId = p_id; };
		int getSelectedMap() const { return m_selectedMapId; };
	};
}