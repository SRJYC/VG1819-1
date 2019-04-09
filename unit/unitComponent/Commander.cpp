#include "Commander.h"
#include "unitInteraction/UnitInteractionManager.h"
#include "UI/HandFrame.h"

// Networking
#include "networking\ClientGame.h"

#include "AI/controller.h"
namespace unit
{
	unit::Commander::Commander()
	{
	}

	unit::Commander::~Commander()
	{
		delete m_adSpawn;
		//delete m_adTile;

		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Summon_Unit, this);
		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Cancel_Summon, this);
	}

	void Commander::init(Unit* p_u)
	{
		m_unit = p_u;

		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Summon_Unit,
			this,
			std::bind(&Commander::spawnUnit, this, std::placeholders::_1, std::placeholders::_2));

		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Cancel_Summon,
			this,
			std::bind(&Commander::spawnUnit, this, std::placeholders::_1, std::placeholders::_2));

		//ad for summon unit
		m_adSpawn = new unit::AbilityDescription();
		m_adSpawn->m_stringValue["name"] = ABILITY_SUMMON_UNIT;
		m_adSpawn->m_intValue["target"] = 1;
		//range
		m_adSpawn->m_intValue["min_range"] = 1;
		m_adSpawn->m_intValue["max_range"] = 1;
		//filter
		m_adSpawn->m_intValue[FILTER] = 2;
		m_adSpawn->m_stringValue["filter0"] = FILTER_UNIT;
		m_adSpawn->m_stringValue["filter1"] = FILTER_UNOWNED_TILE;

		/*
		//ad for manipulate tile
		m_adTile = new unit::AbilityDescription();
		m_adTile->m_stringValue["name"] = ABILITY_MANIPULATE_TILE;
		m_adTile->m_intValue["target"] = 1;
		//range
		m_adTile->m_intValue["min_range"] = 1;
		m_adTile->m_intValue["max_range"] = 1;
		//filter
		m_adTile->m_intValue[FILTER] = 1;
		m_adTile->m_stringValue["filter0"] = FILTER_OWNED_TILE;*/
	}

	/*
	void Commander::manipulateTile()
	{
		if(m_unit->canAct())
			UnitInteractionManager::getInstance()->request(m_unit, m_adTile);
	}*/

	void Commander::spawnUnit(kitten::Event::EventType p_type, kitten::Event * p_data)
	{
		//not his turn
		if (!m_unit->isTurn())
			return;

		if (p_type == kitten::Event::Summon_Unit)
		{
			m_adSpawn->m_cardGOForUnitSummon = p_data->getGameObj(UNIT);
			UnitInteractionManager::getInstance()->request(m_unit, m_adSpawn);
		}
		else if (p_type == kitten::Event::Cancel_Summon)
		{
			UnitInteractionManager::getInstance()->cancel();
		}
	}

	void Commander::resetPower(int p_clientID)
	{
		//if this is multiplayer
		if (networking::ClientGame::isNetworkValid() || AI::controller::AIPresent())
		{
			networking::ClientGame* client = networking::ClientGame::getInstance();

			if (p_clientID == client->getClientId())//check this is player's commander
			{//then reset power
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Reset_Power, nullptr);

				// Draw a card
				int cardsDrawable = 5 - userinterface::HandFrame::getActiveInstance()->getTotalCardsInHand();
				kitten::Event *e = new kitten::Event(kitten::Event::EventType::Draw_Card);// make a draw event
				e->putInt(PLAYER_ID, p_clientID); // give it a player id, 0 being default TODO change this if ever needed. 0 signifies this side's instance player's id. any other should be any number other than 0 
				e->putInt(CARD_COUNT,min(2,cardsDrawable)); //tell to draw at max 2 cards
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::EventType::Draw_Card, e); // Call the event
			}
		}
		else
		{//direct reset power
			kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Reset_Power, nullptr);
		}
	}
}

