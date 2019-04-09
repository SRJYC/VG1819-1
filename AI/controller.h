#pragma once
#include "AI/model.h"
#include "kitten/K_Component.h"
#include "kitten/event_system/EventManager.h"
#include "unit/Unit.h"
#include "Extract/Action.h"

namespace AI {

	class controller : public kitten::K_Component {
	private:
		Model m_model;
		Extract::Sequence m_sequence;
		unit::Unit* m_unit = nullptr;
		int m_playerID;

		void runTurn(unit::Unit* p_unit);

	public:

		controller();
		~controller();

		void start() override;

		static void setupAIControllers();

		struct availableInfo{
			bool canAct = true, canMove = true;
			std::vector<int> summonableUnits, handCardsPicked;
			int availableEnergy=0, summoned = 0;
			std::pair<int,int> curPos;
			std::vector<std::pair<int, int>> blockedPos;
			unit::Unit* sourceUnit;
			
		};
		void generateSequences(Extract::Sequence p_currentSeq, std::vector<Extract::Sequence>& p_sequences, availableInfo p_info);

		void nextActionInSequenceHandler(kitten::Event::EventType p_type, kitten::Event* p_data);
		void nextTurnHandler(kitten::Event::EventType p_type, kitten::Event* p_data);

		static controller* getAIController(int p_playerId);
		static int getAIControllerSize();
		static Model* getAIModel(int p_playerId);
		static bool AIPresent();

		void onDisabled()override;

		void setupEventListeners();
		void tearDownEventListeners();
	};
}
