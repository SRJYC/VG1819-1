#include "UnitTest.h"
#include "ability/node/AbilityNodeManager.h"
#include "unit/InitiativeTracker/InitiativeTracker.h"
#include <iostream>
#include "kibble/kibble.hpp"
#include "kibble/databank/databank.hpp"

// Includes for tile highlighting and manipulating tile
#include "_Project\UseAbilityWhenClicked.h"
#include "_Project\PrintWhenClicked.h"
#include "kitten\K_ComponentManager.h"

//Includes for creating spawn tile
#include "unit/unitComponent/UnitMove.h"
#include "kitten/K_GameObjectManager.h"

#include "_Project/BoardCreator.h"

//Rock
//test the unit data

namespace unit
{
	UnitTest* UnitTest::m_instance = nullptr;

	UnitTest::UnitTest()
	{
	}

	UnitTest * UnitTest::getInstance()
	{
		return m_instance;
	}

	UnitTest * UnitTest::getInstanceSafe()
	{
		if (m_instance == nullptr)
			m_instance = new UnitTest();
		return m_instance;
	}

	void UnitTest::test()
	{
		kibble::UnitDataParser* parser = kibble::getUnitDataParserInstance();

		
		// Testing highlighting tiles and manipulating tiles using testDummy.txt
		{
			kitten::K_ComponentManager* compMan = kitten::K_ComponentManager::getInstance();
			//kitten::K_GameObject* testDummyGO = UnitSpawn::getInstance()->spawnUnitObject(parser->getUnit("testDummy.txt"));
			kitten::K_GameObject* testDummyGO = UnitSpawn::getInstance()->spawnUnitObject(kibble::getUnitFromId(0));
			unit::Unit* testDummy = testDummyGO->getComponent<unit::Unit>();
			
			UnitMonitor::getInstanceSafe()->printUnit(testDummy);
			// Moved PrintWhenClicked and UseAbilityWhenClicked into UnitSpawn::spawnUnitObject() for spawning a Commander
			
			//initialize position
			//testDummyGO->getComponent<unit::UnitMove>()->setTile(BoardCreator::getTile(0, 0));
			testDummy->m_attributes["in"] = 1;
		} 
		// End testing selecting spawned unit
		

		/*
		//create test tile for unit spawn
		kitten::K_GameObject* testTile = kitten::K_GameObjectManager::getInstance()->createNewGameObject("tileobj.txt");

		PrintWhenClicked* printWhenClick = static_cast<PrintWhenClicked*>(kitten::K_ComponentManager::getInstance()->createComponent("PrintWhenClicked"));
		printWhenClick->setMessage("spawn tile is clicked");
		testTile->addComponent(printWhenClick);

		kitten::K_Component* clickBox = kitten::K_ComponentManager::getInstance()->createComponent("ClickableBox");
		testTile->addComponent(clickBox);

		testTile->getTransform().place(0.0f, -1.0f, 16.0f);
		//end of test tile
		*/


		kitten::K_GameObject* u1 = UnitSpawn::getInstance()->spawnUnitObject(parser->getUnit("Priest.txt"));
		kitten::K_GameObject* u2 = UnitSpawn::getInstance()->spawnUnitObject(parser->getUnit("Engineer.txt"));
		//u1->getTransform().move(10.0f, 0.0f, 0.0f);
		//set initial position
		u1->getComponent<unit::UnitMove>()->setTile(BoardCreator::getTile(1, 1));
		u2->getComponent<unit::UnitMove>()->setTile(BoardCreator::getTile(0, 1));

		//test unit 
		unit::Unit* u = u1->getComponent<unit::Unit>();
		//UnitMonitor::getInstanceSafe()->printUnit(u);

		//Test Initiative Tracker
		//kitten::K_GameObject* u2 = UnitSpawn::getInstance()->spawnUnitObject(parser->getUnit("Engineer.txt"));
		//kitten::K_GameObject* u3 = UnitSpawn::getInstance()->spawnUnitObject(parser->getUnit("Duelist.txt"));

		UnitMonitor::getInstanceSafe()->printIT();
		InitiativeTracker::getInstance()->gameTurnStart();
		//UnitMonitor::getInstanceSafe()->printIT();
		//InitiativeTracker::getInstance()->removeUnit(u2);
		//UnitMonitor::getInstanceSafe()->printIT();
	}
}

