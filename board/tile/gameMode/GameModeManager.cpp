#include "GameModeManager.h"
#include "AreaControl/ControlArea.h"
#include "networking/ClientGame.h"
#include "AttackDefend/DefendArea.h"
#include "Capture/ItemSpawnArea.h"
#include "Capture/ItemDropArea.h"

GameModeManager* GameModeManager::sm_instance = nullptr;

void GameModeManager::registerTile(kitten::K_GameObject * p_tileGO, GameModeComponent::TileType p_type)
{
	auto found = m_modeComponentMap.find(p_type);
	if (found != m_modeComponentMap.end())
	{
		//get tile info
		TileInfo* info = p_tileGO->getComponent<TileInfo>();
		found->second->addTile(info);
	}
}

void GameModeManager::listenEvent(kitten::Event::EventType p_type, kitten::Event * p_data)
{
	if (p_type == kitten::Event::New_Game_Turn)
	{
		//init all component when first turn start
		if (!m_isInit)
		{
			for (auto component : m_modeComponentMap)
			{
				component.second->init();
			}

			//clear removed component
			auto end = m_modeComponentMap.cend();
			auto comp = m_modeComponentMap.begin();
			while (comp != end)
			{
				if (comp->second == nullptr)
				{
					comp = m_modeComponentMap.erase(comp);
				}
				else
				{
					comp++;
				}
			}
			m_isInit = true;
		}
		else
		{	//then check all component when other turns start
			//they are actually checking result of last turn
			//so start at turn 2
			for (auto component : m_modeComponentMap)
			{
				component.second->check();
			}
		}

		checkPoints();
	}
}

void GameModeManager::gainPoint(int p_clientId, int p_points)
{
	if (p_clientId >= 0)
		m_points[p_clientId] += p_points;
}

void GameModeManager::removeModeComponent(GameModeComponent * p_comp)
{
	auto end = m_modeComponentMap.end();
	for (auto it = m_modeComponentMap.begin(); it != end; it++)
	{
		if (it->second == p_comp)
		{
			//delete component
			delete it->second;

			//remove from map later
			it->second = nullptr;

			break;
		}

	}
}

GameModeManager::GameModeManager()
	:m_points(std::vector<int>({0,0})),
	m_isInit(false)
{
	registerEvent();

	init();
}

GameModeManager::~GameModeManager()
{
	deregisterEvent();

	for (auto comp : m_modeComponentMap)
	{
		delete comp.second;
	}
	m_modeComponentMap.clear();
}

void GameModeManager::init()
{
	m_modeComponentMap[GameModeComponent::Control] = new ControlArea();
	m_modeComponentMap[GameModeComponent::Defend] = new DefendArea();

	ItemSpawnArea* area = new ItemSpawnArea();
	m_modeComponentMap[GameModeComponent::ItemSpawn] = area;
	m_modeComponentMap[GameModeComponent::ItemDrop0] = new ItemDropArea(0);
	m_modeComponentMap[GameModeComponent::ItemDrop1] = new ItemDropArea(1);

	//read json file
	nlohmann::json jsonfile = jsonIn(GAME_MODE_DATA);

	//get max point
	m_maxPoint = jsonfile["max_point"];

	//set property for components
	nlohmann::json::iterator end = jsonfile["mode_component"].end();
	for (nlohmann::json::iterator it = jsonfile["mode_component"].begin(); it != end; ++it) 
	{
		//get json
		nlohmann::json* compJson = &(*it);

		//check which component of this
		int enu = compJson->operator[]("enum");
		GameModeComponent::TileType type = static_cast<GameModeComponent::TileType>(enu);

		auto found = m_modeComponentMap.find(type);
		if (found != m_modeComponentMap.end())
		{
			found->second->setProperty(compJson);
		}
	}
}

void GameModeManager::registerEvent()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::New_Game_Turn,
		this,
		std::bind(&GameModeManager::listenEvent, this, std::placeholders::_1, std::placeholders::_2));
}

void GameModeManager::deregisterEvent()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::New_Game_Turn, this);
}

void GameModeManager::checkPoints()
{
	//Note: GameModeManager::checkPoints() check points from clientId 0 to 1
	//if both player reach max points, client 0 always win

	int clientId = -1;
	for (int i = 0; i < m_points.size(); i++)
	{
		if (m_points[i] >= m_maxPoint)//reach max points
		{
			clientId = i;
			break;
		}
	}

	if (clientId < 0)//no one reach max points
		return;

	if (networking::ClientGame::isNetworkValid())
	{
		networking::ClientGame* client = networking::ClientGame::getInstance();

		kitten::Event* eventData = new kitten::Event(kitten::Event::Network_End_Game);
		if (clientId == client->getClientId())
		{
			eventData->putInt(GAME_END_RESULT, HOST_COMMANDER_DIED);
		}
		else
		{
			eventData->putInt(GAME_END_RESULT, CLIENT_COMMANDER_DIED);
		}

		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Network_End_Game, eventData);
	}
}

