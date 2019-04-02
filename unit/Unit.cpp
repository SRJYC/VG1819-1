#include "Unit.h"
#include "unit/unitComponent/UnitMove.h"
#include "kitten/K_GameObject.h"
#include "unitInteraction/UnitInteractionManager.h"
#include "AI/Extract/Behavior.h"
#include "board/tile/gameMode/Capture/CaptureItemController.h"

#include "_Project\UniversalPfx.h"

#include <iostream>

// Networking
#include "networking\ClientGame.h"
//@Rock

namespace unit
{
	Unit::Unit() : m_healthBarState(none), m_healthBar(nullptr), m_unitSelect(nullptr)
	{
		m_itemGO = nullptr;

		m_commander = nullptr;
		m_turn = nullptr;

		m_statusContainer = new StatusContainer(this);

		m_cdRecorder = new CooldownRecorder();

		m_castTimer = new CastTimer(this);

		setJoinAD();
	}

	Unit::Unit(const unit::Unit* source) : unit::Unit(*source)
	{
		m_healthBarState = none;
		m_healthBar = nullptr;
		m_unitSelect = nullptr;
		m_commander = nullptr;
		m_turn = nullptr;

		m_statusContainer = new StatusContainer(this);

		m_cdRecorder = new CooldownRecorder();
		m_ADList.clear();
		m_castTimer = new CastTimer(this);

		setJoinAD();
	}

	Unit::~Unit()
	{
		delete m_statusContainer;
		delete m_cdRecorder;
		delete m_castTimer;

		for (auto it : m_ADList)//delete ability description
		{
			delete it;
		}

		if (isCommander())
		{
			delete m_commander;
		}
	}

	void Unit::start()
	{
		m_healthBar = m_attachedObject->getComponent<UnitHealthBar>();

		//bool flag = false;
		for (std::string it : m_tags)//strucutre can't join to another unit
		{
			m_tagCheckMap[it] = true;
		}
		//m_isStructure = flag;*/

		//check if unit has auto cast ability
		for (auto ad : m_ADList)
		{
			auto found = ad->m_intValue.find(AUTO_CAST);
			if (found != ad->m_intValue.end())
			{
				if(found->second)//auto cast property exist and it's 1
					setAutoAbility(ad->m_stringValue[ABILITY_NAME]);
			}
		}
	}

	void Unit::setAutoAbility(const std::string & p_name)
	{
		m_autoCast = true;
		m_autoAbility = p_name;
	}

	//status
	/*
	void Unit::addStatus(ability::Status *p_newStatus)
	{
		m_statusContainer->addStatus(p_newStatus);
	}

	bool Unit::removeStatus(ability::Status * p_oldStatus)
	{
		return m_statusContainer->removeStatus(p_oldStatus);
	}

	ability::Status* Unit::getStatus(const std::string & p_name)
	{
		return m_statusContainer->getStatus(p_name);
	}*/
	

	//status
	StatusContainer * Unit::getStatusContainer()
	{
		return m_statusContainer;
	}

	void Unit::triggerTP(ability::TimePointEvent::TPEventType p_tp, ability::TimePointEvent* p_event)
	{
		if (p_event == nullptr)
		{
			p_event = new ability::TimePointEvent(p_tp);
		}
		m_statusContainer->triggerTP(p_tp,p_event);

		//special cases
		kitten::K_GameObject* tileGO = getTile();
		switch (p_tp)
		{
		case ability::TimePointEvent::Turn_Start:
			if (isCommander())
				m_commander->resetPower(m_clientId);
		case ability::TimePointEvent::Turn_End:
		case ability::TimePointEvent::New_Tile:
			//check item
			if (tileGO != nullptr)
			{
				TileInfo* info = tileGO->getComponent<TileInfo>();
				if (info->hasItem() && !this->hasItem())
				{//pick up item 
					kitten::K_GameObject* item = info->getItem();
					info->removeItem();

					//item is held by unit
					item->getComponent<CaptureItemController>()->setParent(this);
				}
			}
		case ability::TimePointEvent::Leave_Tile:
			if(tileGO != nullptr)
				tileGO->getComponent<TileInfo>()->effect(p_tp, this);
			break;
		default:
			break;
		}
		delete p_event;
	}

