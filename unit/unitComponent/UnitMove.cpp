#include "UnitMove.h"
#include "unit/Unit.h"
#include "board/tile/TileInfo.h"
#include "board/BoardManager.h"
#include "unitInteraction/UnitInteractionManager.h"
#include <iostream>
#include <cmath>

void unit::UnitMove::triggerNewTileEvent()
{
	//trigger new tile event
	Unit* u = m_attachedObject->getComponent<Unit>();

	ability::TimePointEvent* t = new ability::TimePointEvent(ability::TimePointEvent::New_Tile);
	t->putGameObject("tile", m_currentTile);
	u->triggerTP(ability::TimePointEvent::New_Tile,t);
}

void unit::UnitMove::triggerLeaveTileEvent()
{
	//trigger new tile event
	Unit* u = m_attachedObject->getComponent<Unit>();

	//ability::TimePointEvent* t = new ability::TimePointEvent(ability::TimePointEvent::Leave_Tile);
	//t->putGameObject("tile", m_currentTile);
	u->triggerTP(ability::TimePointEvent::Leave_Tile);
}

unit::UnitMove::UnitMove(glm::vec3 p_offset, float p_speed) : 
	m_speed(p_speed),
	m_offset(p_offset),
	m_currentTile(nullptr)
{
	//initialize ad
	m_ad = new unit::AbilityDescription();

	m_ad->m_stringValue["name"] = ACTION_MOVE;
	//m_ad->m_intValue["target"] = 1;

	//filter
	m_ad->m_intValue["need_unit"] = FALSE;
	//m_ad->m_intValue[FILTER] = 1;
	//m_ad->m_stringValue["filter0"] = FILTER_UNIT;
	//area, in this case, path
	m_ad->m_stringValue[AREA_MODE] = PATH;
	m_ad->m_intValue[AREA_FIX] = FALSE;
}

unit::UnitMove::~UnitMove()
{
	if (m_ad != nullptr)
		delete m_ad;
}
/*
void unit::UnitMove::registerListener()
{
	notRegistered = false;

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Tile_Clicked,
		this,
		std::bind(&UnitMove::listenEvent, this, std::placeholders::_1, std::placeholders::_2));
}

void unit::UnitMove::deregisterListener()
{
	notRegistered = true;

	kitten::EventManager::getInstance()->queueRemoveListener(kitten::Event::Tile_Clicked, this);
}

void unit::UnitMove::listenEvent(kitten::Event::EventType p_type, kitten::Event * p_data)
{
	if (p_type == kitten::Event::Tile_Clicked)
	{
		
		bool highlighted = p_data->getInt("highlighted");
		if (highlighted)//move
		{
			move(p_data->getGameObj("tile0"));
		}
		else//cancel move
		{
			std::cout << "Cancel Move" << std::endl;
			triggerUnhighLightEvent();
		}
		deregisterListener();
	}
}
*/
void unit::UnitMove::attempToMove(int p_min, int p_max)
{
	unit::Unit* u = m_attachedObject->getComponent<unit::Unit>();

	//reset min and max
	m_ad->m_intValue["min_range"] = p_min;
	if (p_max < 0)
		m_ad->m_intValue["max_range"] = m_attachedObject->getComponent<Unit>()->m_attributes["mv"];//the range is between 1 and mv attributes
	else
		m_ad->m_intValue["max_range"] = p_max;

	m_ad->m_intValue[AREA_LEN] = m_ad->m_intValue["max_range"];

	UnitInteractionManager::getInstance()->request(u, m_ad);
}
/*
void unit::UnitMove::triggerHighLightEvent(int p_min, int p_max)
{
	//trigger the highlight event shows what are possible move
	kitten::Event* e = new kitten::Event(kitten::Event::Highlight_Tile);
	e->putString(TILE_OWNER_KEY, m_attachedObject->getComponent<Unit>()->m_name + " Move.");//highlight because of this unit move
	e->putInt("min_range", p_min);

	if (p_max < 0)
		e->putInt("max_range", m_attachedObject->getComponent<Unit>()->m_attributes["mv"]);//the range is between 1 and mv attributes
	else
		e->putInt("max_range", p_max);
	e->putGameObj("tileAtOrigin", m_currentTile);
	e->putString("mode", "range");
	e->putString("use", "move");
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Highlight_Tile, e);
}

void unit::UnitMove::triggerUnhighLightEvent()
{
	kitten::Event* e = new kitten::Event(kitten::Event::Unhighlight_Tile);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Unhighlight_Tile, e);
}*/

void unit::UnitMove::dontSetTileAfterMove()
{
	m_tileSetUnit = false;
}

