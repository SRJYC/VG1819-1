#pragma once
#include "unit/UnitCommon.h"
#include "unit/unitComponent/StatusContainer.h"
#include "unit/InitiativeTracker/InitiativeTracker.h"
#include "unit/unitComponent/CooldownRecorder.h"
#include "unit/unitComponent/Commander.h"
#include "unit/unitComponent/CastTimer.h"
#include "unit\unitComponent\UnitSelect.h"

#include "unitComponent\UnitHealthBar.h"

#include "_Project\LerpController.h"

//@Rock
//although unit is just a component of game object
//it acts like central system which pass commands to other components
	struct Behavior;
namespace unit
{
	class UnitTurn;
	class Unit : public kitten::K_Component, public LerpController::ScaleLerpFinishedCallback
	{
	private:

		enum HealthBarState
		{
			none,
			destroying
		};

	private:
		UnitTurn* m_turn;
		StatusContainer * m_statusContainer;
		CooldownRecorder * m_cdRecorder;
		CastTimer * m_castTimer;
		Commander * m_commander;

		//bool m_isStructure;
		std::unordered_map<std::string, bool> m_tagCheckMap;

		std::vector<kitten::K_GameObject*> m_path;

		bool m_lateDestroy = false;
		AbilityDescription m_joinAD;

		std::string m_portraitTexturePath;

		// Healthbar related
		UnitHealthBar* m_healthBar;
		UnitSelect* m_unitSelect;

		HealthBarState m_healthBarState;
		

		virtual void onScaleLerpFinished(kitten::K_GameObject* p_obj) override;
	
		virtual void start() override;

		//set auto cast ability
		bool m_autoCast = false;
		std::string m_autoAbility;
		void setAutoAbility(const std::string& p_name);

		//item
		kitten::K_GameObject* m_itemGO;
	public:
		//members

		int m_numberID;
		std::string m_ID;
		int m_kibbleID;

		std::vector<std::string> m_tags;
		std::string m_name;
		std::unordered_map<std::string, int> m_attributes;//attributes includes all numerical attribute and counter
		UnitSize m_size;

		std::map<std::string,Behavior*> m_AbilityBehavior;

		//two list for ad, 

		//one is used for iterating it in order, 
		std::vector<unit::AbilityDescription*> m_ADList;

		//one is used for get exact ad by name
		std::unordered_map<std::string, unit::AbilityDescription*> m_ADMap;

		int m_clientId = -1;


		//functions
		Unit();
		Unit(const unit::Unit * source);
		~Unit();

		//level up
		void setJoinAD();
		void join();
		void levelup();
		
		//commander interface
		void addCommander(Commander* p_c);
		//void manipulateTile();
		bool isCommander() const;

		//check tag
		bool checkTag(const std::string& p_tag);

//		void summonUnit(int p_id);
		//status interface
		/*
		void addStatus(ability::Status *p_newStatus);
		bool removeStatus(ability::Status *p_oldStatus);
		ability::Status* getStatus(const std::string& p_name);*/
		StatusContainer* getStatusContainer();

		//trigger event
		void triggerTP(ability::TimePointEvent::TPEventType p_tp, ability::TimePointEvent* p_event = nullptr);

		//turn 
		bool hasUpdate() const override { return true; };
		void update() override;
		void turnStart(UnitTurn* p_t);
		bool canMove();
		bool canAct() const;
		void moveDone();
		void actDone();
		bool isTurn() const;
		void turnEnd();
		void playerSkipTurn(bool p_sendPacket = true);

		//position function
		kitten::K_GameObject* getTile();

		//move
		void move();//move action, no restriction, no info needs
		void move(int p_min, int p_max);//move by ability, need range of ability
		void move(kitten::K_GameObject* p_tile);//move to a specific tile by ability
		void move(std::vector<kitten::K_GameObject*> p_path);
		void moveAnime(kitten::K_GameObject* p_tile);

		//ability
		int useAbility(const std::string& p_abilityName, bool p_autoClick = false);
		void cancelAbility(AbilityDescription* p_ad);
		void setCast(AbilityDescription* p_ad, ability::AbilityInfoPackage* p_pack);
		void cancelCast();
		int checkCD(const std::string& p_abilityName);

		//destroy
		int destroyedByDamage();
		int destroyedByJoin();
		void simpleDestroy();
		void destroy();

		// Portrait Texture
		void setPortraitTexturePath(const std::string& p_path) { m_portraitTexturePath = p_path; }
		const std::string& getPortraitTexturePath() const { return m_portraitTexturePath; }

		//item, capture the flag mode
		const bool hasItem() const;
		void addItem(kitten::K_GameObject* p_item);
		void removeItem();
		kitten::K_GameObject* getItem() const;
	};
}
