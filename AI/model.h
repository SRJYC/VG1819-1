#pragma once
#include "Extract/Board.h"
#include "Extract/Hand.h"
#include "Extract/PowerTracker.h"
#include "Extract/Deck.h"
#include "Extract/Unit.h"
#include "Extract/Action.h"
#include "unit/Unit.h"

namespace AI {
	struct Model {
		int playerId;
		Extract::Hand hand;
		Extract::Deck deck;
		Extract::Board board;
		Extract::PowerTracker powerTracker;

		std::vector<Extract::Move> getAvailableMoves(unit::Unit* p_unit);
		std::vector<int> getSummonableCards();
		std::vector<unit::Unit*> getOtherPlayerUnits();
		std::vector<unit::Unit*> getOwnUnits();
		struct TargetRange {
			int min_range, max_range;
			unit::Unit* unit, *targetUnit;
			std::pair<int, int> currentPlacement,targetPlacement;
			std::vector<std::pair<int, int>> blockedPos;
		};
		std::vector<Extract::Move> getAvailableMoves(TargetRange p_Target);
		std::vector<unit::Unit*> getTargetsInRange(TargetRange p_Target);
		std::vector<TileInfo*> getTargetTilesInRange(TargetRange p_Target);
		std::vector<TileInfo*> getTargetNonBlockedTilesInRange(TargetRange p_Target);
		std::vector<TileInfo*> getTargetOwnedTilesInRange(TargetRange p_Target);
		std::vector<TileInfo*> getTargetNotOwnedTilesInRange(TargetRange p_Target);
	};
}