void unit::UnitMove::move(kitten::K_GameObject * p_targetTile)
{
	//remove unit from current tile
	m_currentTile->getComponent<TileInfo>()->removeUnit();

	m_lastTile = m_currentTile;//set current to last

	triggerLeaveTileEvent();

	m_currentTile = p_targetTile;//set target to current

	//set distance
	distanceX = m_currentTile->getTransform().getTranslation().x - m_lastTile->getTransform().getTranslation().x;
	distanceZ = m_currentTile->getTransform().getTranslation().z - m_lastTile->getTransform().getTranslation().z;

}

void unit::UnitMove::setTile(kitten::K_GameObject * p_tile)
{
	if (m_currentTile != nullptr)
	{
		//remove unit from current tile
		m_currentTile->getComponent<TileInfo>()->removeUnit();
	}
	//std::string name = m_attachedObject->getComponent<Unit>()->m_name;
	//add this to target tile
	p_tile->getComponent<TileInfo>()->setUnit(m_attachedObject);

	m_currentTile = p_tile;
	m_lastTile = p_tile;
	reset();

	triggerNewTileEvent();
}

void unit::UnitMove::setTile(int p_x, int p_z)
{
	m_set = true;
	m_tileX = p_x;
	m_tileZ = p_z;
}

kitten::K_GameObject * unit::UnitMove::getTile()
{
	return m_currentTile;
}

void unit::UnitMove::reset()
{
	//set current tile
	if (m_tileSetUnit)
	{
		m_currentTile->getComponent<TileInfo>()->setUnit(m_attachedObject);
	}
	else
	{
		m_currentTile = m_lastTile;
		m_tileSetUnit = true;
	}

	//set the unit's position to current tile
	glm::vec3 t = m_currentTile->getTransform().getTranslation();//get tile's translation
	m_attachedObject->getTransform().place(t.x, t.y, t.z);//set unit object to that tile
	m_attachedObject->getTransform().move(m_offset.x, m_offset.y, m_offset.z);//move unit upward
}

void unit::UnitMove::reach()
{
	reset();
	m_lastTile = m_currentTile;//set current tile as last

	triggerNewTileEvent();

	//tell unit object move is done
	m_attachedObject->getComponent<unit::Unit>()->moveDone();
}

bool unit::UnitMove::hasUpdate() const
{
	return true;
}

void unit::UnitMove::update()
{
	if (m_set)
	{
		if (m_tileX >= 0 && m_tileZ >= 0)
		{
			setTile(BoardManager::getInstance()->getTile(m_tileX, m_tileZ));
		}
		else
		{
			int clientid = m_attachedObject->getComponent<Unit>()->m_clientId;
			setTile(BoardManager::getInstance()->getSpawnPoint(clientid));
		}
		m_set = false;
	}

	if (m_currentTile != m_lastTile)
	{
		float velocity = kitten::K_Time::getInstance()->getDeltaTime() * m_speed * 60.0f;//delta time * speed * 60fps
		
		if (distanceX == 0)//if unit has same x value as target tile
		{
			if (distanceZ > 0)//move in positive direction
			{
				if (distanceZ > velocity)//not close
				{
					m_attachedObject->getTransform().move(0.0f, 0.0f, velocity);
					distanceZ -= velocity;
				}
				else//vecy close, 
				{
					m_attachedObject->getTransform().move(0.0f, 0.0f, distanceZ);
					distanceZ = 0;
				}
			}
			if (distanceZ < 0)
			{
				if (distanceZ < -velocity)
				{
					m_attachedObject->getTransform().move(0.0f, 0.0f, -velocity);
					distanceZ += velocity;
				}
				else
				{
					m_attachedObject->getTransform().move(0.0f, 0.0f, distanceZ);
					distanceZ = 0;
				}
			}
			if (distanceZ == 0)//unit is at target tile
			{
				reach();
			}
		}
		else//not have same x value
		{
			if (distanceX > 0)
			{
				if (distanceX > velocity)
				{
					m_attachedObject->getTransform().move(velocity, 0.0f, 0.0f);
					distanceX -= velocity;
				}
				else
				{
					m_attachedObject->getTransform().move(distanceX, 0.0f, 0.0f);
					distanceX = 0;
				}
			}
			if (distanceX < 0)
			{
				if (distanceX < -velocity)
				{
					m_attachedObject->getTransform().move(-velocity, 0.0f, 0.0f);
					distanceX += velocity;
				}
				else
				{
					m_attachedObject->getTransform().move(distanceX, 0.0f, 0.0f);
					distanceX = 0;
				}
			}
		}
	}
}
