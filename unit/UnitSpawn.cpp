#pragma once
#include "UnitSpawn.h"
#include "kitten/K_GameObjectManager.h"
#include "kitten/K_ComponentManager.h"
#include "unit/InitiativeTracker/InitiativeTracker.h"
#include "unit/unitComponent/UnitClickable.h"
#include "unit/unitComponent/UnitMove.h"
#include "unit/unitComponent/UnitSelect.h"
//board clickable
#include "board/clickable/PrintWhenClicked.h"

#include "puppy/Text/TextBox.h"

#include "kitten/sprites/SpriteAnimator.h"
#include "kitten/sprites/SpriteGroup.h"
//Rock

#include "kibble/databank/databank.hpp"
#include "kibble\GameObjectDataParser.hpp"
#include "networking\ClientGame.h"

namespace unit
{
	UnitSpawn* UnitSpawn::sm_instance = nullptr;

	UnitSpawn::UnitSpawn()
	{
		m_storage = new ActionButtonStore();
		/*
		puppy::TextBox* textBox = static_cast<puppy::TextBox*>(kitten::K_ComponentManager::getInstance()->createComponent("TextBoxAbilities"));
		textBox->setColor(1, 1, 1);
		textBox->setText("");

		kitten::K_Component* select = kitten::K_ComponentManager::getInstance()->createComponent("SelectAbility");

		m_textBoxGO = kitten::K_GameObjectManager::getInstance()->createNewGameObject();
		m_textBoxGO->addComponent(textBox);
		m_textBoxGO->getTransform().place2D(1000, 600);

		m_textBoxGO->addComponent(select);

		kitten::K_GameObjectManager::getInstance()->flagGameObjectToSurvive(m_textBoxGO);*/
	}

	UnitSpawn::~UnitSpawn()
	{
		delete m_storage;
	}

	/*
	kitten::K_GameObject * UnitSpawn::spawnUnitObject(Unit * p_unitData)
	{
		//create unit 
		Unit* unit = p_unitData;

		//get component manager
		kitten::K_ComponentManager* cm = kitten::K_ComponentManager::getInstance();

		//unit object
		kitten::K_GameObject* unitObject = kitten::K_GameObjectManager::getInstance()->createNewGameObject();
		unitObject->addComponent(unit);

		
		//create unit graphic
		UnitGraphic* unitG = static_cast<UnitGraphic*>(cm->createComponent("UnitGraphic"));
		unitObject->addComponent(unitG);

		//create unit move
		kitten::K_Component* uMove = cm->createComponent("UnitMove");
		unitObject->addComponent(uMove);

		//create click box
		kitten::K_Component* uBox = createClickableBox(p_unitData->m_size);
		unitObject->addComponent(uBox);

		//create clickable
		unit::UnitClickable* uClick = static_cast<unit::UnitClickable*>(cm->createComponent("UnitClickable"));
		uClick->setTextBox(m_textBoxGO);
		unitObject->addComponent(uClick);
		
		//hard coded, should be datadriven
		//unitObject->getTransform().move(0.5f, 1.0f, 0.0f);
		//rotate to face camera
		unitObject->getTransform().rotateRelative(glm::vec3(45, 0, 0));

		//add object to Initiative Tracker
		unit::InitiativeTracker::getInstance()->addUnit(unitObject);

		return unitObject;
	}*/

	kitten::K_GameObject* UnitSpawn::spawnUnitObject(const int& p_unitIdentifier) {
		return spawnUnitObjectInternally(kibble::getUnitInstanceFromId(p_unitIdentifier));
	}

	kitten::K_GameObject* UnitSpawn::spawnUnitObject(unit::Unit* p_unit) {
		// This makes a copy of the unit and attaches it. 
		unit::Unit* unit = kibble::getUnitInstanceFromId(p_unit->m_kibbleID);

		// Add all differences to take into account are here. ---------------------------
		unit->m_attributes = p_unit->m_attributes;

		return spawnUnitObjectInternally(unit);
	}

	kitten::K_GameObject* UnitSpawn::spawnUnitObjectInternally(unit::Unit* p_unit) {

		/*
		if (!kibble::checkIfComponentDriven(p_unitIdentifier))
			return spawnUnitObject(kibble::getUnitInstanceFromId(p_unitIdentifier));
			*/

			//get component manager
		kitten::K_ComponentManager* cm = kitten::K_ComponentManager::getInstance();

		//unit object
		kitten::K_GameObject* unitObject = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UnitBase.txt");
		kibble::attachCustomComponentsToGameObject(p_unit, unitObject);
		unitObject->addComponent(p_unit);

		unitObject->getComponent<UnitSelect>()->setActionButtonStore(m_storage);

		//create clickable
		/*
		unit::UnitClickable* uClick = static_cast<unit::UnitClickable*>(cm->createComponent("UnitClickable"));
		uClick->setTextBox(m_textBoxGO);
		unitObject->addComponent(uClick);*/

		/*sprite renderable is replaced by sprite group
		if (unitObject->getComponent<kitten::SpriteRenderable>() != nullptr) {
			// disable unit Graphic
			unitObject->getComponent<UnitGraphic>()->setEnabled(false);
		}*/

		if (unitObject->getComponent<sprites::SpriteGroup>() != nullptr) {
			// disable unit Graphic
			unitObject->getComponent<UnitGraphic>()->setEnabled(false);
		}

		// Set client Id to unit
		if (networking::ClientGame::getInstance())
		{
			unit::Unit* unit = unitObject->getComponent<unit::Unit>();
			unit->m_clientId = networking::ClientGame::getInstance()->getClientId();
			networking::ClientGame::getInstance()->addUnitGameObject(unitObject);
		}

		//add object to Initiative Tracker
		unit::InitiativeTracker::getInstance()->addUnit(unitObject);

		return unitObject;
	}

