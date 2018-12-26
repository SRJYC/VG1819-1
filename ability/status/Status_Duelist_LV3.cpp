#pragma once
#include "ability/status/Status.h"
#include "unit/Unit.h"
//Rock

namespace ability
{
	Status_Duelist_LV3::Status_Duelist_LV3() : Status_LV::Status_LV()
	{
		m_name = STATUS_DUELIST_LV3;
		addTimePoint(TimePointEvent::Level_Up);
	}

	int Status_Duelist_LV3::effect(ability::TimePointEvent::TPEventType p_type, ability::TimePointEvent * p_event)
	{
		if (Status_LV::effect(p_type, p_event) == 0)
		{
			AbilityNode* node = AbilityNodeManager::getInstance()->findNode(ChangeAbilityDescription);

			node->effect(m_unit, ABILITY_SLAY, UNIT_POWER, 2);//increase slay dmg by 2
			node->effect(m_unit, ABILITY_DEFEND, UNIT_CD, -1);//decrease defend cd by 1

			return 0;
		}
		return 1;
	}
}