	//join action
	void Unit::setJoinAD()
	{
		m_joinAD.m_stringValue["name"] = ACTION_JOIN;
		//m_joinAD.m_intValue["target"] = 1;
		m_joinAD.m_intValue["need_unit"] = 1;
		m_joinAD.m_intValue["min_range"] = 1;
		m_joinAD.m_intValue["max_range"] = 1;
	}

	void Unit::join()
	{
		if (isCommander() || checkTag(STRUCTURE) || m_attributes[UNIT_LV] >= 3)//commander and structure can't join to another unit
			return;

		UnitInteractionManager::getInstance()->request(this, &m_joinAD);
	}

	void Unit::levelup()
	{
		//reset tile
		m_attachedObject->getComponent<UnitMove>()->reset();

		std::cout << m_name << " is LV "<< m_attributes[UNIT_LV] << std::endl;
		if (m_attributes[UNIT_LV] > 0 && m_attributes[UNIT_LV] < 3)
		{
			m_attributes[UNIT_LV]++;
			ability::TimePointEvent* t = new ability::TimePointEvent(ability::TimePointEvent::Level_Up);
			t->putInt(UNIT_LV, m_attributes[UNIT_LV]);
			triggerTP(ability::TimePointEvent::Level_Up, t);

			kitten::Event* eventData = new kitten::Event(kitten::Event::Update_Card_Context_Attrib);
			eventData->putGameObj(UNIT_GO_KEY, m_attachedObject);
			kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Update_Card_Context_Attrib, eventData);
		}
	}

	//commander
	void Unit::addCommander(Commander* p_c)
	{
		m_commander = p_c;
		m_commander->init(this);
	}

	bool Unit::isCommander() const
	{
		return m_commander != nullptr;
	}

//	void Unit::manipulateTile()
//	{
//		if(isCommander())
//			m_commander->manipulateTile();
//	}

	bool Unit::checkTag(const std::string & p_tag)
	{
		return m_tagCheckMap[p_tag];
	}

//	bool Unit::isStructure()
//	{
//		return m_isStructure;
//	}

