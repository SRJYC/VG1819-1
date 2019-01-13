// SpawnUnitOnKeyPress
//
// Used to spawn a unit on a key press to test with networking code
// 
// @Ken

#include "networking\SpawnUnitOnKeyPress.h"
#include "kitten\InputManager.h"

#include "kitten/K_GameObjectManager.h"
#include "kitten\K_ComponentManager.h"
#include "kibble/databank/databank.hpp"

// Unit stuff
#include "unit/unitComponent/UnitMove.h"
#include "unit/UnitTest.h"
#include "unit\UnitSpawn.h"
#include "board\BoardManager.h"

#include "unit\InitiativeTracker\InitiativeTracker.h"

// Networking stuf
#include "networking\NetworkData.h"

SpawnUnitOnKeyPress::SpawnUnitOnKeyPress()
	:
	m_bGameStarted(false),
	m_bUnitsSpawned(false),
	m_bEnteringData(false),
	m_unitIdEntered(false),
	m_tileDataReceived(false),
	m_initialUnitsSpawned(false)
{
	//unit spawn create is in gameplay-init now
	//unit::UnitSpawn::getInstance();
}

SpawnUnitOnKeyPress::~SpawnUnitOnKeyPress()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Tile_Clicked_Debug, this);
}

void SpawnUnitOnKeyPress::start()
{
	m_textBox = m_attachedObject->getComponent<puppy::TextBox>();
	assert(m_textBox != nullptr);

	m_stringInputDisplay = m_attachedObject->getComponent<StringInputDisplay>();
	assert(m_stringInputDisplay != nullptr);

	m_inputMan = input::InputManager::getInstance();
	assert(m_inputMan != nullptr);

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Tile_Clicked_Debug,
		this,
		std::bind(&SpawnUnitOnKeyPress::getTileListener, this, std::placeholders::_1, std::placeholders::_2));
}

void SpawnUnitOnKeyPress::update()
{
	if (networking::ClientGame::isNetworkValid())
	{
		if (input::InputManager::getInstance()->keyDown('S') && !input::InputManager::getInstance()->keyDownLast('S'))
		{
			if (!m_unitIdEntered)
			{
				m_textBox->setText("Enter Unit ID:");
				m_inputMan->setPollMode(false);
				m_bEnteringData = true;
			}
		}

		if (m_inputMan->keyDown(GLFW_KEY_ENTER) && !m_inputMan->keyDownLast(GLFW_KEY_ENTER) && m_bEnteringData)
		{
			if (!m_unitIdEntered)
			{
				std::string unitId = m_stringInputDisplay->getString();
				if (unitId == "all" && !m_initialUnitsSpawned)
				{
					summonAllUnits();
				} 
				else if (unitId == "network" && !m_initialUnitsSpawned && !m_bUnitsSpawned)
				{
					summonUnitsForNetworkExample();
				}
				else
				{
					// Convert id from string to int
					try
					{
						m_summonUnitId = std::stoi(unitId);
					} catch (const std::invalid_argument& ia)
					{
						m_summonUnitId = -1;
					}

					// Check if the id is a valid unit id
					if (m_summonUnitId < unit::UnitSpawn::FIRST_UNIT_INDEX || m_summonUnitId > unit::UnitSpawn::MAX_UNIT_INDEX || m_summonUnitId == unit::UnitSpawn::newUnitObject)
					{
						m_textBox->setText("Re-enter unit ID:");
						m_inputMan->setPollMode(false);
					} else // Finally, wait for user to click on a tile to summon
					{
						m_textBox->setText("Click a tile to summon");
						m_unitIdEntered = true;
					}
				}
			}
		}

		if (m_unitIdEntered && m_tileDataReceived)
		{
			networking::ClientGame* client = networking::ClientGame::getInstance();
			int clientId = client->getClientId();
			client->summonUnit(clientId, m_summonUnitId, m_summonTileX, m_summonTileY);
			client->sendSummonUnitPacket(m_summonUnitId, m_summonTileX, m_summonTileY);

			m_bUnitsSpawned = true;
			m_unitIdEntered = false;
			m_bEnteringData = false;
			m_tileDataReceived = false;
			m_inputMan->setPollMode(true);
			m_textBox->setText("");
		}

		if (input::InputManager::getInstance()->keyDown('G') && !input::InputManager::getInstance()->keyDownLast('G') && !m_bGameStarted)
		{
			m_bGameStarted = true;
			networking::ClientGame* client = networking::ClientGame::getInstance();
			if (!client->isGameTurnStarted())
			{
				client->sendBasicPacket(GAME_TURN_START);
			}
		}
	}
}

