#pragma once
#include "ability/node/AbilityNode.h"
#include "Unit/Unit.h"
#include <iostream>
#include <sstream>
//Rock

namespace ability
{
	ChangeAbilityDescriptionNode::ChangeAbilityDescriptionNode()
	{
	}

	int ChangeAbilityDescriptionNode::effect(unit::Unit* p_target, const std::string& p_abilityName, const std::string& p_valueName, int p_value)
	{
		//find ability
		auto found = p_target->m_ADMap.find(p_abilityName);
		if (found != p_target->m_ADMap.end())
		{
			std::cout << p_abilityName << " : " << p_valueName << " is changed by " << p_value << std::endl;
			found->second->m_intValue[p_valueName] += p_value;
			return 0;
		}
		//not found ability
		return 1;
	}

	int ChangeAbilityDescriptionNode::effect(unit::Unit* p_target, const std::string& p_abilityName, const std::string& p_valueName, const std::string& p_value)
	{
		//find ability
		auto found = p_target->m_ADMap.find(p_abilityName);
		if ( found != p_target->m_ADMap.end())
		{
			std::cout << p_abilityName << " : " << p_valueName << " is changed to " << p_value << std::endl;
			found->second->m_stringValue[p_valueName] = p_value;
			return 0;
		}
		//not found ability
		return 1;
	}

	void ChangeAbilityDescriptionNode::addFilter(unit::Unit * p_target, const std::string & p_abilityName, const std::string & p_filter, const std::string & p_filterName)
	{
		auto found = p_target->m_ADMap.find(p_abilityName);
		if (found != p_target->m_ADMap.end())
		{
			//get number of filter
			unit::AbilityDescription* ad = found->second;

			int num = ad->m_intValue[p_filter];

			int index = getFilter(ad, num, p_filter, p_filterName);

			if (index == num)//filter doesn't exist
			{
				ad->m_intValue[p_filter] += 1;//filter number increases

				std::stringstream stm;
				stm << p_filter << index;
				std::string tkey = stm.str();

				ad->m_stringValue[tkey] = p_filterName;//add filter
			}
			//filter exist
		}
	}

	void ChangeAbilityDescriptionNode::removeFilter(unit::Unit * p_target, const std::string & p_abilityName, const std::string & p_filter, const std::string & p_filterName)
	{
		auto found = p_target->m_ADMap.find(p_abilityName);
		if (found != p_target->m_ADMap.end())
		{
			//get number of filter
			unit::AbilityDescription* ad = found->second;

			int num = ad->m_intValue[p_filter];

			int index = getFilter(ad, num, p_filter, p_filterName);

			if (index < num)//filter exist
			{
				ad->m_intValue[p_filter] -= 1;//filter number decreases

				std::stringstream stm;
				stm << p_filter << (num - 1);
				std::string tkey = stm.str();

				std::string buffer = ad->m_stringValue[tkey];//buffer last filter

				//from last filter, every filter move forward
				for (int i = num-2; i >= index; i--)
				{
					std::stringstream stm;
					stm << p_filter << i;
					std::string tkey = stm.str();

					std::string temp = buffer;
					buffer = ad->m_stringValue[tkey];
					ad->m_stringValue[tkey] = temp;
				}

			}
			//filter doesn't exist
		}
	}

	int ChangeAbilityDescriptionNode::getFilter(unit::AbilityDescription * p_ad, int p_num, const std::string & p_filter, const std::string & p_filterName)
	{
		//check if the filter exist
		for (int i = 0; i < p_num; i++)
		{
			std::stringstream stm;
			stm << p_filter << i;
			std::string tkey = stm.str();

			if (p_ad->m_stringValue[tkey] == p_filterName)
				return i;//exist, then return the index
		}

		return p_num;//doesn't exist, then return next index
	}
}