#pragma once
#include "ability/ability/Ability.h"
#include "unit/Unit.h"

#include <algorithm>
//Rock

namespace ability
{
	void AncientOffer::applyStatus(AbilityInfoPackage* p_info, unit::Unit* p_unit)
	{
		//apply Status to target
		ability::Status* se = ability::StatusManager::getInstance()->findStatus(STATUS_AD_CHANGE);

		//status info
		std::vector<std::string> stringKeys;
		std::vector<std::string> intKeys;
		intKeys.push_back(UNIT_DURATION);

		readADChange(p_info, &intKeys, &stringKeys);

		addStatusInfo(se, p_info, intKeys, stringKeys);

		se->endEffectAt(TimePointEvent::Deal_Damage);


		//attach to target
		se->attach(p_unit);
	}

	int AncientOffer::effect(AbilityInfoPackage* p_info)
	{
		//get power limit
		int power = p_info->m_intValue[UNIT_POWER_LIMIT];
		int hp = 0;
		for (auto u : p_info->m_targets)
		{
			//get hp of u
			hp += u->m_attributes[UNIT_HP];

			// Destroy next frame
			// (need to send info about unit across network first)
			u->queueDestroy();
		}

		//set power
		power = std::min(power, hp);
		addADChange(p_info, 0, UNIT_POWER, power);

		//apply status
		applyStatus(p_info, p_info->m_source);

		int randIndex = (rand() % 3) + 1;
		UniversalSounds::playSound("earthy" + std::to_string(randIndex));

		
		//delete package
		done(p_info);

		return 0;
	}

}