/*	void Unit::summonUnit(int p_id)
	{
		if (isCommander())
			m_commander->spawnUnit(p_id);
	}*/

	//check turn end in update, so it will not stick with next unit after action
	void Unit::update()
	{
		if (isTurn())
		{
			m_turn->checkTurn();
		}
	}

	//turn
	void Unit::turnStart(UnitTurn * p_t)
	{
		assert(m_turn == nullptr);
		m_turn = p_t;

		//if the unit's movement is greater than 0, then it can move this turn
		int mv = m_attributes["mv"];
		if (mv <= 0)
			m_turn->move = false;
		else
			m_turn->move = true;

		m_cdRecorder->reduceCD();//reduce cd at start of turn

		int i = m_castTimer->changeTimer();//reduce ct at start of turn
		if (i == 0)
		{
			m_castTimer->cast();//cast ability if timer reaches 0
			if (m_turn == nullptr)
				return;
		}
		else if (m_castTimer->isCasting())
		{
			playerSkipTurn(false);//if it still cast, it skips turn
			return;
		}
		else if (m_ADList.size() == 0)//doesn't have abilities
		{
			m_turn->act = false;
		}
		else
		{
			m_turn->act = true;
		}

		//if has auto cast ability, use it
		if (m_autoCast)
		{
			useAbility(m_autoAbility);
		}
		kitten::Event* eventData = new kitten::Event(kitten::Event::Next_Units_Turn_Start);
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Next_Units_Turn_Start, eventData);
	}

	bool Unit::canMove()
	{
		if (m_attributes[UNIT_BASE_MV] <= 0)//unit can not move, like structure
			return false;
		else if (m_turn != nullptr)//this is unit's turn and check if it can move
			return m_turn->move;
		return true;
	}

	bool Unit::canAct() const
	{
		assert(m_turn != nullptr);
		return m_turn->act;
	}

	void Unit::moveDone()
	{
		if (m_turn != nullptr)//move by action
		{
			bool moveDone = false;
			if (!m_path.empty())//has next tile in path
			{
				auto it = m_path.erase(m_path.begin());//remove first tile
				if (it != m_path.end())
				{
					move(*it);//move to next
				}
				else
					moveDone = true;
			}
			else
				moveDone = true;

			if (moveDone && !m_lateDestroy)
			{
				m_turn->move = false;

				kitten::EventManager::getInstance()->queueEvent(kitten::Event::Action_Complete, new kitten::Event(kitten::Event::Action_Complete));
				std::cout << "movement Complete\n";
			}
		}

		if (m_lateDestroy)
			simpleDestroy();
	}

	void Unit::actDone()
	{
		assert(m_turn != nullptr);
		m_turn->act = false;
	}

	bool Unit::isTurn() const
	{
		//if m_turn is nullptr, it means its not this unit turn
		return m_turn != nullptr;
	}

	void Unit::turnEnd()
	{
		assert(m_turn != nullptr);
		m_turn = nullptr;
	}

	void Unit::playerSkipTurn(bool p_sendPacket)
	{
		assert(m_turn != nullptr);

		networking::ClientGame* client = networking::ClientGame::getInstance();
		if (client != nullptr && p_sendPacket)
		{			
			if (!client->isServerCalling())
			{
				client->sendSkipTurnPacket(this);
			}
			else
			{
				client->setServerCalling(false);
			}
		}

		m_turn->turnEnd();
	}

	kitten::K_GameObject * Unit::getTile()
	{
		return m_attachedObject->getComponent<unit::UnitMove>()->getTile();
	}

	//move
	void Unit::move()//move by instruction
	{
		if (!canMove())
			return;

		unit::UnitMove* moveComponet = m_attachedObject->getComponent<unit::UnitMove>();
		moveComponet->attempToMove();
	}

	void Unit::move(int p_min, int p_max)//move by ability with range
	{
		if (!canMove())
			return;

		unit::UnitMove* moveComponet = m_attachedObject->getComponent<unit::UnitMove>();
		moveComponet->attempToMove(p_min, p_max);
	}

	void Unit::move(kitten::K_GameObject * p_tile)//move by ability with fixed target
	{
		if (!canMove())
			return;

		unit::UnitMove* moveComponet = m_attachedObject->getComponent<unit::UnitMove>();
		moveComponet->move(p_tile);
	}

	void Unit::move(std::vector<kitten::K_GameObject*> p_path)
	{
		m_path = p_path;
		move(m_path[0]);
	}

	void Unit::moveAnime(kitten::K_GameObject * p_tile)
	{
		//if (!canMove())
		//	return;

		unit::UnitMove* moveComponet = m_attachedObject->getComponent<unit::UnitMove>();
		moveComponet->dontSetTileAfterMove();
		moveComponet->move(p_tile);
	}

	//ability
	int Unit::useAbility(const std::string& p_abilityName, bool p_autoClick)
	{
		//if (!canAct())
		//	return -1;

		AbilityDescription* ad;
		auto found = m_ADMap.find(p_abilityName);
		if (found != m_ADMap.end())
		{
			std::cout << "use ability: " << p_abilityName << std::endl;
			ad = found->second;
		}
		else
		{
			std::cout << "Ability: " << p_abilityName << " isn't found" << std::endl;
			return -2;//doesn't have ability
		}

		/*lv check and cd check is move to action button store
		//non commander unit will check unit's lv
		if (m_attributes[UNIT_LV] < ad->m_intValue[UNIT_LV] && !isCommander())
		{
			std::cout << p_abilityName << "require lv (" << ad->m_intValue["lv"] << ") " << std::endl;
			std::cout << m_name << " is lv (" << m_attributes["lv"] << ")" << std::endl;
			return -3;//means unit can not use this ability
		}

		//check cd
		if (m_cdRecorder->checkCD(ad) != 0)
		{//it's in cool down
			std::cout << p_abilityName << " is in Cooldown. CD remain:"<<m_cdRecorder->checkCD(ad) << std::endl;
			return -4;
		}*/

		m_cdRecorder->addCD(ad);
		UnitInteractionManager::getInstance()->request(this, ad, p_autoClick);

		return 0;
	}

	void Unit::cancelAbility(AbilityDescription* p_ad)
	{
		m_cdRecorder->cancelCD(p_ad);
	}

	void Unit::setCast(AbilityDescription * p_ad, ability::AbilityInfoPackage * p_pack)
	{
		std::string name = p_ad->m_stringValue["name"];
		int time = p_ad->m_intValue[UNIT_CT];
		m_castTimer->set(name,p_pack,time);
		playerSkipTurn(false);
	}

	void Unit::cancelCast()
	{
		m_castTimer->cancelCast();
	}

	int Unit::checkCD(const std::string & p_abilityName)
	{
		AbilityDescription* ad;
		auto found = m_ADMap.find(p_abilityName);

		assert(found != m_ADMap.end());//ability not found

		ad = found->second;

		return m_cdRecorder->checkCD(ad);
	}

	//destroy
	int Unit::destroyedByDamage()
	{
		//TO DO: send destroyed even
		m_healthBarState = destroying;
		
		if (m_healthBar == nullptr)
		{
			m_healthBar = m_attachedObject->getComponent<UnitHealthBar>();
			assert(m_healthBar != nullptr);
		}

		m_healthBar->getForegroundBarLerpController()->addScaleLerpFinishedCallback(this);

		if (m_unitSelect == nullptr)
		{
			m_unitSelect = m_attachedObject->getComponent<UnitSelect>();
			assert(m_unitSelect != nullptr);
		}

		m_unitSelect->disableInteraction(true);

		return 0;
	}

	int Unit::destroyedByJoin()
	{
		m_lateDestroy = true;
		return 0;
	}

	void Unit::simpleDestroy()
	{
		//remove from tile
		auto info = getTile()->getComponent<TileInfo>();
		if (info != nullptr)
		{
			info->removeUnit();

			if (this->hasItem())//unit has item
			{
				//drop item on the tile if the tile doesn't have item
				if (!info->hasItem())
				{
					//item is held by tile
					m_itemGO->getComponent<CaptureItemController>()->setParent(info);
					//remove item from unit
					this->removeItem();
				}
				else//there can only be one item on the tile, so remove this item
				{
					m_itemGO->getComponent<CaptureItemController>()->remove();
					this->removeItem();
				}
			}
			
		}

		//trigger unit destroy event
		triggerTP(ability::TimePointEvent::Unit_Destroy);

		//remove from intiative tracker
		InitiativeTracker::getInstance()->removeUnit(m_attachedObject);
	}

	void Unit::destroy()
	{	
		const glm::vec3& pos = getTransform().getTranslation();
		auto pfxInstance = UniversalPfx::getInstance();
		assert(pfxInstance != nullptr);
		pfxInstance->playEffect(UNIT_DEATH_EFFECT_NAME, pos);
		

		std::cout << m_name << " is destroyed! " << std::endl;
		
		simpleDestroy();

		// Commander Death Victory/Defeat Condition
		if (isCommander())
		{
			if (networking::ClientGame::isNetworkValid())
			{
				networking::ClientGame* client = networking::ClientGame::getInstance();

				kitten::Event* eventData = new kitten::Event(kitten::Event::Network_End_Game);
				if (m_clientId == client->getClientId())
				{
					eventData->putInt(GAME_END_RESULT, HOST_COMMANDER_DIED);
				}
				else
				{
					eventData->putInt(GAME_END_RESULT, CLIENT_COMMANDER_DIED);
				}
				
				kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Network_End_Game, eventData);
			}			
		}
	}

	//item
	const bool Unit::hasItem() const
	{
		return m_itemGO != nullptr;
	}

	void Unit::addItem(kitten::K_GameObject* p_item)
	{
		m_itemGO = p_item;
	}

	void Unit::removeItem()
	{
		m_itemGO = nullptr;
	}

	kitten::K_GameObject * Unit::getItem() const
	{
		return m_itemGO;
	}

	//hp bar
	void Unit::onScaleLerpFinished(kitten::K_GameObject* p_obj) //Called when healthbar is done animating
	{
		switch (m_healthBarState)
		{
		case destroying:
			destroy();
			break;
		}

		m_healthBarState = none;
	}
}
