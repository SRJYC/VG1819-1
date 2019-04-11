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
					differentUnitKind = false,
					sameUnitKind = false,
					differentLevel = false,
					allies = false;
			};

			struct Status {
				bool encourage = false;
			};

			struct Ability {
				Filter filter; Status status;
				bool selectRepeat = false,
					hasPower = false, hasPowerLimit = false
					;

				int minRange = 0, maxRange = 0,
					power = 0, targets = 1,
					counterPower = 0, counterChange = 0,
					demonicPresence = 0, powerLimit = 999, duration;

				std::map<std::string, int> counter, counterLimit;

				std::string name;
			};



			int hp = 0, mv = 0, cost = 0, lv = 0, turnsChanneling = 0, clientId, kibbleId;
			bool isCommander = false;
			Status status;
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