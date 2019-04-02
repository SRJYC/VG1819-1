// SummonUnit
//
// Handles the SummonUnit ability used by Commanders
//
// @Ken

#include "ability\ability\Ability.h"
#include "kitten\event_system\EventManager.h"
#include "unit\Unit.h"
#include "unit/unitComponent/UnitMove.h"
#include "components/PowerTracker.h"
#include "board/BoardManager.h"
#include "unit/UnitSpawn.h"
#include "kitten/K_GameObjectManager.h"
#include "kibble/databank/databank.hpp"
#include "networking\ClientGame.h"
#include "board\tile\TileInfo.h"
#include "components/DragNDrop/SpawnUnitOnDrop.h"
#include "_Project\UniversalPfx.h"

namespace ability
{
	int SummonUnit::effect(AbilityInfoPackage* p_info)
	{

		PowerTracker* powerTracker = BoardManager::getInstance()->getPowerTracker();

		kitten::K_GameObject* targetTile = p_info->m_targetTilesGO[0];

		// Unit setup
		unit::Unit* unit = p_info->m_cardGOForUnitSummon->getComponent<unit::Unit>();
		if(unit->m_clientId == -1)
			unit->m_clientId = p_info->m_sourceClientId;

		// Update Power Tracker
		if (p_info->m_sourceClientId == networking::ClientGame::getClientId() || networking::ClientGame::getClientId() == -1)
		{
			BoardManager::getInstance()->getPowerTracker()->summonUnitCost(unit->m_attributes[UNIT_COST]);
		}

		// Generate Unit and set Tile
		kitten::K_GameObject* summonedUnitGO = unit::UnitSpawn::getInstance()->spawnUnitObject(unit);
		summonedUnitGO->getComponent<unit::UnitMove>()->setTile(targetTile);
		UniversalPfx::getInstance()->playEffect("Summon", targetTile->getTransform().getTranslation());


		SpawnUnitOnDrop* onDrop = unit->getGameObject().getComponent<SpawnUnitOnDrop>();
		if (onDrop != nullptr)
		{
			onDrop->removeCard();
		}
		/*
		//get Unit data
		unit::Unit* u = kibble::getUnitFromId(unitId);
		if (u->m_attributes[UNIT_COST] <= powerTracker->getCurrentPower())
		{
			kitten::K_GameObject* uGO = unit::UnitSpawn::getInstance()->spawnUnitObject(2);

			powerTracker->summonUnitCost(u->m_attributes[UNIT_COST]);

			kitten::K_GameObject* tile = p_info->m_targetTilesGO[0];
			uGO->getComponent<unit::UnitMove>()->setTile(tile);

			if (networking::ClientGame::getInstance() != nullptr)
			{
				uGO->getComponent<unit::Unit>()->m_clientId = p_info->m_sourceClientId;
			}

		}
		*/
		/*
		kitten::Event* p_data = new kitten::Event(kitten::Event::EventType::Highlight_Tile);
		//p_data->putTileList(&p_info->m_targetTiles);
		p_data->putString(TILE_OWNER_KEY, p_info->m_source->m_ID);
		p_data->putGameObj("tileAtOrigin", p_info->m_source->getTile());
		p_data->putString("mode", "range");
		p_data->putString("use", "summon");
		p_data->putInt("min_range",1);
		p_data->putInt("max_range", 1);
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::EventType::Highlight_Tile, p_data);
		*/

		UniversalSounds::playSound("summon");

		//delete package
		delete p_info;

		return 0;
	}
}