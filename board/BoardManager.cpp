#include "BoardManager.h"

#include "kitten\event_system\EventManager.h"
#include "kitten\QuadEdgeRenderable.h"

#include "kitten/K_ComponentManager.h"
#include "kitten/K_GameObjectManager.h"
#include "networking\ClientGame.h"

BoardManager* BoardManager::sm_instance = nullptr;

void BoardManager::createBoard(int p_mapID, bool p_enableTileInfoDisplay)
{
	m_boardCreator->setTileInfoDisplay(p_enableTileInfoDisplay);

	int mapId = p_mapID;
	if (networking::ClientGame::getMapId() > -1)
	{
		mapId = networking::ClientGame::getInstance()->getMapId();
	}
	m_boardCreator->createBoard(mapId);

	//it's done, delete it
	delete m_boardCreator;
	m_boardCreator = nullptr;
}

//board property
kitten::K_GameObject* BoardManager::getSpawnPoint(int m_clientId)
{
	kitten::K_GameObject* tile;
	if (m_clientId > -1 && m_clientId < m_spawnPointList.size())
	{
		std::pair<int, int> point = m_spawnPointList[m_clientId];
		tile = getTile(point.first, point.second);
	}
	else
	{
		std::pair<int, int> point = m_spawnPointList.back();
		tile = getTile(point.first, point.second);
		m_spawnPointList.pop_back();
	}

	return tile;
}

std::pair<int, int> BoardManager::getDimension() const
{
	return m_dimension;
}

kitten::K_GameObject * BoardManager::getTile(int p_x, int p_z) const
{
	//if out of range, return nullptr
	if (p_x < 0 || p_z < 0 || p_x >= m_dimension.first || p_z >= m_dimension.second)
		return nullptr;
	std::pair<int, int> pos(p_x, p_z);
	int z_length = m_dimension.second;
	if (pos == m_tileList[p_x * z_length + p_z]->getComponent<TileInfo>()->getPos())
	{
		return m_tileList[p_x * z_length + p_z];
	}
	assert(false);//not found tile or wrong in position
	return nullptr;
}

//area
void BoardManager::showArea(kitten::K_GameObject* p_pivot)
{
	if (m_area->isActive())
	{
		hideArea();

		m_areaList = m_area->getTileListWithPivot(p_pivot);
		applyFilter(&m_areaList);

		m_highlighter->highlightTile(TileInfo::Area, m_areaList);
	}
}

void BoardManager::hideArea()
{
	if (m_area->isActive())
	{
		m_highlighter->unhighlightAll(TileInfo::Area);
	}
}

kitten::Event::TileList BoardManager::getArea()
{
	if (m_area->isActive())
	{
		m_area->removePattern();
		m_highlighter->unhighlightAll(TileInfo::Area);
		if (m_select)
		{
			m_select = false;
			m_selectRepeat = false;
			m_selectNum = 0;
			m_targetNum = 0;
			m_highlighter->unhighlightAll(TileInfo::Select);
			return m_selectList;
		}
		else
		{
			return m_areaList;
		}
	}

	return kitten::Event::TileList();
}

//select target
void BoardManager::select()
{
	if (m_select && m_area->isActive())
	{
		m_selectNum++;
		//add area list to select list
		for (int i = 0; i < m_areaList.size(); i++)
		{
			std::pair<int, int> pos = m_areaList[i];

			if (m_selectRepeat)//allow duplicate
			{
				m_selectList.push_back(pos);
			}
			else
			{
				//check duplicate
				bool repeat = false;
				for (int j = 0; j < m_selectList.size(); j++)
				{
					if (pos == m_selectList[j])
					{
						repeat = true;
						break;
					}
				}
				if (repeat)//ignore repeat
					continue;
				else
					m_selectList.push_back(pos);
			}
		}

		m_highlighter->highlightTile(TileInfo::Select, m_selectList);
	}
}

void BoardManager::deselect()
{
	if (m_select && m_area->isActive())
	{
		m_selectNum--;
		//remove area list from select list
		for (int i = 0; i < m_areaList.size(); i++)
		{
			std::pair<int, int> pos = m_areaList[i];

			auto it = m_selectList.begin();
			while (it != m_selectList.end())
			{
				if (*it == pos)
				{
					it = m_selectList.erase(it);
				}
				else
				{
					it++;
				}
			}
		}
		m_highlighter->unhighlightTile(TileInfo::Select, m_areaList);
	}
}

//range
kitten::Event::TileList BoardManager::getRange() const 
{
	return m_rangeList;
}

