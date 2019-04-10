#include "Status.h"
#include "unit/Unit.h"
#include "unit\unitComponent\UnitStatusIcons.h"
#include <iostream>

namespace ability
{
	int Status::effect()
	{
		return -1;
	}
	int Status::effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent * p_event)
	{
		return -1;
	}

	void Status::registerTPEvent()
	{
		for (auto pair : m_TPList)
		{
			unit::StatusContainer* sc = m_unit->getStatusContainer();
			TimePointEvent::TPEventType type = pair.first;
			sc->registerTP(type,this);
		}
	}

	void Status::print()
	{
		std::cout << "Name: "<< m_name << std::endl;
		std::cout << "ID: " << m_Id << std::endl;
		std::cout << "Description: " << m_description << std::endl;
		std::cout << "Attached Unit: " << m_unit->m_name << std::endl;
		
		std::cout << "Counter: " << std::endl;
		for (auto it = m_intValue.begin(); it != m_intValue.end(); it++)
		{
			std::cout << "\t" << it->first << " : " << it->second << std::endl;
		}
		std::cout << "attributeChange: " << std::endl;
		for (auto it = m_attributeChange.begin(); it != m_attributeChange.end(); it++)
		{
			std::cout << "\t" << it->first << " : " << it->second << std::endl;
		}
	}

	void Status::notifyUnitDestroy(unit::Unit * p_u)
	{
		tryRemoveCaster(p_u);
	}

	void Status::tryRemoveCaster(unit::Unit * p_u)
	{
		if (p_u == m_caster)
		{
			m_caster = nullptr;
		}
	}

	Status::Status()
		:m_caster(nullptr),
		m_unit(nullptr)
	{
	}

	Status::~Status()
	{
		//effectEnd();
	}

	void Status::changeName(const std::string & p_msg)
	{
		m_name = p_msg;
	}

	void Status::changeLV(int p_lv)
	{
		m_LV = p_lv;
	}

	void Status::changeDescription(const std::string & p_msg)
	{
		m_description = p_msg;
	}

	/*
	void Status::setEffectedAD(const std::string & p_msg)
	{
		m_effectedAD = p_msg;
	}*/

	void Status::addCounter(const std::string & p_key, int p_value)
	{
		if (m_intValue.find(p_key) == m_intValue.end())
		{
			m_intValue.insert(std::make_pair(p_key, p_value));
		}
		else
		{
			m_intValue.at(p_key) = p_value;
		}
	}

	void Status::addAttributeChange(const std::string & p_key, int p_value)
	{
		if (p_value > 0)
		{
			m_statusType = StatusType::Stat_Buff;
		}
		else
		{
			m_statusType = StatusType::Stat_Debuff;
		}

		if (m_attributeChange.find(p_key) == m_attributeChange.end())
		{
			m_attributeChange.insert(std::make_pair(p_key, p_value));
		}
		else
		{
			m_attributeChange.at(p_key) = p_value;
		}
	}

	void Status::addTimePoint(const TimePointEvent::TPEventType& p_value, int p_priority)
	{
		m_TPList[p_value] = p_priority;
	}

	void Status::setCaster(unit::Unit * p_u)
	{
		m_caster = p_u;
		p_u->registerDestroy(this);
	}

	void Status::endEffectAt(const TimePointEvent::TPEventType& p_value)
	{
		m_endEffectEvent = p_value;

		addTimePoint(m_endEffectEvent);
	}

	const std::unordered_map<TimePointEvent::TPEventType, int>& Status::getTPlist()
	{
		return m_TPList;
	}

	int Status::getPriority(const TimePointEvent::TPEventType & p_tp)
	{
		return m_TPList[p_tp];
	}

	void Status::attach(unit::Unit * p_u, bool p_nonLevelUpStatus)
	{
		m_unit = p_u; 
		p_u->getStatusContainer()->addStatus(this);

		registerTPEvent();

		// Avoids the scenario during databank setup where level up statuses are attached to a
		// Unit component without an attached GO
		if (p_nonLevelUpStatus)
		{
			auto statusIconsComp = m_unit->getGameObject().getComponent<unit::UnitStatusIcons>();
			if (statusIconsComp != nullptr)
			{
				statusIconsComp->addStatus(this);
			}
		}

		effect();
	}

	void Status::removeThis()
	{
		m_unit->getStatusContainer()->queueRemove(this);
		auto statusIconsComp = m_unit->getGameObject().getComponent<unit::UnitStatusIcons>();
		if (statusIconsComp != nullptr)
		{
			statusIconsComp->removeStatus(this);
		}
	}

	int Status::changeCounter(const std::string & p_cName, int p_value)
	{
		auto found = m_intValue.find(p_cName);
		if ( found != m_intValue.end())
		{
			found->second += p_value;
			return 0;
		}
		//not find target counter
		return 1;
	}

	void Status::checkDuration(const TimePointEvent::TPEventType& p_type)
	{
		if (m_intValue.find(UNIT_DURATION) != m_intValue.end())
		{
			if (m_endEffectEvent == p_type)
				checkDuration();
		}
	}

	void Status::checkDuration()
	{
		changeCounter();

		if (m_intValue.at(UNIT_DURATION) <= 0)
		{
			effectEnd();
			removeThis();
		}
	}


	void Status::setEffectedAD()
	{
		//check if there is effected CD
		auto found = m_intValue.find(STATUS_EFFECTED_AD);
		if (found == m_intValue.end())//don't have
			return;
		
		if (found->second <= 0)//number doesn't right
			return;

		int num = found->second;
		for (int i = 0; i < num; i++)
		{
			//get name
			std::string adName = m_stringValue[STATUS_AD_NAME(i)];

			std::unordered_map<std::string, int> attrChange;

			//get attribute change for ad
			int n = m_intValue[STATUS_AD_ATTRIBUTE_NUM(i)];
			for (int j = 0; j < n; j++)
			{
				//attribute name
				std::string attrName = m_stringValue[STATUS_AD_ATTRIBUTE(i, j)];
				//attribute value change
				int value = m_intValue[STATUS_AD_VALUE(i, j)];

				attrChange[attrName] = value;
			}

			m_effectedAD[adName] = attrChange;
		}
	}

	void Status::changeEffectedAD(bool p_reverse)
	{
		AbilityNode* node = AbilityNodeManager::getInstance()->findNode(ChangeAbilityDescription);
		for (auto it : m_effectedAD)
		{
			for (auto attr : it.second)
			{
				if(p_reverse)
					node->effect(m_unit, it.first, attr.first, -attr.second);
				else
					node->effect(m_unit, it.first, attr.first, attr.second);
			}
		}
	}
}