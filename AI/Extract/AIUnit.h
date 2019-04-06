#pragma once
#include "unit/Unit.h"

namespace AI {
	namespace Extract {


		struct Unit {

			struct Filter {
				bool enemies = false,
					water = false,
					tilesOwnedByAny = false,
					tilesOwnedByTeam = false,
					tilesNotOwnedByTeam = false,
					occupiedTiles = false,
					unoccupiedTiles = false,
					allies = false;
			};

			struct Ability {
				Filter filter;
				bool selectRepeat = false,
					hasPower = false
					;

				int minRange = 0, maxRange = 0,
					power = 0, targets = 1,
					counterPower = 0, counterChange = 0;

				std::map<std::string, int> counter, counterLimit;

				std::string name;
			};



			int hp = 0, mv = 0, cost = 0, turnsChanneling = 0, clientId;
			bool isCommander = false;
			unit::Unit* source;
			std::vector<Ability> ability;
			Unit(unit::Unit* p_data);
		};

		extern Unit::Ability manipulateTile, join, summon, move;
		void setupUniqueAbilities();

		struct UnitCard {
			int cost, kibbleId;
			int handIndex;
			unit::Unit* source;
			UnitCard(unit::Unit* p_data, int handIndex);
		};
	}



}