#pragma once
#include "Extract/Board.h"
#include "Extract/Hand.h"
#include "Extract/PowerTracker.h"
#include "Extract/Deck.h"
#include "Extract/AIUnit.h"
#include "Extract/Action.h"



namespace AI {

	struct Model {
		int playerId;
		Extract::Hand hand;
		Extract::Deck deck;
		Extract::Board board;
		Extract::PowerTracker powerTracker;
	};

	// Info that is retained through out the recursion
	struct retainedInfo {
		Extract::Unit source;
		std::vector<Extract::Sequence> generatedSequences;
		std::vector<unit::Unit*> allUnits, enemyUnits, allyUnits;
		Extract::Hand& hand;
		Extract::Deck& deck;
		Extract::Board& board;
		Extract::PowerTracker& powerTracker;

		retainedInfo(unit::Unit* p_unit, Model& p_model);
	};

	// Info that changes with each level of recursion
	struct passedInfo {
		bool canAct = true, canMove = true;
		int availableEnergy = 0, summoned = 0;
		std::pair<int, int> curPos;
		std::vector<std::pair<int, int>> blockedPos; // positions that are blocked due to an action that was performed
		std::vector<int> handCardsPicked;
		Extract::Sequence sequence;
		std::vector<Extract::UnitCard> summonableUnits;
	};

	// Info passed for targetting purposes 
	struct targettingInfo {
		Extract::Unit::Ability& source;
		std::vector<std::pair<int, int>> targets;
		int focusedTarget = 0;
		std::pair<int, int> lastTarget = std::make_pair(-1,-1);


		targettingInfo(Extract::Unit::Ability& source) : source(source) {}
	};
}


struct MoveCoor {
	int cost = 0;
	std::pair<int, int> coor;

	MoveCoor(std::pair<int, int> coor, int cost) : coor(coor), cost(cost) {}

	struct setComp {
		bool operator()(const MoveCoor& l, const MoveCoor& r) const { return l.coor < r.coor; }
	};

	struct pqComp {
		bool operator()(const MoveCoor& l, const MoveCoor& r) const { return l.cost > r.cost; }
	};
};