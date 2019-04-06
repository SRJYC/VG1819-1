#pragma once
#include "InitiativeTracker.h"
#include "unit/Unit.h"
#include "kitten/K_GameObjectManager.h"
#include "unit/InitiativeTracker/InitiativeTrackerUI.h"
#include "kibble/kibble.hpp"
#include <algorithm>
#include "kitten/event_system/EventManager.h"
#include "networking\ClientGame.h"
#include "unit/unitComponent/unitAction/ActionButtonStore.h"
#include "unitInteraction/UnitInteractionManager.h"
//Rock

#define LISTSIZE(listA, listB) listA.size() + listB.size()

unit::InitiativeTracker* unit::InitiativeTracker::sm_instance = nullptr;

void unit::InitiativeTracker::sortListByIn(int p_list)
{
	//sort the unit object list 
	//highest intiative unit go first
	switch (p_list)
	{
	case 1://sort wait list that's after current unit
		if (m_currentUnitIndex >= m_unitObjectList.size())
		{//current unit is in wait list
			auto end = m_waitUnitObjectList.end();

			//find current unit
			for (auto it = m_waitUnitObjectList.begin(); it != end; it++)
			{
				if (getCurrentUnit() == *it)
				{
					//sort unit after current unit in wait list
					std::sort(it+1, m_waitUnitObjectList.end(), [](kitten::K_GameObject * p_u1, kitten::K_GameObject * p_u2)
					{
						int in1 = p_u1->getComponent<unit::Unit>()->m_attributes["in"];
						int in2 = p_u2->getComponent<unit::Unit>()->m_attributes["in"];
						return in1 > in2;
					});
					break;
				}
			}
		}
		else
		{//current unit is 
			std::sort(m_waitUnitObjectList.begin(), m_waitUnitObjectList.end(), [](kitten::K_GameObject * p_u1, kitten::K_GameObject * p_u2)
			{
				int in1 = p_u1->getComponent<unit::Unit>()->m_attributes["in"];
				int in2 = p_u2->getComponent<unit::Unit>()->m_attributes["in"];
				return in1 > in2;
			});
		}
		break;
	default://sort unit list
		std::sort(m_unitObjectList.begin(), m_unitObjectList.end(), [](kitten::K_GameObject * p_u1, kitten::K_GameObject * p_u2)
		{
			int in1 = p_u1->getComponent<unit::Unit>()->m_attributes["in"];
			int in2 = p_u2->getComponent<unit::Unit>()->m_attributes["in"];
			return in1 > in2;
		});
	}
	
}

void unit::InitiativeTracker::addWaitList()
{
	if (!m_waitUnitObjectList.empty())
	{
		m_unitObjectList.insert(m_unitObjectList.end(), m_waitUnitObjectList.begin(), m_waitUnitObjectList.end());
		m_waitUnitObjectList.clear();
	}
}

int unit::InitiativeTracker::getUnitObjectIndex(kitten::K_GameObject * p_uGO)
{
	for (auto it = m_unitObjectList.begin();it!=m_unitObjectList.end();it++)
	{//check if the unit is in normal list
		if (*it == p_uGO)
		{
			int index = it - m_unitObjectList.begin();
			return index;
		}
	}
	for (auto it = m_waitUnitObjectList.begin(); it != m_waitUnitObjectList.end(); it++)
	{//check if the unit is in wait list
		if (*it == p_uGO)
		{
			int i = it - m_waitUnitObjectList.begin();
			int index = m_unitObjectList.size() + i;
			return index;
		}
	}
	assert(false);
	return 0;
}

unit::InitiativeTracker::InitiativeTracker()
{
	m_display = new TurnChangeDisplay();

	m_uturn = new unit::UnitTurn();
	m_uturn->setNewTurnDisplay(m_display);

	m_UI = new unit::InitiativeTrackerUI();

	m_uAura =kibble::getGameObjectDataParserInstance()->getGameObject("unit_aura.json");
	m_uAura->getTransform().setIgnoreParent(false);
	m_uAura->setEnabled(false);

	m_currentUnitIndex = -1;//flag, means object list isn't initialize

	//add button listener
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::New_Unit_Turn,
		this,
		std::bind(&InitiativeTracker::newTurnListener, this, std::placeholders::_1, std::placeholders::_2));
}

