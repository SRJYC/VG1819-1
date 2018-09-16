#pragma once
#include "UnitCommon.h"


//Rock

class Unit
{
public:
	std::string m_ID;

	std::vector<std::string> m_tags;
	std::string m_name;

	std::map<std::string, int> m_attributes;

	int m_maxHP;
	int m_HP;
	int m_baseMV;
	int m_MV;
	int m_baseIN;
	int m_IN;
	int m_baseCost;
	int m_Cost;
	int m_LV;

	UnitSize m_size;

	std::map<std::string, int> m_Counter;
	std::vector<Ability*> m_Ability;
	std::vector<Status*> m_Status;

	Unit();
	~Unit();

	void addAbility(Ability *p_newAbility);
	void addStatus(Status *p_newStatus);
	bool removeStatus(Status *p_oldStatus);

	std::vector<Ability*> getAbilities() { return m_Ability; };
	std::vector<Status*> getStatus() { return m_Status; };

	int useAbility(int p_abilityIndex);
	int callStatus(int p_StatusIndex, int p_event);
	//event is the one of the events that this status registered
	//should be enum, I didn't define it yet, so use int as place holder
};

