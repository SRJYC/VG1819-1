#include "UnitInteractionManager.h"
#include "unitInteraction/TileGetter.h"
#include "CounterGetter.h"
#include <iostream>
#include <sstream>
#include "networking\ClientGame.h"
#include "components\DragNDrop\SpawnUnitOnDrop.h"

UnitInteractionManager* UnitInteractionManager::sm_instance = nullptr;

void UnitInteractionManager::request(unit::Unit* p_unit, unit::AbilityDescription * p_ad, bool p_autoClick)
{
	if (m_busy)
		return;

	m_busy = true;

	m_unit = p_unit;
	m_ad = p_ad;
	m_abilityName = m_ad->m_stringValue[ABILITY_NAME];

	//debug message 
	//std::cout << "UnitInteractionManager Receive: " << p_unit->m_name << " :: " << m_abilityName << std::endl;

	//create package
	if (m_package != nullptr)
		m_package = nullptr;
	m_package = new ability::AbilityInfoPackage();

	//set source
	m_package->m_source = p_unit;

	//copy all values to package
	addPropertyFromADToPack();

	//check if unit is necessary, default is false
	m_needunit = false;
	if(m_ad->m_intValue.find("need_unit")!= m_ad->m_intValue.end())
		m_needunit = m_ad->m_intValue["need_unit"];

	//check if needs counter
	m_getCounter = (p_ad->m_stringValue.find(COUNTER_SELECT) == p_ad->m_stringValue.end());

	/*
	if (p_ad->m_stringValue.find(COUNTER_NAME) != p_ad->m_stringValue.end())//need counter
	{
		m_getCounter = false;
	}*/

	//needs tile clicked
	m_getTile = false;

	//set client id
	m_package->m_sourceClientId = networking::ClientGame::getClientId();

	//set auto click
	if (p_autoClick)//this request is auto click
	{
		m_autoClick = true;
	}
	else
	{
		auto found = m_ad->m_intValue.find(AUTO_CLICK);
		if (found != m_ad->m_intValue.end())
		{//auto click property exist then m_autoClick is same as it
			m_autoClick = found->second;
		}
	}

	send();
}

void UnitInteractionManager::setCounter(const std::string& p_counter, int p_n)
{
	m_package->m_intValue[p_counter] = p_n;
	m_getCounter = true;

	send();
}

void UnitInteractionManager::setTarget(std::vector<kitten::K_GameObject*> p_tileList, std::vector<unit::Unit*> p_unitList)
{
	m_package->m_targets = p_unitList;
	m_package->m_targetTilesGO = p_tileList;
	m_getTile = true;

	send();
}

UnitInteractionManager::UnitInteractionManager()
{
	m_package = nullptr;
	m_ad = nullptr;
	m_tileGetter = new TileGetter();
	m_counterGetter = new CounterGetter();

	m_busy = false;
}

UnitInteractionManager::~UnitInteractionManager()
{
	if (m_package != nullptr)
		delete m_package;

	delete m_tileGetter;
	delete m_counterGetter;
}

void UnitInteractionManager::cancel()
{
	if (!m_busy)
		return;

	m_busy = false;

	//delete package
	if (m_package != nullptr)
	{
		std::cout << "UnitInteractionManager Cancel Ability" << std::endl;
		if (m_ad->m_cardGOForUnitSummon != nullptr)
		{
			SpawnUnitOnDrop* onDrop = m_ad->m_cardGOForUnitSummon->getComponent<SpawnUnitOnDrop>();
			if (onDrop != nullptr)
			{
				onDrop->resetCard();
			}
		}
		m_package->m_source->cancelAbility(m_ad);
		delete m_package;
		m_package = nullptr;

		//m_counterGetter->cancel();
		//m_tileGetter->cancel();
	}

}

bool UnitInteractionManager::isBusy() const
{
	return m_busy;
}

void UnitInteractionManager::reset()
{
	//delete package
	if (m_package != nullptr)
	{
		delete m_package;
		m_package = nullptr;
	}

	//tile getter will refresh whenever needed so don't need to reset

	//reset counter getter
	m_counterGetter->reset();

	m_busy = false;
}

void UnitInteractionManager::send()
{
	if (!m_busy)
		return;

	if (!m_getCounter)
	{
		//ask player for counter
		m_counterGetter->requireCounter(m_ad, m_unit);
		return;
	}

	if (!m_getTile)
	{
		//ask player for targets
		m_tileGetter->requireTile(m_ad, m_unit, m_needunit, m_autoClick);
		return;
	}

	auto found = m_ad->m_intValue.find("ct");
	if (found == m_ad->m_intValue.end() || found->second <1)//no cast time
	{//directly use it
		ability::AbilityManager::getInstance()->useAbility(m_abilityName, m_package);
	}
	else//has cast time
	{//set cast ability
		m_unit->setCast(m_ad, m_package);
		if (networking::ClientGame::isNetworkValid())
		{
			networking::ClientGame::getInstance()->sendCastTimeAbilityPacket(m_ad, m_package);
		}
	}

	m_busy = false;

	m_package = nullptr;
	if (m_ad->m_stringValue[ABILITY_NAME] != ACTION_MOVE) {
		kitten::EventManager::getInstance()->queueEvent(kitten::Event::Action_Complete, new kitten::Event(kitten::Event::Action_Complete));
		std::cout << m_ad->m_stringValue[ABILITY_NAME] << " Complete\n";
	}
}

void UnitInteractionManager::addPropertyFromADToPack()
{
	for (auto it : m_ad->m_intValue)
	{
		m_package->m_intValue[it.first] = it.second;
	}

	for (auto it : m_ad->m_stringValue)
	{
		m_package->m_stringValue[it.first] = it.second;
	}

	m_package->m_cardGOForUnitSummon = m_ad->m_cardGOForUnitSummon;
}