unit::InitiativeTracker::~InitiativeTracker()
{
	kitten::EventManager::getInstance()->removeListener(
		kitten::Event::EventType::New_Unit_Turn,
		this
	);

	delete m_UI;
	delete m_uturn;
	delete m_display;
}

void unit::InitiativeTracker::addUnit(kitten::K_GameObject * p_unit)
{
	if (m_currentUnitIndex < 0)//not initialize
	{
		m_unitObjectList.push_back(p_unit);
	}
	else
	{
		m_waitUnitObjectList.push_back(p_unit);
		sortListByIn(1);
		m_UI->change(getUnitObjectIndex(p_unit));
	}

	/*
	m_unitObjectList.push_back(p_unit);
	if (m_currentUnitIndex >= 0)
	{
		int in1 = m_unitObjectList[m_currentUnitIndex]->getComponent<unit::Unit>()->m_attributes["in"];
		int in2 = p_unit->getComponent<unit::Unit>()->m_attributes["in"];
		if (in2 > in1)//new unit at front of current unit
		{
			m_currentUnitIndex++;
		}
		else//new unit at back of current unit
		{
			m_currentUnitIndex--;
		}
		//sort list
		sortListByIn();
	}*/
}

bool unit::InitiativeTracker::removeUnit(kitten::K_GameObject * p_unit)
{
	bool found = false;
	int index;
	for (auto it = m_unitObjectList.begin(); it != m_unitObjectList.end(); it++)
	{
		if (*it == p_unit)
		{
			found = true;
			//get index
			index = it - m_unitObjectList.begin();

			//remove it from list
			m_unitObjectList.erase(it);
			break;
		}
	}

	if (!found)
	{
		for (auto it = m_waitUnitObjectList.begin(); it != m_waitUnitObjectList.end(); it++)
		{//check if the unit is in wait list
			if (*it == p_unit)
			{
				found = true;
				//get index
				int i = it - m_waitUnitObjectList.begin();
				index = m_unitObjectList.size() + i;

				//remove it from list
				m_waitUnitObjectList.erase(it);
				break;
			}
		}
	}

	if (found)
	{
		//check if it is in extra turn list
		for (auto it = m_extraTurnUnitList.begin(); it != m_extraTurnUnitList.end(); it++)
		{
			if (*it == p_unit)//remove it from extra turn list
			{
				m_extraTurnUnitList.erase(it);
			}
		}

		if (m_uturn->isCurrent(p_unit))//pass turn to next
		{
			m_uturn->unitDestroyed();
			if (m_currentUnitIndex < getUnitNumber()) // there is next unit
			{
				m_uAura->getTransform().setParent(&getCurrentUnit()->getTransform());
				m_uturn->turnStart(getCurrentUnit());
			}
			else if (getUnitNumber() > 0)//there's unit but not this
				gameTurnStart();//start a new turn
			else//no unit left
				m_uturn->setEnd();
		}
		else if (index < m_currentUnitIndex)//change current index
			m_currentUnitIndex--;

		m_UI->change(index);

		kitten::K_GameObjectManager::getInstance()->destroyGameObject(p_unit);
		return true;
	}

	assert(false);//not found
	return false;
}

kitten::K_GameObject * unit::InitiativeTracker::getUnitByIndex(int p_index)
{
	//no unit
	if (LISTSIZE(m_unitObjectList, m_waitUnitObjectList) == 0)
		return nullptr;

	if (m_extraTurnUnitList.size() > 0)//there's extra turn unit
	{
		if (p_index > m_indexOfBreakPoint)//units before extra turn unit are passed
		{
			int index = p_index - m_indexOfBreakPoint - 1;
			if (index < m_extraTurnUnitList.size())//unit is in extra turn list
			{
				return m_extraTurnUnitList[index];
			}
			else//extra turn unit is passed
			{
				p_index -= index;//move index
			}
		}
	}

	if (p_index >= 0 && p_index < m_unitObjectList.size())
	{
		return m_unitObjectList[p_index];
	}
	//else if (p_index >= m_unitObjectList.size() && p_index < LISTSIZE(m_unitObjectList, m_extraTurnUnitList))
	//{
	//	int index = p_index - m_unitObjectList.size();
	//	return m_extraTurnUnitList[index];
	//}
	else if(p_index >= m_unitObjectList.size())
	{
		int size = m_unitObjectList.size();
		int index = p_index - size;
		return m_waitUnitObjectList[index];
	}
}