//event 
void BoardManager::registerEvent()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Highlight_Tile_With_List,
		this,
		std::bind(&BoardManager::listenEvent, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Highlight_Tile,
		this,
		std::bind(&BoardManager::listenEvent, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Unhighlight_Tile,
		this,
		std::bind(&BoardManager::listenEvent, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Set_Area_Pattern,
		this,
		std::bind(&BoardManager::listenEvent, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Right_Clicked,
		this,
		std::bind(&BoardManager::listenEvent, this, std::placeholders::_1, std::placeholders::_2));
}

void BoardManager::deregisterEvent()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::Highlight_Tile_With_List, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::Highlight_Tile, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::Unhighlight_Tile, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::Set_Area_Pattern, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::Right_Clicked, this);
}


//tile click
void BoardManager::tileClicked(bool p_send)
{
	if (!m_area->isActive())
	{
		return;//not wait for player to choose target, ingore it
	}

	if (p_send)
	{
		if (m_select)
		{
			select();

			if (m_selectNum < m_targetNum)//dont have enough selected target
				return;
		}

		if (m_area->isActive())
		{
			kitten::Event* e = new kitten::Event(kitten::Event::Tile_Clicked);
			e->putInt("highlighted", TRUE);
			kitten::Event::TileList list = getArea();
			e->putInt(TILE_NUMBER, list.size());
			for (int i = 0; i < list.size(); i++)
			{
				int x = list[i].first;
				int z = list[i].second;
				kitten::K_GameObject* tileGO = getTile(x, z);

				std::string key = TILE + std::to_string(i);

				e->putGameObj(key, tileGO);
			}
			m_highlighter->unhighlightAll(TileInfo::Range);
			kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Tile_Clicked, e);
		}
	}
	else
	{
		kitten::Event* e = new kitten::Event(kitten::Event::Tile_Clicked);
		e->putInt("highlighted", FALSE);
		getArea();
		m_highlighter->unhighlightAll(TileInfo::Range);
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Tile_Clicked, e);
	}

	m_areaList.clear();
	m_rangeList.clear();
	m_selectList.clear();
}

void BoardManager::autoClick(kitten::K_GameObject * p_tile)
{
	//check if it's in range
	TileInfo* info = p_tile->getComponent<TileInfo>();
	if (info->isHighlighted(TileInfo::Range))
	{
		//change area list
		showArea(p_tile);

		//send data
		tileClicked(true);
	}
	else
	{
		//cancel ability
		tileClicked(false);
	}
}

//reset components
void BoardManager::resetComponents()
{
	m_area->removePattern();
	m_range->clear();
	m_highlighter->reset();
	if (m_boardCreator == nullptr)
		m_boardCreator = new BoardCreator();

	this->createBoard();
}


//private

//constructor and destructor
BoardManager::BoardManager() :
	m_range(nullptr),
	m_hlGO(nullptr),
	m_highlighter(nullptr),
	m_pipeline(nullptr),
	m_area(nullptr),
	m_boardCreator(nullptr),
	m_boardObject(nullptr),
	m_dimension(std::make_pair(0,0)),
	m_isGridEnabled(true)
{
	//m_boardGO = kitten::K_GameObjectManager::getInstance()->createNewGameObject();

	//m_pathFind = new PathFind();
	registerEvent();

	m_range = new Range();
	if (m_dimension != std::make_pair(0, 0))
		m_range->setDimension(m_dimension.first, m_dimension.second);

	m_hlGO = kitten::K_GameObjectManager::getInstance()->createNewGameObject("highlighter.json");
	m_highlighter = m_hlGO->getComponent<Highlighter>();
	kitten::K_GameObjectManager::getInstance()->flagGameObjectToSurvive(m_hlGO);

	m_pipeline = new TilePipeline();
	m_area = new Area();

	m_select = false;
	m_selectRepeat = false;
	m_selectNum = 0;
	m_targetNum = 0;
}

BoardManager::~BoardManager()
{
	deregisterEvent();

	//delete m_pathFind;
	delete m_range;
	//delete m_highlighter;
	delete m_pipeline;
	delete m_area;
}

//listen event
void BoardManager::listenEvent(kitten::Event::EventType p_type, kitten::Event * p_data)
{
	switch (p_type)
	{
	case kitten::Event::Highlight_Tile_With_List:
		highlightTileWithList(p_data);
		break;
	case kitten::Event::Highlight_Tile:
		highlightTile(p_data);
		break;
	case kitten::Event::Unhighlight_Tile:
		m_highlighter->unhighlightAll(TileInfo::Range);
		m_highlighter->unhighlightAll(TileInfo::Area);
		break;
	case kitten::Event::Set_Area_Pattern:
		setArea(p_data);
		break;
	case kitten::Event::Right_Clicked:
		tileClicked(false);
		break;
	default:
		break;
	}
}

