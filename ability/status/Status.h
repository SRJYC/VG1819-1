
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

/*
Priority: The order of status to be trigger in same Time Point, 
higher priority will be trigger early since it will be sorted in the front of list

0:Default priority, normal speed of changing data, 
or the data doesn't matter as long as the Time Point is reached.
Most status are 0 priority
-1:Block, Dodge
-2:Shield
-3:Lord Order(Receive Damage)
*/

#pragma once
#include "ability/node/AbilityNodeManager.h"
#include "ability/status/statusEvent/TimePointEvent.h"
#include "kitten/event_system/EventManager.h"

#include <string>
#include <vector>

#define INFO_PACKAGE_KEY "info"

namespace ability
{
	class Status
	{
	public:
		enum StatusType
		{
			None,
			Stat_Debuff,
			Stat_Buff,
			MV_Debuff,
			MV_Buff,
			Shield
		};
		
		std::string m_source;
		std::unordered_map<std::string, int> m_intValue;
		std::unordered_map<std::string, std::string> m_stringValue;

		Status();
		virtual ~Status();

		//set property
		void changeName(const std::string & p_msg);
		void changeLV(int p_lv);
		void changeDescription(const std::string & p_msg);
		//void setEffectedAD(const std::string & p_msg);
		void addCounter(const std::string & p_key, int p_value);
		void addAttributeChange(const std::string & p_key, int p_value);
		void addTimePoint(const TimePointEvent::TPEventType& p_value, int p_priority = 0);
		void setCaster(unit::Unit* p_u);

		//change when to reduce duration counter, turn end is default
		void endEffectAt(const TimePointEvent::TPEventType& p_value = TimePointEvent::Turn_End);


		// Getters for info
		const std::unordered_map<TimePointEvent::TPEventType, int>& getTPlist();
		int getPriority(const TimePointEvent::TPEventType& p_tp);
		int getLV() { return m_LV; }
		const std::unordered_map<std::string, int>& getCounters() { return m_intValue; }
		const std::unordered_map<std::string, int>& getAttributeChanges() { return m_attributeChange; }
		std::string getDescription() { return m_description; };
		std::string getName() { return m_name; };
		std::string getID() { return m_Id; };

		//common status method
		void attach(unit::Unit* p_u, bool p_nonLevelUpStatus = true);
		virtual Status* clone() const = 0;
		virtual int effect();//activate when attached to unit
		virtual int effect(const TimePointEvent::TPEventType& p_type, TimePointEvent* p_event);
		void registerTPEvent();

		StatusType getStatusType() { return m_statusType; }

		//for test
		void print();

		//register detroy notifier
		void notifyUnitDestroy(unit::Unit* p_u);
	protected:
		//the text that will be showed to player
		std::string m_name;
		std::string m_description;
		std::string m_Id;//the id identify status

		unit::Unit * m_unit;//the unit this status attached to
		
		std::unordered_map<std::string, int> m_attributeChange;

		int m_LV = 0;//lv of level up status

		//the unit who cast the ability to apply the status
		unit::Unit* m_caster;

		//the list of event that will be registered and its priority
		std::unordered_map<TimePointEvent::TPEventType, int> m_TPList;

		TimePointEvent::TPEventType m_endEffectEvent = TimePointEvent::None;

		StatusType m_statusType = StatusType::None;

		//common help method for status

		void removeThis();//remove this status from status container

		//change counter, it change duration by default
		int changeCounter(const std::string& p_cName = "duration", int p_value = -1);
		
		//check if the event of duration deduction happens, if it is reduce duration
		void checkDuration(const TimePointEvent::TPEventType& p_type);
		void checkDuration();

		//end effect
		virtual void effectEnd() {};


		//				effectedAD name :               (attribute : value)
		std::unordered_map<std::string, std::unordered_map<std::string, int> > m_effectedAD;
		
		
		//change effected ad attributes
		void setEffectedAD();

		//actually change
		void changeEffectedAD(bool p_reverse = false);


		void tryRemoveCaster(unit::Unit* p_u);

		void tryDeregisterDestroy();
	};

	/*
	class Status_LV : public Status
	{
		//this class handle the attribute change for all lv up status
	public:
		Status_LV();
		virtual Status* clone() const { return new Status_LV(*this); };
		virtual int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};*/

