#include "AIUnit.h"

#define LOOKUPINT(str) (origAbility->m_intValue[str])
#define LOOKUPSTR(str) (origAbility->m_stringValue[str])
#define CHECKINTEXISTS(str) (origAbility->m_intValue.find(str) != origAbility->m_intValue.end())
#define CHECKSTREXISTS(str) (origAbility->m_stringValue.find(str) != origAbility->m_stringValue.end())

namespace AI {
	namespace Extract {
		Unit::Unit(unit::Unit * p_data) : source(p_data)
		{
			this->hp = p_data->m_attributes[UNIT_HP];
			this->cost = p_data->m_attributes[UNIT_COST];
			this->mv = p_data->m_attributes[UNIT_MV];
			this->lv = p_data->m_attributes[UNIT_LV];
			this->clientId = p_data->m_clientId;
			this->kibbleId = p_data->m_kibbleID;

			for (auto origAbility : p_data->m_ADList) {
				if (origAbility->m_intValue[UNIT_LV] > p_data->m_attributes[UNIT_LV]
					// TODO CHECK IF THE ABILITY IS ON COOLDOWN
					|| (CHECKINTEXISTS("area_fix")) // Delete later
					|| (LOOKUPSTR(ABILITY_NAME) == ABILITY_MANIPULATE_TILE)
					|| (CHECKINTEXISTS(COUNTER_MIN) && p_data->m_attributes[LOOKUPSTR(COUNTER_NAME)] < LOOKUPINT(COUNTER_MIN)) 
					) continue;

				this->ability.push_back(Ability());
				Ability& lastAbility = this->ability.back();

				lastAbility.name = LOOKUPSTR(ABILITY_NAME);

				lastAbility.minRange = LOOKUPINT(MIN_RANGE);
				lastAbility.maxRange = LOOKUPINT(MAX_RANGE);

				lastAbility.targets = CHECKINTEXISTS(UNIT_TARGETS) ? LOOKUPINT(UNIT_TARGETS) : 1;
				lastAbility.selectRepeat = CHECKINTEXISTS(UNIT_SELECT_REPEAT) ? LOOKUPSTR(UNIT_SELECT_REPEAT)  ==  "true" : false;
				
				lastAbility.power= CHECKINTEXISTS(UNIT_POWER)?LOOKUPINT(UNIT_POWER): 0;
				lastAbility.counterChange = CHECKINTEXISTS(COUNTER_CHANGE)?LOOKUPINT(COUNTER_CHANGE):0;

				bool checkIfFiltersExist =
					origAbility->m_stringValue.find("filter") != origAbility->m_stringValue.end()
					, check2 = origAbility->m_intValue.find("filter") != origAbility->m_intValue.end();

				if (CHECKINTEXISTS("filter")) {
					int filters = LOOKUPINT("filter");
					for (int i = 0; i < filters; ++i) {
						std::string filter = LOOKUPSTR("filter" + std::to_string(i));
						if (filter.compare("enemy") == 0) {
							lastAbility.filter.enemies = true;
						}
						else if (filter.compare("no_unit") == 0) {
							lastAbility.filter.unoccupiedTiles = true;
						}
						else if (filter.compare("owned_tile") == 0) {
							lastAbility.filter.tilesOwnedByTeam = true;
						}
						else if (filter.compare("water") == 0) {
							lastAbility.filter.water = true;
						}
					}
				}
				lastAbility.filter.unoccupiedTiles = LOOKUPINT(UNIT_NEED_UNIT);

				// TODO ADD the counter buildup map setter. 
				//std::map<std::string, int> counter, counterLimit;
			}

			if (this->isCommander = p_data->isCommander()) {

			}
			else {
			}
		}
		Unit::Ability manipulateTile, join, summon, move;
		void setupUniqueAbilities()
		{
			// Setup Tile manupilation ability
			manipulateTile.name = ABILITY_MANIPULATE_TILE;
			manipulateTile.minRange = 1;
			manipulateTile.maxRange = 1;
			manipulateTile.filter.tilesOwnedByAny = true;
			manipulateTile.filter.water = true;


			// Setup Summon ability
			summon.name = ABILITY_SUMMON_UNIT;
			summon.minRange = 1;
			summon.maxRange = 1;
			summon.filter.occupiedTiles = true;
			summon.filter.tilesNotOwnedByTeam = true;
			summon.filter.water = true;
			
			// Setup join ability
			join.name = ACTION_JOIN;
			join.minRange = 1;
			join.maxRange = 1;
			join.filter.unoccupiedTiles = true;
			join.filter.enemies = true;
			join.filter.differentUnitKind = true;

			// Setup movement
			move.name = ACTION_MOVE;
			move.filter.water = true;
		}

		UnitCard::UnitCard(unit::Unit* p_data, int p_handIndex) : source(p_data) {
			this->cost = p_data->m_attributes[UNIT_COST];
			this->kibbleId = p_data->m_kibbleID;
			this->handIndex = p_handIndex;

		}
	}
}