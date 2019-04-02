#include "model.h"
#include <set>
#include <queue>

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
std::vector<AI::Extract::Move> AI::Model::getAvailableMoves(unit::Unit* p_unit) {
	int initialMovement = p_unit->m_attributes[UNIT_MV];
	std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();
	std::set<MoveCoor, MoveCoor::setComp> movesVisited;
	std::priority_queue<MoveCoor, std::vector<MoveCoor>, MoveCoor::pqComp> movesToCheck;
	int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
	movesToCheck.push(MoveCoor(p_unit->getTile()->getComponent<TileInfo>()->getPos(), 0));
	movesVisited.insert(movesToCheck.top());

	while (movesToCheck.size() > 0)
	{
		MoveCoor temp = movesToCheck.top();
		movesToCheck.pop();
		for (int i = 0; i < 4; i++)
		{
			int tempX = sides[i][0] + temp.coor.first, tempY = sides[i][1] + temp.coor.second,
				tempCost;

			if (// Check if Coordinates are valid
				tempX >= 0 && tempX < boardDimensions.first
				&& tempY >= 0 && tempY < boardDimensions.second
				// Check if it's already occupied by something
				&& !board.board[tempX][tempY]->hasUnit()
				// Check if movable to by comparing sum of cost to total allowed
				&& (tempCost = temp.cost + board.board[tempX][tempY]->getMVCost()) <= initialMovement
				) {
				MoveCoor newMove(std::pair<int, int>(tempX, tempY), tempCost);
				// Check if we already visited this node
				auto moveIt = movesVisited.find(newMove);
				if (moveIt != movesVisited.end()) {
					// Check if current costs less than previous venture
					if (tempCost < moveIt->cost) {
						// update the cost and send it to check for it's children that may have better costs
						movesVisited.erase(moveIt);
						movesVisited.insert(newMove);
						movesToCheck.push(newMove);
					}
					//else we don't care about the move
				}
				else {
					//We add it as a visited node and check it
					movesVisited.insert(newMove);
					movesToCheck.push(newMove);
				}
			}
		}
	}

	// send back that move set
	std::vector<Extract::Move> moves;
	for (auto it : movesVisited) {
		moves.push_back(Extract::Move(it.coor.first, it.coor.second));
	}

	return moves;
}

std::vector<AI::Extract::Move> AI::Model::getAvailableMoves(TargetRange p_Target) {
	int initialMovement = p_Target.unit->m_attributes[UNIT_MV];
	std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();
	std::set<MoveCoor, MoveCoor::setComp> movesVisited;
	std::priority_queue<MoveCoor, std::vector<MoveCoor>, MoveCoor::pqComp> movesToCheck;
	int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
	movesToCheck.push(MoveCoor(p_Target.currentPlacement, 0));
	movesVisited.insert(movesToCheck.top());

	while (movesToCheck.size() > 0)
	{
		MoveCoor temp = movesToCheck.top();
		movesToCheck.pop();
		for (int i = 0; i < 4; i++)
		{
			int tempX = sides[i][0] + temp.coor.first, tempY = sides[i][1] + temp.coor.second,
				tempCost;

			if (// Check if Coordinates are valid
				tempX >= 0 && tempX < boardDimensions.first
				&& tempY >= 0 && tempY < boardDimensions.second
				// Check if it's already occupied by something
				&& !board.board[tempX][tempY]->hasUnit()
				// Check if movable to by comparing sum of cost to total allowed
				&& (tempCost = temp.cost + board.board[tempX][tempY]->getMVCost()) <= MAX(initialMovement,1)
				// Check if blocked by something done already
				&& std::find(p_Target.blockedPos.begin(),p_Target.blockedPos.end(),std::make_pair(tempX,tempY)) == p_Target.blockedPos.end()
				) {
				MoveCoor newMove(std::pair<int, int>(tempX, tempY), tempCost);
				// Check if we already visited this node
				auto moveIt = movesVisited.find(newMove);
				if (moveIt != movesVisited.end()) {
					// Check if current costs less than previous venture
					if (tempCost < moveIt->cost) {
						// update the cost and send it to check for it's children that may have better costs
						movesVisited.erase(moveIt);
						movesVisited.insert(newMove);
						movesToCheck.push(newMove);
					}
					//else we don't care about the move
				}
				else {
					//We add it as a visited node and check it
					movesVisited.insert(newMove);
					movesToCheck.push(newMove);
				}
			}
		}
	}

	// send back that move set
	std::vector<Extract::Move> moves;
	for (auto it : movesVisited) {
		if(p_Target.currentPlacement != it.coor)
			moves.push_back(Extract::Move(it.coor.first, it.coor.second));
	}

	return moves;
}

