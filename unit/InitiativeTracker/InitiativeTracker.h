#pragma once
#include "unit/InitiativeTracker/UnitTurn.h"
#include "kitten/K_GameObject.h"
#include "unit/InitiativeTracker/UnitAura.h"
#include "TurnChangeDisplay.h"
#include "kitten\event_system\Event.h"
#include "kitten\event_system\EventExample.h"
//Rock
//This class should be singleton
//it handles all units on the board
//it decides the order the units take turn

namespace unit
{
	class UnitTurn;
	class InitiativeTrackerUI;
	class InitiativeTracker
	{
	private:
		static InitiativeTracker * sm_instance;
		UnitTurn* m_uturn;
		InitiativeTrackerUI* m_UI;
		kitten::K_GameObject* m_uAura;
		TurnChangeDisplay* m_display;

		//list for new units
		std::vector<kitten::K_GameObject*> m_waitUnitObjectList;
		//list for old units
		std::vector<kitten::K_GameObject*> m_unitObjectList;

		//the unit that takes extra turn
		std::vector<kitten::K_GameObject*> m_extraTurnUnitList;
		int m_indexOfBreakPoint;//the index of break point

		int m_currentUnitIndex;

		bool m_gameStarted;

		void sortListByIn(int p_list=0);
		void addWaitList();
		int getUnitObjectIndex(kitten::K_GameObject* p_uGO);

	public:
		InitiativeTracker();
		~InitiativeTracker();

		static void createInstance() { assert(sm_instance == nullptr); sm_instance = new InitiativeTracker(); };
		static void destroyInstance() { assert(sm_instance != nullptr); delete(sm_instance); sm_instance = nullptr; };
		static InitiativeTracker* getInstance() { return sm_instance; };

		void addUnit(kitten::K_GameObject* p_unit);
		bool removeUnit(kitten::K_GameObject* p_unit);

		kitten::K_GameObject* getUnitByIndex(int p_index);
		int getUnitNumber();//get unit number in tracker list, may have repeat
		int getUnitNumberOnBoard();//get unit number on board
		kitten::K_GameObject* getCurrentUnit();
		int getCurrentUnitIndex();
		//int getIndexByUnit(kitten::K_GameObject* p_u);

		std::vector<kitten::K_GameObject*> getUnitList();

		//turn phase
		//start of whole turn->unit turn->next->repeat until no more unit->start of whole turn
		void gameTurnStart();
		void unitTurnEnd();
		void gameTurnEnd();

		void addExtraTurn(kitten::K_GameObject* p_unit);
		
		//button listener
		void newTurnListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	};
}