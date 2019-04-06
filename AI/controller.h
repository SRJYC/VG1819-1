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

		std::vector<std::pair<int, int>> getTargetsInRange(const retainedInfo & p_retainedInfo, const passedInfo & p_passedInfo, const targettingInfo& p_targgetingInfo);
		std::vector<AI::Extract::Move> getAvailableMoves(const retainedInfo & p_retainedInfo, const passedInfo & p_passedInfo, const targettingInfo& p_targgetingInfo);
		std::vector<Extract::UnitCard> getSummonableCards();
		void setUnitLists(retainedInfo& rInfo);

		// it's under the assumption that the passed info is a newly setup one in the previous level.
		void generateSequences(retainedInfo& p_retainedInfo, passedInfo& p_passedInfo);

	public:

		controller();
		~controller();

		void start() override;

		static void setupAIControllers();

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
