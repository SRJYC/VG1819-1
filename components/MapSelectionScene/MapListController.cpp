#include "MapListController.h"
#include "kitten/InputManager.h"
#include "kitten/event_system/EventManager.h"
#include "ChangeMapDetailOnClick.h"
#include "kitten/K_GameObjectManager.h"
#include "ChangeListOnClick.h"

MapListController::MapListController(const std::string & p_randomImgPath, const std::string & p_randomDes, const std::string & p_upData, const std::string & p_downData, const std::string & p_mapData, float p_startX, float p_startY, float p_offsetY, int p_limitNum)
	:m_randomMapImagePath(p_randomImgPath),
	m_randomMapImageDescription(p_randomDes),
	m_upButtonData(p_upData),
	m_downButtonData(p_downData),
	m_mapButtonData(p_mapData),
	m_startX(p_startX),
	m_startY(p_startY),
	m_offsetY(p_offsetY),
	m_limit(p_limitNum)
{
}

MapListController::~MapListController()
{
}

void MapListController::update()
{
}

void MapListController::start()
{
	//get map list
	m_mapList = kibble::MapReader::getInstance()->getMapList();

	//create up button
	//create game object
	m_upButton = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_upButtonData);
	//get component
	ChangeListOnClick* cloc = m_upButton->getComponent<ChangeListOnClick>();
	//set controller
	cloc->setController(this);
	//move position
	m_upButton->getTransform().place2D(m_startX, m_startY);

	//create map buttons
	m_firstId = -1;
	m_lastId = -1;
	for (int i = 1; i <= m_limit; i++)
	{
		//if out of range, don't have enough map to fill list, so no more buttons
		int index = i - 2;
		int size = m_mapList.size();
		if (index >= size)
			break;

		//create game object
		kitten::K_GameObject* go = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_mapButtonData);
		
		//add to list
		m_mapButtonList.push_back(go);

		//get component
		ChangeMapDetailOnClick* oc = go->getComponent<ChangeMapDetailOnClick>();

		//set controller
		oc->setController(this);

		//move position
		go->getTransform().place2D(m_startX, m_startY - i * m_offsetY);

		//set info
		//get map
		kibble::MapReader::Map m;

		//first one is random option, not in map list
		if (index == -1)
		{
			m.name = "random";
		}
		else
		{
			m = m_mapList[index];
		}

		//change info
		oc->setInfo(m.name, index);

		m_lastId++;
	}

	//create down button
	//create game object
	m_downButton = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_downButtonData);
	//get component
	cloc = m_downButton->getComponent<ChangeListOnClick>();
	//set controller
	cloc->setController(this);
	//move position
	m_downButton->getTransform().place2D(m_startX, m_startY - (m_limit+1)*m_offsetY);
}

void MapListController::updateMapDetail(int p_mapId)
{
	kitten::Event* e = new kitten::Event(kitten::Event::Update_Map_Detail);
	if (p_mapId >= 0 && p_mapId < m_mapList.size())
	{//choose map
		e->putString("Image", m_mapList[p_mapId].imgPath);
		e->putString("Text", m_mapList[p_mapId].description);

		//select map
		kibble::MapReader::getInstance()->selectMap(p_mapId);
	}
	else
	{//choose random
		e->putString("Image", m_randomMapImagePath);
		e->putString("Text", m_randomMapImageDescription);

		kibble::MapReader::getInstance()->selectMap(-1);
	}
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Update_Map_Detail, e);
}

void MapListController::changeButton(int p_move)
{
	//postive number means move upward
	if (p_move > 0)
	{
		if (m_firstId == -1)//already show first map (random)
			return;
		else if (m_firstId - p_move < -1)//out of range, move as much as possible
		{
			resetButton(-1);
		}
		else//can move up 
		{
			resetButton(m_firstId - p_move);
		}
	}
	else if (p_move < 0)//negative number means move downward
	{
		if (m_lastId == m_mapList.size())//already show last map
			return;
		else if (m_lastId - p_move > m_mapList.size())//out of range, move as much as possible
		{
			int first = m_mapList.size() - m_limit;
			resetButton(first);
		}
		else//can move down
		{
			int first = m_lastId - p_move - m_limit;
			resetButton(first);
		}
	}
}

void MapListController::resetButton(int p_firstId)
{
	//don't need change
	if (m_firstId == p_firstId)
		return;

	//move pointer
	m_firstId = p_firstId;
	int index = m_firstId;

	//the rest is map list
	for (auto go : m_mapButtonList)
	{
		if (index >= m_mapList.size())//out of range
			break;

		//get map
		kibble::MapReader::Map m;

		//first one is random option, not in map list
		if (index == -1)
		{
			m.name = "random";
		}
		else
		{
			m = m_mapList[index];
		}

		//get component
		ChangeMapDetailOnClick* oc = go->getComponent<ChangeMapDetailOnClick>();

		//change info
		oc->setInfo(m.name,index);

		index++;
	}

	m_lastId = index;
}
