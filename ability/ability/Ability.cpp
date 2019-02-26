#include "Ability.h"
#include "unit/Unit.h"

#include "board/tile/TileInfo.h"

#include "_Project\ProjectileManager.h"

#include "unit/UnitSpawn.h"
#include "unit/unitComponent/UnitMove.h"
#include "networking\ClientGame.h"

#include "UI/HandFrame.h"

#include <iostream>

void ability::Ability::singleTargetDamage(AbilityInfoPackage* p_info, bool p_fireProjectile)
{
	//damage target by power
	if (checkTarget(p_info))
	{
		if (p_fireProjectile)
		{
			ProjectileManager::fireProjectile(m_name, p_info->m_source, p_info->m_targets[0], this, p_info);
		}
		else
		{
			singleTargetProjectileFinished(p_info);
		}
	}
	else
	{
		//delete package
		done(p_info);
	}
}

void ability::Ability::singleTargetProjectileFinished(AbilityInfoPackage* p_package)
{
	//trigger deal damage event
	triggerTPEvent(ability::TimePointEvent::Deal_Damage, p_package->m_source, p_package);

	//trigger receive damage event
	unit::Unit* target = p_package->m_targets[0];
	triggerTPEvent(ability::TimePointEvent::Receive_Damage, target, p_package);

	//so power will change to negative
	int power = -(p_package->m_intValue.find(UNIT_POWER)->second);

	damage(target, power);

	//delete package
	done(p_package);
}

void ability::Ability::multiTargetDamage(AbilityInfoPackage* p_info, bool p_fireProjectile)
{
	if (p_fireProjectile)
	{
		ProjectileManager::multiDamageFireProjectile(m_name, p_info->m_source, this, p_info);
	}
	else
	{
		multiTargetProjectileFinished(p_info);
	}
}

void ability::Ability::multiTargetProjectileFinished(AbilityInfoPackage* p_package)
{
	triggerTPEvent(ability::TimePointEvent::Deal_Damage, p_package->m_source, p_package);

	for (unit::Unit* u : p_package->m_targets)
	{
		//get copy of package
		AbilityInfoPackage* clonePackage = new AbilityInfoPackage(*p_package);

		//trigger receive damage
		triggerTPEvent(ability::TimePointEvent::Receive_Damage, u, clonePackage);

		int power = -(clonePackage->m_intValue.find(UNIT_POWER)->second);

		damage(u, power);

		//delete clone
		delete clonePackage;
	}

	//delete package
	done(p_package);
}

kitten::K_GameObject * ability::Ability::summonToken(AbilityInfoPackage* p_info, int p_unitIndex)
{
	kitten::K_GameObject* u = unit::UnitSpawn::getInstance()->spawnUnitObject(p_unitIndex);
	kitten::K_GameObject* tile = p_info->m_targetTilesGO[0];
	u->getComponent<unit::UnitMove>()->setTile(tile);

	if (networking::ClientGame::getInstance() != nullptr)
	{
		u->getComponent<unit::Unit>()->m_clientId = p_info->m_sourceClientId;
	}

	return u;
}

int ability::Ability::damage(unit::Unit* p_target, int power)
{
	AbilityNode* node1 = AbilityNodeManager::getInstance()->findNode(ChangeAttribute);

	//change hp
	node1->effect(p_target, UNIT_HP, power);

	return 0;
}

void ability::Ability::done(const AbilityInfoPackage* p_info)
{
	p_info->m_source->actDone();

	delete p_info;
}

void ability::Ability::changeCounter(unit::Unit * p_target, const std::string & p_name, int p_n)
{
	AbilityNode* node1 = AbilityNodeManager::getInstance()->findNode(ChangeAttribute);

	//change counter
	node1->effect(p_target, p_name, p_n);
}

bool ability::Ability::checkTarget(const AbilityInfoPackage * p_info)
{
	if (&(p_info->m_targets) != nullptr)
	{
		if (p_info->m_targets.size() > 0)
		{
			return true;
		}
	}
	return false;
}

bool ability::Ability::checkAlly(unit::Unit* p_source, unit::Unit* p_target)
{
	return p_source->m_clientId == p_target->m_clientId;
}

bool ability::Ability::checkTag(unit::Unit* p_u, const std::string& p_tag)
{
	for (std::string it : p_u->m_tags)
	{
		if (it == p_tag)
		{
			return true;
		}
	}
	return false;
}

void ability::Ability::getTarget(AbilityInfoPackage * p_info)
{
	std::vector<unit::Unit*> unitlist;
	std::vector<kitten::K_GameObject*> tileList = p_info->m_targetTilesGO;

	for (kitten::K_GameObject* t : tileList)
	{
		TileInfo* ti = t->getComponent<TileInfo>();

		if (ti->hasUnit())
		{
			kitten::K_GameObject* u = ti->getUnit();
			unitlist.push_back(u->getComponent<unit::Unit>());
		}
	}

	p_info->m_targets = unitlist;
}

std::vector<kitten::K_GameObject*> ability::Ability::getCardsInHand()
{
	std::list<userinterface::UIObject*> list = userinterface::HandFrame::getActiveInstance()->getInnerObjects();
	
	std::vector<kitten::K_GameObject*> objectList;

	for (auto it : list)
	{
		objectList.push_back( &(it->getGameObject()) );
	}
	return objectList;
}

void ability::Ability::triggerTPEvent(ability::TimePointEvent::TPEventType p_tp, unit::Unit * p_target, AbilityInfoPackage * p_info)
{
	unit::StatusContainer* sc = p_target->getStatusContainer();
	ability::TimePointEvent* t = new ability::TimePointEvent(p_tp);
	t->putPackage(INFO_PACKAGE_KEY, p_info);
	sc->triggerTP(p_tp, t);
}

void ability::Ability::addStatusInfo(Status * p_st, AbilityInfoPackage* p_info)
{
	auto it = p_info->m_stringValue.find(STATUS_NAME);
	if (it != p_info->m_stringValue.end())
	{
		std::string name = it->second;
		p_st->changeName(name);
	}

	it = p_info->m_stringValue.find(STATUS_DESCRIPTION);
	if (it != p_info->m_stringValue.end())
	{
		std::string des = it->second;
		p_st->changeDescription(des);
	}

	p_st->m_source = m_name;
}

void ability::Ability::drawCard(int p_id, int p_num)
{
	kitten::Event *e = new kitten::Event(kitten::Event::EventType::Draw_Card);
	e->putInt(PLAYER_ID, p_id);
	e->putInt(CARD_COUNT, p_num);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::EventType::Draw_Card, e);
}
