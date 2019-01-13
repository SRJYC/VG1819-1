/*
@Rock 11.22

CounterGetter:
	This is a component of Unit Interaction Manager.
	It will ask player to enter a number in range to be the cost.
*/

#pragma once
#include "UnitInteractionManager.h"
class CounterGetter
{
public:
	CounterGetter();
	~CounterGetter();

	void requireCounter(unit::AbilityDescription* p_ad, unit::Unit* p_source);

	void getPlayerChoice(int p_n);
	void cancel();
private:
	std::string m_name;
	kitten::K_GameObject* m_displayWindowGO;

	void getAllCounter(unit::Unit * p_source);
	void playerChooseCounter(unit::AbilityDescription * p_ad, unit::Unit * p_source);

	

};
