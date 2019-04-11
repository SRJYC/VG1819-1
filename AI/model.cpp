#include "model.h"

namespace AI {
	retainedInfo::retainedInfo(unit::Unit * p_unit, Model& p_model) : source(p_unit),
		hand(p_model.hand), powerTracker(p_model.powerTracker), deck(p_model.deck), board(p_model.board)
	{

	}

}