std::vector<int> AI::Model::getSummonableCards()
{
	std::vector<int> summonableCards;
	for (auto i = 0; i < hand.m_cards.size(); ++i) {
		if (powerTracker.checkPowerAmountUsable(hand.m_cards[i]->m_attributes[UNIT_COST])) {
			summonableCards.push_back(i);
		}
	}
	return summonableCards;
}

std::vector<unit::Unit*> AI::Model::getOtherPlayerUnits() {
	std::vector<unit::Unit*> units;
	for (kitten::K_GameObject* unit : unit::InitiativeTracker::getInstance()->getUnitList()) {
		unit::Unit* temp = unit->getComponent<unit::Unit>();
		if (temp->m_clientId != this->playerId) {
			units.push_back(temp);
		}
	}
	return units;
}

std::vector<unit::Unit*> AI::Model::getOwnUnits() {
	std::vector<unit::Unit*> units;
	for (kitten::K_GameObject* unit : unit::InitiativeTracker::getInstance()->getUnitList()) {
		unit::Unit* temp = unit->getComponent<unit::Unit>();
		if (temp->m_clientId == this->playerId) {
			units.push_back(temp);
		}
	}
	return units;
}

std::vector<unit::Unit*> AI::Model::getTargetsInRange(TargetRange p_Target)
{
	std::vector<unit::Unit*> unitsAround;
	int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
	std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();

	for (int i = 1; i <= p_Target.max_range; ++i) {
		for (int j = 0; j <= p_Target.max_range - i; ++j) {
			if (i + j < p_Target.min_range) continue;
			for (int k = 0; k < 4; ++k) {
				std::pair<int, int> temp = std::make_pair(
					p_Target.currentPlacement.first + sides[k][0] * i + sides[k][1] * j,
					p_Target.currentPlacement.second + sides[k][1] * i + sides[k][0] * j
				);
				if (// Check if Coordinates are valid
					temp.first >= 0 && temp.first < boardDimensions.first
					&& temp.second >= 0 && temp.second < boardDimensions.second
					// Check if inhabitted by units
					&& board.board[temp.first][temp.second]->hasUnit())
				{
					unitsAround.push_back(board.board[temp.first][temp.second]->getUnit()->getComponent<unit::Unit>());
				}
			}
		}
	}

	if (p_Target.min_range == 0) unitsAround.push_back(p_Target.unit);

	return unitsAround;
}

std::vector<TileInfo*> AI::Model::getTargetTilesInRange(TargetRange p_Target)
{
	std::vector<TileInfo*> tiles;
	int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
	std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();

	for (int i = 1; i <= p_Target.max_range; ++i) {
		for (int j = 0; j <= p_Target.max_range - i; ++j) {
			if (i + j < p_Target.min_range) continue;
			for (int k = 0; k < 4; ++k) {
				std::pair<int, int> temp = std::make_pair(
					p_Target.currentPlacement.first + sides[k][0] * i + sides[k][1] * j,
					p_Target.currentPlacement.second + sides[k][1] * i + sides[k][0] * j
				);
				if (// Check if Coordinates are valid
					temp.first >= 0 && temp.first < boardDimensions.first
					&& temp.second >= 0 && temp.second < boardDimensions.second
					)
				{
					tiles.push_back(board.board[temp.first][temp.second]);
				}
			}
		}
	}

	if (p_Target.min_range == 0) tiles.push_back(p_Target.unit->getTile()->getComponent<TileInfo>());

	return tiles;
}

