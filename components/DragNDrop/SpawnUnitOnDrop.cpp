#include "SpawnUnitOnDrop.h"
#include "unit/Unit.h"
#include "unit/UnitSpawn.h"
#include "kitten/InputManager.h"
#include "board/tile/TileInfo.h"
#include "unit/unitComponent/UnitMove.h"
#include "kitten/K_GameObjectManager.h"
#include "kibble/databank/databank.hpp"
#include "UI/HandFrame.h"
#include "UI/CardUIO.h"
#include "components/PowerTracker.h"
#include "board/BoardManager.h"
#include <iostream>

// Networking
#include "networking\ClientGame.h"

#define CARD_HOVER_MOVE_TIME 0.2

void SpawnUnitOnDrop::onClick()
{
	if (networking::ClientGame::isNetworkValid())
	{
		if (unit::InitiativeTracker::getInstance()->getCurrentUnitIndex() >= 0)
		{
			unit::Unit* currentUnit = unit::InitiativeTracker::getInstance()->getCurrentUnit()->getComponent<unit::Unit>();
			unit::Unit* clientCommander = networking::ClientGame::getInstance()->getCommander();
			if (currentUnit == clientCommander)
			{
				DragNDrop::onClick();
			}
		}
	}
	else
	{
		DragNDrop::onClick();
	}
}

void SpawnUnitOnDrop::onDrop()
{
	// Check if we hit something
	kitten::K_GameObject* targetTile = input::InputManager::getInstance()->getMouseLastHitObject();
	if (targetTile == nullptr  // No Target
		|| targetTile->getComponent<TileInfo>() == nullptr // Target isn't a tile
		|| targetTile->getComponent<TileInfo>()->hasUnit() // Target tile already has a unit on it
		)
	{
		DragNDrop::onDrop();
		return;
	}
	
	// Unit setup
	unit::Unit* unit = m_attachedObject->getComponent<unit::Unit>();
	if (unit == nullptr) unit = kibble::getUnitFromId(1); // basically defaults to a priest. TODO remove this line when hand fully functional.

	// Check for unit stuff.
	if(BoardManager::getInstance()->getPowerTracker()->getCurrentPower() < unit->m_attributes[UNIT_COST] // Check if there is enough power to spawn this.
		)
	{
		DragNDrop::onDrop();
		return;
	}

	// Update Power Tracker
	BoardManager::getInstance()->getPowerTracker()->summonUnitCost(unit->m_attributes[UNIT_COST]);

	// Generate Unit and set Tile
	unit::UnitSpawn::getInstance()->spawnUnitObject(unit)->getComponent<unit::UnitMove>()->setTile(targetTile);

	// Send the summoned unit if we're playing multiplayer
	if (networking::ClientGame::isNetworkValid())
	{
		TileInfo* tileInfo = targetTile->getComponent<TileInfo>();
		networking::ClientGame::getInstance()->sendSummonUnitPacket(unit->m_kibbleID, tileInfo->getPosX(), tileInfo->getPosY());
	}

	// Remove Card from hand
	userinterface::CardUIO* cardUIObject = this->m_attachedObject->getComponent<userinterface::CardUIO>();
	userinterface::HandFrame::getActiveInstance()->removeCard((userinterface::UIObject*)cardUIObject);

	// Delete Card
	kitten::K_GameObjectManager::getInstance()->destroyGameObjectWithChild(this->m_attachedObject);

	// Disable Card Context
	m_cardContext->setEnabled(false);
}

void SpawnUnitOnDrop::onHoverEnd() {
	if (!m_isDragging)
	{
		//getTransform().place2D(m_origin.x, m_origin.y);
		m_lerpController->positionLerp(m_origin, CARD_HOVER_MOVE_TIME);
		m_isHovered = false;
	}
	m_cardContext->setEnabled(false);
}

void SpawnUnitOnDrop::onHoverStart() {
	if (!m_isDragging)
	{
		m_isHovered = true;
		//getTransform().place2D(m_origin.x, m_origin.y + 50);
		m_lerpController->positionLerp(glm::vec3(m_origin.x,m_origin.y + 50 , m_origin.z), CARD_HOVER_MOVE_TIME);
	}
	// TODO: Set the unit from the proper attached Unit
	m_cardContext->setUnit(kibble::getUnitFromId(1));
	m_cardContext->setEnabled(true);
}

void SpawnUnitOnDrop::onPause()
{
	DragNDrop::onDrop();
	m_isDragging = false;
	m_cardContext->setEnabled(false);
}

void SpawnUnitOnDrop::onPosChanged(const glm::vec3& p_newPos)
{
	if (!m_isDragging && !m_isHovered && !(m_lerpController != nullptr && m_lerpController->isPosLerping()))
	{
		m_origin = m_attachedObject->getTransform().getTranslation();
	}
} 

SpawnUnitOnDrop::SpawnUnitOnDrop()
	:
	DragNDrop(true),
	m_cardContext(nullptr)
{

}

SpawnUnitOnDrop::~SpawnUnitOnDrop()
{
	getTransform().removePositionListener(this);
}

void SpawnUnitOnDrop::start()
{
	DragNDrop::start();
	getTransform().addPositionListener(this);
}