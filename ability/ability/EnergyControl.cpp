#pragma once
#include "ability/ability/Ability.h"

#include "unit/Unit.h"

//Rock

namespace ability
{
	int EnergyControl::effect(AbilityInfoPackage* p_info)
	{
		if (checkTarget(p_info))
		{
			unit::Unit* target = p_info->m_targets[0];
			int power = p_info->m_intValue[COUNTER_CHANGE];

			int limit = p_info->m_intValue[COUNTER_MAX];
			int counter = target->m_attributes[COUNTER_ENERGY];

			if (counter + power > limit)
			{
				power = limit - counter;
				if (power < 0)
					power = 0;
			}

			//change counter
			changeCounter(target, COUNTER_ENERGY, power);

			UniversalSounds::playSound("magic_power_up");
		}

		//delete package
		done(p_info);

		

		return 0;
	}
}