	class Status_Encourage : public Status
	{
		//this is buff apply by ability [Encourage]
		//it triggers when unit will deal damage
		//then it increase the ability's power by this status power
	public:
		Status_Encourage();
		Status* clone() const { return new Status_Encourage(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	class Status_Dodge : public Status
	{
		//this is buff apply by ability [Dodge]
		//it triggers when unit will receive damage
	public:
		Status_Dodge();
		Status* clone() const { return new Status_Dodge(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	/*
	class Status_Priest_LV3 : public Status_LV
	{
		//this is trigger when preiest is lv3
	private:
		bool m_activate = false;
	public:
		Status_Priest_LV3();
		Status* clone() const { return new Status_Priest_LV3(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	}; 

	class Status_Archer_LV3 : public Status_LV
	{
		//this is trigger when Archer is lv3
	public:
		Status_Archer_LV3();
		Status* clone() const { return new Status_Archer_LV3(*this); };

		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	class Status_Duelist_LV3 : public Status_LV
	{
		//this is trigger when Duelist is lv3
	public:
		Status_Duelist_LV3();
		Status* clone() const { return new Status_Duelist_LV3(*this); };

		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};*/

	class Status_Temp_Change : public Status
	{
	public:
		Status_Temp_Change();
		Status* clone() const { return new Status_Temp_Change(*this); };
		int effect();
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
		void effectEnd();

	};

	class Status_AD_Change : public Status
	{
	public:
		Status_AD_Change();
		Status* clone() const { return new Status_AD_Change(*this); };
		int effect();
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
		void effectEnd();
	};

	/*
	class Status_Load : public Status
	{
	public:
		Status_Load();
		Status* clone() const { return new Status_Load(*this); };
		int effect();
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
		void effectEnd();
	};*/

	class Status_Shield : public Status
	{
	public:
		Status_Shield();
		Status* clone() const { return new Status_Shield(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	class Status_Block : public Status
	{
	public:
		Status_Block();
		Status* clone() const { return new Status_Block(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	class Status_Curse : public Status
	{
	public:
		Status_Curse();
		Status* clone() const { return new Status_Curse(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	/*
	class Status_Eternal_Eye_LV3 : public Status_LV
	{
		//this is trigger when Eternal Eye is lv3
	public:
		Status_Eternal_Eye_LV3();
		Status* clone() const { return new Status_Eternal_Eye_LV3(*this); };

		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	class Status_Lancer_LV2 : public Status_LV
	{
		//this is trigger when Lancer is lv2
	public:
		Status_Lancer_LV2();
		Status* clone() const { return new Status_Lancer_LV2(*this); };

		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);

	private:
		bool m_active = false;

		void generateArmor();
	};*/

	class Status_IN_Change : public Status
	{
	public:
		Status_IN_Change();
		~Status_IN_Change();
		Status* clone() const { return new Status_IN_Change(*this); };
		int effect();
		void effectEnd();

		void registerEvent();
		void listenEvent(kitten::Event::EventType p_type, kitten::Event * p_data);
	};

	class Status_Attach : public Status
	{
	public:
		Status_Attach();
		Status* clone() const { return new Status_Attach(*this); };
		int effect();
	};

	/*
	class Status_Wraith_LV2 : public Status_LV
	{
	public:
		Status_Wraith_LV2();
		Status* clone() const { return new Status_Wraith_LV2(*this); };

		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	class Status_Evil_Fiend_LV : public Status_LV
	{
	public:
		Status_Evil_Fiend_LV();
		Status* clone() const { return new Status_Evil_Fiend_LV(*this); };

		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};*/

	class Status_Vampiric_Curse : public Status
	{
	public:
		Status_Vampiric_Curse();
		Status* clone() const { return new Status_Vampiric_Curse(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	/*
	class Status_Gorefiend_LV3 : public Status_LV
	{
	public:
		Status_Gorefiend_LV3();
		Status* clone() const { return new Status_Gorefiend_LV3(*this); };

		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};*/

	class Status_Cursed_Being : public Status
	{
	private:
		void applyStatus(unit::Unit* p_u);//apply vampiric curse to target
	public:
		Status_Cursed_Being();
		Status* clone() const { return new Status_Cursed_Being(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	//summon unit when attached unit is destroyed
	class Status_Last_Word_Summon : public Status
	{
	public:
		Status_Last_Word_Summon();
		Status* clone() const { return new Status_Last_Word_Summon(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
		void setUnitToSummon(int p_id, int p_lv = 1);
	};


	class Status_Poison : public Status
	{
	public:
		Status_Poison();
		Status* clone() const { return new Status_Poison(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};

	class Status_Lord_Order : public Status
	{
	public:
		Status_Lord_Order();
		Status* clone() const { return new Status_Lord_Order(*this); };
		int effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event);
	};
}



