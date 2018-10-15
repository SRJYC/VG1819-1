#pragma once
#include "ability/node/AbilityNodeManager.h"

#include <string>
#include <unordered_map>
#include <vector>

//Rock

//Status include Buff, Debuff, Passive Ability, and level up bonus
//It has ability nodes as Ability, but the effects are triggered by some event or time point

//Event can be a enum, such as Level up, Turn Start, Tile Change....
//Status should register some of the event to something high level
//when any of the event occurs, high level class should notify this
//and this should do something based on which event occur.

//EXAMPLE: Demonic Restriction will handle Tile Change event
//if it's not Demonic Presence Tile, its effect will activate
//otherwise deactivate

//EXAMPLE: a "Until next turn" Status will handle Turn Start event
//it will decrease duration by 1 and see if it's zero
//then decide to remove this effect


namespace ability
{
	enum TimePointEvent
	{
		Turn_Start,
		Turn_End,
		Level_Up,
		New_Tile,
		Deal_Damage,
		Receive_Damage,
	};

	class Status
	{
	public:
		unit::Unit * m_unit;//the unit this status attached to

		std::unordered_map<std::string, int> m_counter;
		//Most commonly counter is duration. But it can be more, such as how many times it can be used

		std::string m_name;
		std::string m_description;//the text that will be showed to player
		int m_LV;
		std::unordered_map<std::string, int> m_attributeChange;

		std::vector<ability::TimePointEvent> m_TPList;//the list of event that will be registered
		//TO DO: Register Event

		virtual int effect();//this is effect that activates when status is added
		virtual int effect(TimePointEvent p_timePoint);
		virtual int effect(TimePointEvent p_timePoint, ability::AbilityInfoPackage* p_pack);
		virtual int effect(TimePointEvent p_timePoint, int p_value);
		//TO DO: Listen Event and take effect
		//Maybe use Chain of Responsibility

		Status();
		virtual ~Status();

		void attach(unit::Unit* p_u);

		virtual Status* clone() const = 0;

	protected:
		void removeThis();//TO DO: remove this Status from Unit, and delete the instance
		int changeCounter(const std::string& p_cName = "duration", int p_value = -1);
		void checkDuration();
	};

	class Status_LV : public Status
	{
		//this class handle the attribute change for all lv up status
	public:
		virtual Status* clone() const { return new Status_LV(*this); };
		virtual int effect(TimePointEvent p_timePoint, int p_value);
	};

	class Status_CD : public Status
	{
		//this status should store all cd and ct
		//at turn start, all cd ct decrease by 1
		//when unit use ability, it needs to check here
	public:
		std::string m_abilityName;//the name of the ability that's in cooldown

		Status_CD();
		virtual ~Status_CD();
		Status* clone() const { return new Status_CD(*this); };
		int effect(TimePointEvent p_timePoint);
	};

	class Status_Encourage : public Status
	{
		//this is buff apply by ability [Encourage]
		//it triggers when unit will deal damage
		//then it increase the ability's power by this status power
	public:
		Status_Encourage();
		virtual ~Status_Encourage();
		Status* clone() const { return new Status_Encourage(*this); };
		int effect(TimePointEvent p_timePoint);
		int effect(TimePointEvent p_timePoint, ability::AbilityInfoPackage* p_pack);
	};

	class Status_Dodge : public Status
	{
		//this is buff apply by ability [Dodge]
		//it triggers when unit will receive damage
	public:
		Status_Dodge();
		virtual ~Status_Dodge();
		Status* clone() const { return new Status_Dodge(*this); };
		int effect(TimePointEvent p_timePoint);
		int effect(TimePointEvent p_timePoint, ability::AbilityInfoPackage* p_pack);
	};

	class Status_Priest_LV3 : public Status_LV
	{
		//this is trigger when preiest is lv3
	private:
		bool m_activate = false;
	public:
		Status_Priest_LV3();
		virtual ~Status_Priest_LV3();
		Status* clone() const { return new Status_Priest_LV3(*this); };
		int effect(TimePointEvent p_timePoint);
		int effect(TimePointEvent p_timePoint, int p_value);
	}; 

	class Status_Archer_LV3 : public Status_LV
	{
		//this is trigger when Archer is lv3
	public:
		Status_Archer_LV3();
		virtual ~Status_Archer_LV3();
		Status* clone() const { return new Status_Archer_LV3(*this); };

		int effect(TimePointEvent p_timePoint, int p_value);
	};

	class Status_Duelist_LV3 : public Status_LV
	{
		//this is trigger when Duelist is lv3
	public:
		Status_Duelist_LV3();
		virtual ~Status_Duelist_LV3();
		Status* clone() const { return new Status_Duelist_LV3(*this); };

		int effect(TimePointEvent p_timePoint, int p_value);
	};
}