	/*
	Unit* UnitSpawn::spawnUnitFromData(UnitData * p_unitData)
	{
		Unit* unit = new Unit();

		//TO DO: an ID system instead of random words
		unit->m_ID = "testUnit01";//hard coded for now

		//copy tag, name and attributes
		unit->m_tags = p_unitData->m_tags;
		unit->m_name = p_unitData->m_name;

		int hp = p_unitData->m_HP;
		int in = p_unitData->m_IN;
		int mv = p_unitData->m_MV;
		int cost = p_unitData->m_Cost;

		unit->m_attributes[UNIT_HP] = hp;
		unit->m_attributes[UNIT_MAX_HP] = hp;
		unit->m_attributes[UNIT_IN] = in;
		unit->m_attributes[UNIT_BASE_IN] = in;
		unit->m_attributes[UNIT_MV] = mv;
		unit->m_attributes[UNIT_BASE_MV] = mv;
		unit->m_attributes[UNIT_COST] = cost;
		unit->m_attributes[UNIT_BASE_COST] = cost;
		//set lv to 1
		unit->m_attributes[UNIT_LV] = 1;

		unit->m_size = p_unitData->m_size;

		//readAD
		for (auto it : p_unitData->m_ad)
		{
			//get copy
			AbilityDescription * ad = new AbilityDescription();
			ad->m_intValue = it->m_intValue;
			ad->m_stringValue = it->m_stringValue;
			unit->m_ADList[it->m_stringValue["name"]] = ad;
		}

		//readSD
		for (auto it : p_unitData->m_sd)
		{
			readSD(it)->attach(unit);
		}

		//doesn't belong to any client
		unit->m_clientId = -1;

		//check if it's commander
		//check every tag
		for (int i = 0; i < p_unitData->m_tags.size(); i++)
		{
			if (p_unitData->m_tags[i] == "Commander")
			{
				spawnCommander(unit, p_unitData);
				break;
			}
		}

		return unit;
	}

	void UnitSpawn::spawnCommander(Unit* p_u, UnitData * p_unitData)
	{
		//change lv to -1 since it doesn't apply to commander
		p_u->m_attributes[UNIT_LV] = -1;

		p_u->m_ID = "testCommander01";

		Commander * c = new Commander();
		p_u->addCommander(c);

		// Had to comment this out for testing Commander's ManipulateTile ability, using testDummy.txt
		// Threw errors every other time
		//commander->m_porPath = p_unitData->m_porPath;
	}*/

	kitten::K_Component * UnitSpawn::createClickableBox(UnitSize p_size)
	{
		switch (p_size)
		{
		case unit::point:
			return kitten::K_ComponentManager::getInstance()->createComponent("ClickableBoxForPointUnit");
			break;
		case unit::cube:
			return kitten::K_ComponentManager::getInstance()->createComponent("ClickableBoxForCubeUnit");
			break;
		default:
			return nullptr;
		}
		return nullptr;
	}

	/*
	ability::Status* UnitSpawn::readSD(unit::StatusDescription* p_sd)
	{
		std::string name = p_sd->m_stringValue["name"];

		//find a empty copy of the status
		ability::Status* s = ability::StatusManager::getInstance()->findStatus(name);

		if (p_sd->m_stringValue.find("description") != p_sd->m_stringValue.end())
		{
			s->changeDescription(p_sd->m_stringValue["description"]);
		}

		if (p_sd->m_intValue.find("time_point") != p_sd->m_intValue.end())
		{
			int i = p_sd->m_intValue["time_point"];
			s->addTimePoint(static_cast<ability::TimePointEvent::TPEventType>(i));
		}

		//for lv status
		if (p_sd->m_intValue.find(UNIT_LV) != p_sd->m_intValue.end())
		{
			s->changeLV(p_sd->m_intValue[UNIT_LV]);
			//hp
			if (p_sd->m_intValue.find(UNIT_HP) != p_sd->m_intValue.end())
			{
				int hp = p_sd->m_intValue[UNIT_HP];
				s->addAttributeChange(UNIT_HP, hp);
				s->addAttributeChange(UNIT_MAX_HP, hp);
			}
			//in
			if (p_sd->m_intValue.find(UNIT_IN) != p_sd->m_intValue.end())
			{
				int in = p_sd->m_intValue[UNIT_IN];
				s->addAttributeChange(UNIT_IN, in);
				s->addAttributeChange(UNIT_BASE_IN, in);
			}
			//mv
			if (p_sd->m_intValue.find(UNIT_MV) != p_sd->m_intValue.end())
			{
				int mv = p_sd->m_intValue[UNIT_MV];
				s->addAttributeChange(UNIT_MV, mv);
				s->addAttributeChange(UNIT_BASE_MV, mv);
			}
		}

		return s;
	}*/
}