void SpawnUnitOnKeyPress::summonAllUnits()
{
	networking::ClientGame* client = networking::ClientGame::getInstance();
	int clientId = client->getClientId();

	// King
	client->summonUnit(clientId, unit::UnitSpawn::King, 7, 7);
	client->sendSummonUnitPacket(unit::UnitSpawn::King, 7, 7);

	// Queen
	client->summonUnit(clientId, unit::UnitSpawn::Queen, 6, 7);
	client->sendSummonUnitPacket(unit::UnitSpawn::Queen, 6, 7);

	// StoneSlinger
	client->summonUnit(clientId, unit::UnitSpawn::StoneSlinger, 2, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::StoneSlinger, 2, 4);

	// Priest
	client->summonUnit(clientId, unit::UnitSpawn::Priest, 3, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::Priest, 3, 4);

	// Priest
	client->summonUnit(clientId, unit::UnitSpawn::Priest, 4, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::Priest, 4, 4);

	// Oligarch
	client->summonUnit(clientId, unit::UnitSpawn::Oligarch, 5, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::Oligarch, 5, 4);

	// Duelist
	client->summonUnit(clientId, unit::UnitSpawn::Duelist, 6, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::Duelist, 6, 4);

	// Engineer
	client->summonUnit(clientId, unit::UnitSpawn::Engineer, 7, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::Engineer, 7, 4);

	// Pyromancer
	client->summonUnit(clientId, unit::UnitSpawn::Pyromancer, 8, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::Pyromancer, 8, 4);

	// Archer
	client->summonUnit(clientId, unit::UnitSpawn::Archer, 9, 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::Archer, 9, 4);

	// Wall
	client->summonUnit(clientId, unit::UnitSpawn::Wall, 5, 2);
	client->sendSummonUnitPacket(unit::UnitSpawn::Wall, 5, 2);

	// Armory
	client->summonUnit(clientId, unit::UnitSpawn::Armory, 7, 2);
	client->sendSummonUnitPacket(unit::UnitSpawn::Armory, 7, 2);

	// ArrowTower
	client->summonUnit(clientId, unit::UnitSpawn::ArrowTower, 8, 2);
	client->sendSummonUnitPacket(unit::UnitSpawn::ArrowTower, 8, 2);

	// DivineStatue
	client->summonUnit(clientId, unit::UnitSpawn::DivineStatue, 9, 2);
	client->sendSummonUnitPacket(unit::UnitSpawn::DivineStatue, 9, 2);

	m_initialUnitsSpawned = true;
	m_bUnitsSpawned = true;
}

void SpawnUnitOnKeyPress::summonUnitsForNetworkExample()
{
	networking::ClientGame* client = networking::ClientGame::getInstance();
	int clientId = client->getClientId();
	int col = 0;

	if (clientId == 0)
		col = 9;
	else if (clientId == 1)
		col = 5;
	int row = 8;

	// Commander, King for host, Queen for client
	// For testing, Spawned in on 2 connections in ClientGame/ServerGame
	//client->summonUnit			(clientId, unit::UnitSpawn::King + clientId, col, row);
	//client->sendSummonUnitPacket(clientId, unit::UnitSpawn::King + clientId, col, row);

	client->summonUnit(clientId, unit::UnitSpawn::Priest, col, row - 1);
	client->sendSummonUnitPacket(unit::UnitSpawn::Priest, col, row - 1);

	client->summonUnit(clientId, unit::UnitSpawn::Archer, col, row - 2);
	client->sendSummonUnitPacket(unit::UnitSpawn::Archer, col, row - 2);

	client->summonUnit(clientId, unit::UnitSpawn::Duelist, col, row - 3);
	client->sendSummonUnitPacket(unit::UnitSpawn::Duelist, col, row - 3);

	client->summonUnit(clientId, unit::UnitSpawn::DivineStatue, col, row - 4);
	client->sendSummonUnitPacket(unit::UnitSpawn::DivineStatue, col, row - 4);

	m_initialUnitsSpawned = true;
	m_bUnitsSpawned = true;
}

void SpawnUnitOnKeyPress::getTileListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	printf("getTileListener()\n");
	if (m_unitIdEntered)
	{
		int tileX = p_event->getInt(TILE_POS_X);
		int tileY = p_event->getInt(TILE_POS_Y);
		TileInfo* info = BoardManager::getInstance()->getTile(tileX, tileY)->getComponent<TileInfo>();
		if (info->hasUnit())
		{
			m_textBox->setText("Tile occupied, reselect");
		} else
		{
			m_tileDataReceived = true;
			m_summonTileX = tileX;
			m_summonTileY = tileY;
		}
	}
}