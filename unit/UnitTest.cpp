#include "UnitTest.h"
#include "ability/node/AbilityNodeManager.h"
#include "unit/InitiativeTracker/InitiativeTracker.h"
#include <iostream>
#include "kibble/kibble.hpp"
#include "kibble/databank/databank.hpp"
#include "unitComponent\unitAction\ActionButtonStore.h"
#include "networking/ClientGame.h"

// Includes for tile highlighting and manipulating tile
#include "_Project\UseAbilityWhenClicked.h"
//board clickable
#include "board/clickable/PrintWhenClicked.h"
#include "kitten\K_ComponentManager.h"

//Includes for creating spawn tile
#include "unit/unitComponent/UnitMove.h"
#include "kitten/K_GameObjectManager.h"

#include "board/BoardManager.h"
#include "kitten/sprites/SpriteGroup.h"
//Rock
//test the unit data

namespace unit
{
	UnitTest* UnitTest::m_instance = nullptr;
	bool UnitTest::m_bTested = false;

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

	void UnitTest::destroyInstance()
	{
		if(m_instance != nullptr)
			delete m_instance;
	}

	void UnitTest::test()
	{
		m_bTested = true;
		//comment for testing IT


	   	//kitten::K_GameObject* uNEWDUMMY = UnitSpawn::getInstance()->spawnUnitObject(13); // 13 king !!!
		//uNEWDUMMY->getComponent<unit::UnitMove>()->setTile(6, 6);
		//uNEWDUMMY->getComponent<Unit>()->m_clientId = 1;

		kitten::K_GameObject* uNEWDUMMY2 = UnitSpawn::getInstance()->spawnUnitObject(14); // queen !!!
		uNEWDUMMY2->getComponent<unit::UnitMove>()->setTile(BoardManager::getInstance()->getSpawnPoint(0));
		uNEWDUMMY2->getComponent<unit::Unit>()->m_clientId = 0;
		
		/*
		// Testing highlighting tiles and manipulating tiles using testDummy.txt
		{
			kitten::K_GameObject* testDummyGO = UnitSpawn::getInstanceSafe()->spawnUnitObject(0);
			unit::Unit* testDummy = testDummyGO->getComponent<unit::Unit>();
			UnitMonitor::getInstanceSafe()->printUnit(testDummy);

			//initialize position
			testDummyGO->getComponent<unit::UnitMove>()->setTile(4,4);

			//move to unit spawn
			//kitten::K_ComponentManager* compMan = kitten::K_ComponentManager::getInstance();
			//testDummyGO->addComponent(compMan->createComponent("UseAbilityWhenClicked"));
			//testDummyGO->addComponent(compMan->createComponent("SelectAbility"));

			//PrintWhenClicked* printWhenClick = static_cast<PrintWhenClicked*>(compMan->createComponent("PrintWhenClicked"));
			//printWhenClick->setMessage("Test Dummy");
			//testDummyGO->addComponent(printWhenClick);
		}*/
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
		
		//set initial position
		
		//kitten::K_GameObject* u0 = UnitSpawn::getInstance()->spawnUnitObject(2);//archer
		//u0->getComponent<unit::UnitMove>()->setTile(3, 3);

//		kitten::K_GameObject* uNEWDUMMY = UnitSpawn::getInstance()->spawnUnitObject(13); // 13 king !!!
//		uNEWDUMMY->getComponent<unit::UnitMove>()->setTile(-1, -1);
//		uNEWDUMMY->getComponent<Unit>()->m_clientId = 1;

//		kitten::K_GameObject* uNEWDUMMY2 = UnitSpawn::getInstance()->spawnUnitObject(14); // queen !!!
//		uNEWDUMMY2->getComponent<unit::UnitMove>()->setTile(-1, -1);
//		uNEWDUMMY2->getComponent<Unit>()->m_clientId = 0;

//		kitten::K_GameObject* u0 = UnitSpawn::getInstance()->spawnUnitObject(2);//archer
//		u0->getComponent<unit::UnitMove>()->setTile(6, 10);
//		u0->getComponent<Unit>()->m_attributes[UNIT_LV] = 3;
//		u0->getComponent<Unit>()->m_clientId = 0;

//		kitten::K_GameObject* u1 = UnitSpawn::getInstance()->spawnUnitObject(1);//priest
//		u1->getComponent<unit::UnitMove>()->setTile(2, 5);

//		kitten::K_GameObject* u1b = UnitSpawn::getInstance()->spawnUnitObject(1);//priest
//		u1b->getComponent<unit::UnitMove>()->setTile(1,5);

//		kitten::K_GameObject* u3 = UnitSpawn::getInstance()->spawnUnitObject(3);//duelist
//		u3->getComponent<unit::UnitMove>()->setTile(4, 2);

//		kitten::K_GameObject* u4 = UnitSpawn::getInstance()->spawnUnitObject(4);//engineer
//		u4->getComponent<unit::UnitMove>()->setTile(5, 2);
		
//		kitten::K_GameObject* u5 = UnitSpawn::getInstance()->spawnUnitObject(7);//pyromancer
//		u5->getComponent<unit::UnitMove>()->setTile(3, 5);

//		kitten::K_GameObject* u6 = UnitSpawn::getInstance()->spawnUnitObject(8);//stone slinger
//		u6->getComponent<unit::UnitMove>()->setTile(10, 4);

//		kitten::K_GameObject* u7 = UnitSpawn::getInstance()->spawnUnitObject(9);//arrow tower
//		u7->getComponent<unit::UnitMove>()->setTile(3, 6);

//		kitten::K_GameObject* u8 = UnitSpawn::getInstance()->spawnUnitObject(10);//armory
//		u8->getComponent<unit::UnitMove>()->setTile(4, 3);

//		kitten::K_GameObject* u9 = UnitSpawn::getInstance()->spawnUnitObject(11);//divine status
//		u9->getComponent<unit::UnitMove>()->setTile(4, 4);
		
//		kitten::K_GameObject* u10 = UnitSpawn::getInstance()->spawnUnitObject(12);//enchanter
//		u10->getComponent<unit::UnitMove>()->setTile(4, 5);
//		u10->getComponent<unit::Unit>()->m_attributes[UNIT_LV] = 3;
//		u10->getComponent<unit::Unit>()->m_attributes[COUNTER_ENERGY] = 4;

//		kitten::K_GameObject* u11 = UnitSpawn::getInstance()->spawnUnitObject(5);//wall
//		u11->getComponent<unit::UnitMove>()->setTile(5, 3);
		
//		kitten::K_GameObject* u12 = UnitSpawn::getInstance()->spawnUnitObject(15);//eternal eye
//		u12->getComponent<unit::UnitMove>()->setTile(8, 4);

//		kitten::K_GameObject* u13 = UnitSpawn::getInstance()->spawnUnitObject(16);//lancer
//		u13->getComponent<unit::UnitMove>()->setTile(5, 5);

//		kitten::K_GameObject* u14 = UnitSpawn::getInstance()->spawnUnitObject(17);//mud demon
//		u14->getComponent<unit::UnitMove>()->setTile(5, 6);

		kitten::K_GameObject* u15 = UnitSpawn::getInstance()->spawnUnitObject(18);//summoning witch
		u15->getComponent<unit::UnitMove>()->setTile(6, 1);

//		kitten::K_GameObject* u16 = UnitSpawn::getInstance()->spawnUnitObject(20);//poison pool
//		u16->getComponent<unit::UnitMove>()->setTile(6, 2);

//		kitten::K_GameObject* u17 = UnitSpawn::getInstance()->spawnUnitObject(21);//evil fiend
//		u17->getComponent<unit::UnitMove>()->setTile(6, 3);

//		kitten::K_GameObject* u18 = UnitSpawn::getInstance()->spawnUnitObject(22);//horror lord
//		u18->getComponent<unit::UnitMove>()->setTile(5, 4);

//		kitten::K_GameObject* u19 = UnitSpawn::getInstance()->spawnUnitObject(23);//wraith
//		u19->getComponent<unit::UnitMove>()->setTile(7, 1);

//		kitten::K_GameObject* u192 = UnitSpawn::getInstance()->spawnUnitObject(23);//wraith
//		u192->getComponent<unit::UnitMove>()->setTile(3, 4);

//		kitten::K_GameObject* u193 = UnitSpawn::getInstance()->spawnUnitObject(23);//wraith
//		u193->getComponent<unit::UnitMove>()->setTile(2, 4);

//		kitten::K_GameObject* u20 = UnitSpawn::getInstance()->spawnUnitObject(24);//gorefiend
//		u20->getComponent<unit::UnitMove>()->setTile(2, 4);

		kitten::K_GameObject* u21 = UnitSpawn::getInstance()->spawnUnitObject(29);//eldritch lord
		u21->getComponent<unit::UnitMove>()->setTile(4, 7);

		//test unit 
		//unit::Unit* u = u1->getComponent<unit::Unit>();
		//UnitMonitor::getInstanceSafe()->printUnit(u);

		//Test Initiative Tracker
		//kitten::K_GameObject* u2 = UnitSpawn::getInstance()->spawnUnitObject(parser->getUnit("Engineer.txt"));
		//kitten::K_GameObject* u3 = UnitSpawn::getInstance()->spawnUnitObject(parser->getUnit("Duelist.txt"));

		UnitMonitor::getInstanceSafe()->printIT();
		//InitiativeTracker::getInstance()->gameTurnStart();
		//UnitMonitor::getInstanceSafe()->printIT();
		//InitiativeTracker::getInstance()->removeUnit(u2);
		//UnitMonitor::getInstanceSafe()->printIT();

		//test sprite group
		/*
		kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();
		kitten::K_ComponentManager* cm = kitten::K_ComponentManager::getInstance();

		kitten::K_GameObject* g = gm->createNewGameObject();
		g->getTransform().place(2.0, 0.5, 2.0);

		kitten::K_GameObject* go = gm->createNewGameObject();

		go->addComponent(cm->createComponent("SpriteRenderable"));

		sprites::SpriteAnimator* animator = static_cast<sprites::SpriteAnimator*>(cm->createComponent("SpriteAnimator"));
		animator->setSpriteSheet("wizard");
		go->addComponent(animator);

		go->getTransform().setIgnoreParent(false);
		go->getTransform().setParent(&g->getTransform());

		go->getTransform().place(0, 0, 0);
		go->getTransform().rotateAbsolute(glm::vec3(45, 0, 0));
		go->getTransform().scaleAbsolute(10, 10, 10);*/

		//test cursor
		//kitten::K_GameObjectManager::getInstance()->createNewGameObject("cursor.json");

		//test ui clickable
		/*
		//initialize
		kitten::K_GameObjectManager* goMan = kitten::K_GameObjectManager::getInstance();
		//frame object
		kitten::K_GameObject* fgo = goMan->createNewGameObject();

		//component
		kitten::K_ComponentManager* comMan = kitten::K_ComponentManager::getInstance();
		//add frame
		kitten::K_Component* frame = comMan->createComponent("Frame");
		fgo->addComponent(frame);
		kitten::K_Component* f = comMan->createComponent("ClickableFrame");
		fgo->addComponent(f);
		kitten::K_Component* c = comMan->createComponent("TrackerBlockClickable");
		fgo->addComponent(c);

		fgo->getTransform().scale2D(10,10);
		fgo->getTransform().place2D(500,500);*/

		//test action button
		//kitten::K_GameObjectManager::getInstance()->createNewGameObject("unit_action_button.json");
	}
}
