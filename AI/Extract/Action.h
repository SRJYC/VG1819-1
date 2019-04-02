#pragma once 
#include<vector>
#include "unit/Unit.h"
#include "AI/Extract/Board.h"
#include "unitInteraction/UnitInteractionManager.h"
#include "kitten/K_GameObjectManager.h"
#include "kitten/K_ComponentManager.h"
#include "kibble/databank/databank.hpp"

namespace AI {
	namespace Extract {
		struct Action {
			double weight = 0; 
			virtual void run(unit::Unit* p_unit) {};
		};

		struct Sequence {
			std::vector<Action*> actions;
			double weight = 0;
			int cStep = 0;
			bool complete = false;
			void run(unit::Unit* p_unit);
			void step(unit::Unit* p_unit);

			Sequence();
			~Sequence();

			struct weightComp {
				bool operator()(const Sequence& l, const Sequence& r) const { return l.weight > r.weight; }
			};
		};

		struct Move : public Action {
			int targetX, targetY;

			Move(int targetX, int targetY) : targetX(targetX), targetY(targetY) {}

			void run(unit::Unit* p_unit) override;
		};

		struct ManipulateTile : public Action {
			int targetX, targetY;

			ManipulateTile(int targetX, int targetY) : targetX(targetX), targetY(targetY) {}

			void run(unit::Unit* p_unit) override;
		};

		struct Summon : public Action {
			int targetX, targetY;
			int targetToSummon;
			int handOffset=0;
			Summon(int targetX,int targetY, int targetToSummon, int handOffset): targetX(targetX), targetY(targetY), targetToSummon(targetToSummon), handOffset(handOffset) {}

			void run(unit::Unit* p_unit) override;
		};

		struct Ability : public Action {
			int targetX, targetY;
			std::string abilityName;
			Ability(int targetX, int targetY, std::string abilityName) : targetX(targetX), targetY(targetY),abilityName(abilityName) {}
			void run(unit::Unit* p_unit) override;
		};

	}
}