int unit::InitiativeTracker::getUnitNumber()
{
	return m_unitObjectList.size() + m_waitUnitObjectList.size() + m_extraTurnUnitList.size();
}

int unit::InitiativeTracker::getUnitNumberOnBoard()
{
	return m_unitObjectList.size() + m_waitUnitObjectList.size();
}

kitten::K_GameObject * unit::InitiativeTracker::getCurrentUnit()
{
	return getUnitByIndex(m_currentUnitIndex);
}

int unit::InitiativeTracker::getCurrentUnitIndex()
{
	return m_currentUnitIndex;
}

std::vector<kitten::K_GameObject*> unit::InitiativeTracker::getUnitList()
{
	return m_unitObjectList;
}

void unit::InitiativeTracker::gameTurnStart()
{/*
	// TEMPORARY
	// When dynamically adding a new unit, need to reset the turn of the
	// previously current unit
	// Will need a better solution to dynamically adding new units in the middle of a turn
	// instead of just calling gameTurnStart when one is added
	if (m_uturn->move || m_uturn->act)
	{
		m_uturn->turnReset();
	}
	*/

	//active aura
	m_uAura->setEnabled(true);

	//display new turn message
	m_display->displayNewGameTurn();

	//sort the unit object list
	addWaitList();
	sortListByIn();


	//change turn counter / trigger new game turn event
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::New_Game_Turn, nullptr);


	m_currentUnitIndex = 0;
	m_UI->turnStart();
	
	if (getUnitNumber() > 0)
	{
		kitten::K_GameObject* currentUnit = getCurrentUnit();
		m_uAura->getTransform().setParent(&currentUnit->getTransform());
		m_uturn->turnStart(currentUnit);//let the unit start its turn
	}
}

void unit::InitiativeTracker::unitTurnEnd()
{
	/*
	if (m_extraTurnUnitList.size() > 0)//has unit in extra turn list
	{
		kitten::K_GameObject* unit = *m_extraTurnUnitList.begin();//get first unit in list
		
		//set turn
		m_uAura->getTransform().setParent(&unit->getTransform());
	}*/

	m_currentUnitIndex++;//iterator point next unit
	if (m_currentUnitIndex < getUnitNumber())
	{
		kitten::K_GameObject* currentUnit = getCurrentUnit();
		m_uAura->getTransform().setParent(&currentUnit->getTransform());
		m_UI->next();
		m_uturn->turnStart(currentUnit);//let next unit start its turn
	}
	else// start a new game turn
	{
		gameTurnEnd();
	}
}

void unit::InitiativeTracker::gameTurnEnd()
{
	//clear extra turn list
	m_extraTurnUnitList.clear();

	//trigger game turn end event
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Game_Turn_End, nullptr);


	//start of new turn
	gameTurnStart();

	/*move to commander, so power will reset at their turn
	//reset power tracker
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Reset_Power, nullptr);
	*/
}

void unit::InitiativeTracker::addExtraTurn(kitten::K_GameObject * p_unit)
{
	m_indexOfBreakPoint = m_currentUnitIndex;
	if (m_extraTurnUnitList.size() == 0)
		m_extraTurnUnitList.push_back(p_unit);
	else if (m_extraTurnUnitList.size() == 1)
		m_extraTurnUnitList[0] = p_unit;
	m_UI->change(m_currentUnitIndex);
}

void unit::InitiativeTracker::newTurnListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	unit::Unit* currentUnit = getCurrentUnit()->getComponent<unit::Unit>();
	if (networking::ClientGame::getInstance()->getClientId() == currentUnit->m_clientId)
	{
		ActionButtonStore* abs = getCurrentUnit()->getComponent<UnitSelect>()->getButtonStorage();
		if (abs->isDisplay())//player is choosing action
			return;

		if (UnitInteractionManager::getInstance()->isBusy())//player uses ability
			return;

		currentUnit->playerSkipTurn();
	}
}