std::vector<TileInfo*> AI::Model::getTargetNonBlockedTilesInRange(TargetRange p_Target)
{
	std::vector<TileInfo*> tiles;
	int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
	std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();

	for (int i = 1; i <= p_Target.max_range; ++i) {
		for (int j = 0; j <= p_Target.max_range - i; ++j) {
			if (i + j < p_Target.min_range) continue;
			for (int k = 0; k < 4; ++k) {
				std::pair<int, int> temp = std::make_pair(
					p_Target.currentPlacement.first + sides[k][0] * i + sides[k][1] * j,
					p_Target.currentPlacement.second + sides[k][1] * i + sides[k][0] * j
				);
				if (// Check if Coordinates are valid
					temp.first >= 0 && temp.first < boardDimensions.first
					&& temp.second >= 0 && temp.second < boardDimensions.second
					// checks if its blocked only if the TargetRange is supplied with blocked positions to check for
					&& std::find(p_Target.blockedPos.begin(), p_Target.blockedPos.end(), temp) == p_Target.blockedPos.end()
					&& !board.board[temp.first][temp.second]->hasUnit() 
					)
				{
					tiles.push_back(board.board[temp.first][temp.second]);
				}
			}
		}
	}

	if (p_Target.min_range == 0) tiles.push_back(p_Target.unit->getTile()->getComponent<TileInfo>());

	return tiles;
}

std::vector<TileInfo*> AI::Model::getTargetOwnedTilesInRange(TargetRange p_Target)
{
	std::vector<TileInfo*> tiles;
	int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
	std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();

	for (int i = 1; i <= p_Target.max_range; ++i) {
		for (int j = 0; j <= p_Target.max_range - i; ++j) {
			if (i + j < p_Target.min_range) continue;
			for (int k = 0; k < 4; ++k) {
				std::pair<int, int> temp = std::make_pair(
					p_Target.currentPlacement.first + sides[k][0] * i + sides[k][1] * j,
					p_Target.currentPlacement.second + sides[k][1] * i + sides[k][0] * j
				);
				if (// Check if Coordinates are valid
					temp.first >= 0 && temp.first < boardDimensions.first
					&& temp.second >= 0 && temp.second < boardDimensions.second
					&& board.board[temp.first][temp.second]->getOwnerId() == p_Target.unit->m_clientId
					// checks if its blocked only if the TargetRange is supplied with blocked positions to check for
					&& std::find(p_Target.blockedPos.begin(), p_Target.blockedPos.end(), temp) == p_Target.blockedPos.end()
					&& !board.board[temp.first][temp.second]->hasUnit()
					)
				{
					tiles.push_back(board.board[temp.first][temp.second]);
				}
			}
		}
	}

	if (p_Target.min_range == 0) tiles.push_back(p_Target.unit->getTile()->getComponent<TileInfo>());

	return tiles;
}

std::vector<TileInfo*> AI::Model::getTargetNotOwnedTilesInRange(TargetRange p_Target)
{
	std::vector<TileInfo*> tiles;
	int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
	std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();

	for (int i = 1; i <= p_Target.max_range; ++i) {
		for (int j = 0; j <= p_Target.max_range - i; ++j) {
			if (i + j < p_Target.min_range) continue;
			for (int k = 0; k < 4; ++k) {
				std::pair<int, int> temp = std::make_pair(
					p_Target.currentPlacement.first + sides[k][0] * i + sides[k][1] * j,
					p_Target.currentPlacement.second + sides[k][1] * i + sides[k][0] * j
				);
				if (// Check if Coordinates are valid
					temp.first >= 0 && temp.first < boardDimensions.first
					&& temp.second >= 0 && temp.second < boardDimensions.second
					&& board.board[temp.first][temp.second]->getOwnerId() != p_Target.unit->m_clientId
					)
				{
					tiles.push_back(board.board[temp.first][temp.second]);
				}
			}
		}
	}

	if (p_Target.min_range == 0) tiles.push_back(p_Target.unit->getTile()->getComponent<TileInfo>());

	return tiles;
}

