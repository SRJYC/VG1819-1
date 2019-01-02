#pragma once

#include "ability/AbilityManager.h"
#include "ability/AbilityInfoPackage.h"
#include "kitten/K_Common.h"
#include "unit/unitComponent/UnitGraphic.h"
#include "UI/CardArt.h"
#include "ability/status/Status.h"

//attributes
#define UNIT_HP "hp"
#define UNIT_MAX_HP "max_hp"
#define UNIT_IN "in"
#define UNIT_BASE_IN "base_in"
#define UNIT_MV "mv"
#define UNIT_BASE_MV "base_mv"
#define UNIT_COST "cost"
#define UNIT_BASE_COST "base_cost"
#define UNIT_LV "lv"

//ability related
#define UNIT_CT "ct"
#define UNIT_CD "cd"
#define UNIT_DURATION "duration"
#define UNIT_POWER "power"

//counter attribute
#define COUNTER_NAME "counter_name"
#define COUNTER_MAX "counter_max"
#define COUNTER_MIN "counter_min"
#define COUNTER_SELECT "counter_select"
//counter select option
#define COUNTER_CHOOSE "choose"
#define COUNTER_ALL "all"
//counter type
#define COUNTER_ENERGY "energy"

//tags
#define STRUCTURE "Structure"

//@Rock

namespace unit
{
	enum UnitSize
	{
		point=0,
		cube,
	};

	struct AbilityDescription
	{
		//may have:
		//name:string
		//power: int
		//targetNumber:int
		//range: int
		//lv:int
		std::unordered_map<std::string, std::string> m_stringValue;
		std::unordered_map<std::string, int> m_intValue;
	};

	struct StatusDescription
	{
		//may have:
		//tp:timePointEvent
		//name:string
		//duration:int
		//power:int
		//lv:int
		std::vector<ability::TimePointEvent::TPEventType> m_TPList;
		std::unordered_map<std::string, std::string> m_stringValue;
		std::unordered_map<std::string, int> m_intValue;
	};

	struct UnitData
	{
		std::string m_name;
		int m_HP;
		int m_MV;
		int m_IN;
		int m_Cost;
		UnitSize m_size; // size is enum

		//Commander and other useful things, commander must be first, in [0], 
		std::vector<std::string> m_tags;

		std::vector<unit::AbilityDescription*> m_ad;
		std::vector<unit::StatusDescription*> m_sd;

		std::string m_spriteName;
		std::string m_texPath;
		std::string m_porPath;
		std::string m_artPath;
	};
}