void BoardManager::setGrid(bool p_enabled)
{
	auto end = m_tileList.cend();
	for (auto it = m_tileList.cbegin(); it != end; ++it)
	{
		auto edgeRenderable = (*it)->getComponent<kitten::QuadEdgeRenderable>();
		
		if (edgeRenderable != nullptr)
		{
			edgeRenderable->setEnabled(p_enabled);
		}
	}

	m_isGridEnabled = p_enabled;
}

bool BoardManager::isGridEnabled() const
{
	return m_isGridEnabled;
}

//high light
void BoardManager::highlightTile(kitten::Event * p_data)
{
	m_range->clear();

	kitten::Event::TileList list;
	if (p_data->getString("mode") == "range")
	{
		list = m_range->getTilesInRange(p_data);
	}
	else if (p_data->getString("mode") == "all")
	{//highlight all tile
		for (int i = 0; i < m_tileList.size(); i++)
		{
			list.push_back(m_tileList[i]->getComponent<TileInfo>()->getPos());
		}
	}
	/*
	else if (p_data->getString("mode") == ABILITY_MANIPULATE_TILE)
	{
		const kitten::Event::TileList* l = p_data->getTileList();
		m_highlighter->highlightTile(TileInfo::Owned, *l);
		return;
	}*/

	//apply filter
	setFilter(FILTER, p_data);
	applyFilter(&list);

	m_rangeList = list;

	m_highlighter->highlightTile(TileInfo::Range, list);
}
void BoardManager::highlightTileWithList(kitten::Event * p_data)
{
	const kitten::Event::TileList* list = p_data->getTileList();
	m_highlighter->highlightTile(TileInfo::Range, *list);
}

//unit ability related, set range and area
void BoardManager::setFilter(const std::string & p_filter, kitten::Event * p_data)
{
	int filterNum = p_data->getInt(p_filter);
	m_pipeline->resetFilter();

	kitten::K_GameObject* origin = p_data->getGameObj("tileAtOrigin");
	kitten::K_GameObject* ugo = origin->getComponent<TileInfo>()->getUnit();
	m_pipeline->setSource(ugo->getComponent<unit::Unit>());
	for (int i = 0; i < filterNum; i++)
	{
		std::string key = p_filter + std::to_string(i);
		m_pipeline->useFilter(p_data->getString(key));
	}
}
void BoardManager::applyFilter(kitten::Event::TileList * p_list)
{
	m_pipeline->filterList(p_list);
}
void BoardManager::setArea(kitten::Event * p_data)
{
	//select
	m_selectList.clear();
	m_selectList.shrink_to_fit();//reset list

	if (p_data->getInt("select") == TRUE)
	{
		m_select = true;
		m_targetNum = p_data->getInt("target");
		if (p_data->getInt("repeat") == TRUE)
		{
			m_selectRepeat = true;
		}
		else
		{
			m_selectRepeat = false;
		}
	}

	//area
	m_area->setPattern(p_data);
	setFilter(AREA_FILTER, p_data);

	//pivot tile
	kitten::K_GameObject* p = p_data->getGameObj(ORIGIN);

	//show inital highlight
	TileInfo* info = p->getComponent<TileInfo>();
	if (info->isHighlighted(TileInfo::Range))
	{
		if (p_data->getInt(AUTO_CLICK) == TRUE)
		{
			autoClick(p);//auto casting ability
		}
		else
		{
			showArea(p);//show inital highlight
		}
	}
}


//set board
void BoardManager::setSpawnPoint(const kitten::Event::TileList& p_list)
{
	m_spawnPointList = p_list;
}
void BoardManager::setTileList(const std::vector<kitten::K_GameObject*>& p_list)
{
	m_tileList = p_list;
}
void BoardManager::setDimension(int p_x, int p_z)
{
	m_dimension = std::pair<int, int>(p_x, p_z);
	if(m_range != nullptr)
		m_range->setDimension(p_x, p_z);
}
void BoardManager::setMapID(int p_id)
{
	m_mapId = p_id;
}

void BoardManager::setBoardGameObject(kitten::K_GameObject * p_go)
{
	m_boardObject = p_go;
}






/*
void BoardManager::unhighlightTile(kitten::Event * p_data)
{
	m_highlighter->unhighlightAll(TileInfo::ForRange);
}
*/


