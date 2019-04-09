
#include "controller.h"
#include "unit/InitiativeTracker/InitiativeTracker.h"
#include "unit/Unit.h"
#include "kibble/databank/databank.hpp"
#include "Extract/Action.h"
#include <set>
#include <queue>
#include "ability/AbilityMacro.h"
#include "unit/UnitSpawn.h"
#include "kitten/K_GameObject.h"
#include "unit/unitComponent/UnitMove.h"
#include "_Project/DisableAfterTime.h"
#include "networking/ClientGame.h"
#include "AI/Extract/Behavior.h"
#include "AI/Extract/Action.h"

template <typename Iterator>
inline bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
	/* Credits: Thomas Draper */
	// http://stackoverflow.com/a/5097100/8747
	if ((first == last) || (first == k) || (last == k))
		return false;
	Iterator itr1 = first;
	Iterator itr2 = last;
	++itr1;
	if (last == itr1)
		return false;
	itr1 = last;
	--itr1;
	itr1 = k;
	--itr2;
	while (first != itr1)
	{
		if (*--itr1 < *itr2)
		{
			Iterator j = k;
			while (!(*itr1 < *j)) ++j;
			std::iter_swap(itr1, j);
			++itr1;
			++j;
			itr2 = k;
			std::rotate(itr1, j, last);
			while (last != j)
			{
				++j;
				++itr2;
			}
			std::rotate(k, itr2, last);
			return true;
		}
	}
	std::rotate(first, k, last);
	return false;
}

namespace AI {
	std::map<int,controller*> AIcontrollers;
	std::vector<controller*> AIcontrollerlist;
	NearestEnemy defaultBehavior;

	controller::controller() { // TODO make an ID dispensor, i hate this already. 
		setupEventListeners();
		AIcontrollerlist.push_back(this);
	}

	controller::~controller() {
		tearDownEventListeners();
		AIcontrollerlist.erase(std::find(AIcontrollerlist.begin(), AIcontrollerlist.end(), this));
		AIcontrollers.erase(this->m_playerID);

	}

	void controller::runTurn(unit::Unit* p_unit)
	{
		// Disable the previous timer
		this->m_attachedObject->setEnabled(false);

		// Check if it's a commander poiting out this controller's start of a new round
		if (p_unit->isCommander()) {
			// Reset PowerTracker
			m_model.powerTracker.resetCurrent();

			// Draw Cards
			for (int i = 0; i < 2 && m_model.hand.canAddCard(); ++i) {
				Extract::Deck::InteractionState draw = m_model.deck.drawTop();
				if (draw.status != AI::Extract::Deck::Status::OK) break;
				unit::Unit* unit = new unit::Unit(kibble::getUnitFromId(draw.card)); // IMPORTANT!!!!!!!!! REMEMBER TO DELETE WHEN YOU TAKE IT OUT
				unit->m_clientId = this->m_playerID;
				m_model.hand.addCard(unit);
			}
		}
		m_model.board.refreshBoards();

		// Setup retained info
		retainedInfo rInfo(p_unit, m_model);
		setUnitLists(rInfo);

		// Setup initial passed info
		passedInfo pInfo;
		pInfo.availableEnergy = m_model.powerTracker.m_iCurrentPower;
		pInfo.curPos = p_unit->getTile()->getComponent<TileInfo>()->getPos();
		pInfo.summonableUnits = getSummonableCards();

		// Run generator of possible sequences
		generateSequences(rInfo, pInfo);

		// Sort Sequences and pick one
		// TODO bother looking for one based on externally set choices instead of the best.
		std::sort(rInfo.generatedSequences.begin(), rInfo.generatedSequences.end(), Extract::Sequence::weightComp());
		m_unit = p_unit;
		if (rInfo.generatedSequences.size() > 0) {
			m_sequence = rInfo.generatedSequences[0];
		}
		else
		{
			m_sequence = Extract::Sequence();
		}
		kitten::EventManager::getInstance()->queueEvent(kitten::Event::Action_Complete, new kitten::Event(kitten::Event::Action_Complete));
	}

	void controller::start() {
	}

	void controller::setupAIControllers()
	{
		// Alter the client Id to reflect non AI, player controlled opponent. 
		// They'll always set Id to 0 when theres an AI ;
		int startingId = networking::ClientGame::getClientId();

		// Setup related Abilities of extracted units
		Extract::setupUniqueAbilities();

		for (controller* controller: AIcontrollerlist) {
			controller->m_playerID = ++startingId;
			controller->m_model.playerId = controller->m_playerID;
			AIcontrollers[controller->m_playerID] = controller;

			// TODO work on how decks are picked, for now MUH OH SEE
			// Set up deck the AI will use

			int randomDeckID = (std::uniform_real_distribution<long double>()(controller->m_model.deck.RNGZUZ)* kibble::getAIDeckDataListCount());
			controller->m_model.deck.setDeckSource(kibble::getAIDeckDataFromId(randomDeckID));
			controller->m_model.deck.setupDeck();

			// Set up hand
			while (controller->m_model.hand.canAddCard()) {
				Extract::Deck::InteractionState draw = controller->m_model.deck.drawTop();
				if (draw.status != AI::Extract::Deck::Status::OK) break;
				unit::Unit* unit = new unit::Unit(kibble::getUnitFromId(draw.card));
				unit->m_clientId = controller->m_playerID;
				controller->m_model.hand.addCard(unit);
			}

			// Spawn Commander
			kitten::K_GameObject* unitGO = unit::UnitSpawn::getInstance()->spawnUnitObject(controller->m_model.deck.m_deckSource->commanderID);
			unitGO->getComponent<unit::UnitMove>()->setTile(BoardManager::getInstance()->getSpawnPoint(controller->m_playerID ));
			unitGO->getComponent<unit::Unit>()->m_clientId = controller->m_playerID;

			// Set up board reference
			controller->m_model.board.setupBoard();

		}
	}

	std::vector<std::pair<int, int>> controller::getTargetsInRange(const retainedInfo & p_retainedInfo, const passedInfo & p_passedInfo, const targettingInfo& p_targgetingInfo)
	{
		std::vector<std::pair<int, int>> tilePos;
		int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
		std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();
		Extract::Board& board = m_model.board;
		Extract::Unit::Filter& filter = p_targgetingInfo.source.filter;

		for (int i = 1; i <= p_targgetingInfo.source.maxRange; ++i) {
			for (int j = 0; j <= p_targgetingInfo.source.maxRange - i; ++j) {
				if (i + j < p_targgetingInfo.source.minRange) continue;
				for (int k = 0; k < 4; ++k) {
					std::pair<int, int> temp = std::make_pair(
						p_passedInfo.curPos.first + sides[k][0] * i + sides[k][1] * j,
						p_passedInfo.curPos.second + sides[k][1] * i + sides[k][0] * j
					);
					if (// Check if Coordinates are valid
						temp.first >= 0 && temp.first < boardDimensions.first
						&& temp.second >= 0 && temp.second < boardDimensions.second
						// Now that the coordinates are valid, start filter checks
						&& (!filter.tilesOwnedByAny || board.tile[temp.first][temp.second]->getOwnerId() < 0)
						&& (!filter.tilesOwnedByTeam || board.tile[temp.first][temp.second]->getOwnerId() != p_retainedInfo.source.clientId)
						&& (!filter.tilesNotOwnedByTeam || board.tile[temp.first][temp.second]->getOwnerId() == p_retainedInfo.source.clientId)
						&& (!filter.occupiedTiles || (!board.tile[temp.first][temp.second]->hasUnit()
							&& std::find(p_passedInfo.blockedPos.begin(), p_passedInfo.blockedPos.end(), temp) == p_passedInfo.blockedPos.end()))
						&& (!filter.unoccupiedTiles || (board.tile[temp.first][temp.second]->hasUnit() && board.unit[temp.first][temp.second] != p_retainedInfo.source.source)
								|| std::find(p_passedInfo.blockedPos.begin(), p_passedInfo.blockedPos.end(), temp) != p_passedInfo.blockedPos.end())
						&& (!filter.water || board.tile[temp.first][temp.second]->getType() != LandInformation::Water_land) // uber frog filter salesman
						&& (!filter.enemies || board.unit[temp.first][temp.second] == nullptr || board.unit[temp.first][temp.second]->m_clientId == p_retainedInfo.source.clientId)
						&& (!filter.allies || board.unit[temp.first][temp.second] == nullptr || board.unit[temp.first][temp.second]->m_clientId != p_retainedInfo.source.clientId)
						&& (!filter.sameUnitKind || board.unit[temp.first][temp.second] == nullptr || board.unit[temp.first][temp.second]->m_kibbleID != p_retainedInfo.source.kibbleId)
						&& (!filter.differentUnitKind || board.unit[temp.first][temp.second] == nullptr || board.unit[temp.first][temp.second]->m_kibbleID == p_retainedInfo.source.kibbleId)
						)
					{
						tilePos.push_back(temp);
					}
				}
			}
		}

		if (p_targgetingInfo.source.minRange == 0) 
			tilePos.push_back(p_passedInfo.curPos);

		return tilePos;
	}

	std::vector<AI::Extract::Move> controller::getAvailableMoves(const retainedInfo & p_retainedInfo, const passedInfo & p_passedInfo, const targettingInfo& p_targgetingInfo) {
		int initialMovement = p_retainedInfo.source.mv;
		std::pair<int, int> boardDimensions = BoardManager::getInstance()->getDimension();
		std::set<MoveCoor, MoveCoor::setComp> movesVisited;
		std::priority_queue<MoveCoor, std::vector<MoveCoor>, MoveCoor::pqComp> movesToCheck;
		int sides[4][2] = { {1,0},{0,1},{-1,0},{0,-1} };
		movesToCheck.push(MoveCoor(p_passedInfo.curPos, 0));
		movesVisited.insert(movesToCheck.top());
		Extract::Board& board = m_model.board;

		while (movesToCheck.size() > 0)
		{
			MoveCoor temp = movesToCheck.top();
			movesToCheck.pop();
			for (int i = 0; i < 4; i++)
			{
				auto tempPos = std::make_pair(sides[i][0] + temp.coor.first, sides[i][1] + temp.coor.second);
				int tempCost;

				if (// Check if Coordinates are valid
					tempPos.first >= 0 && tempPos.first < boardDimensions.first
					&& tempPos.second >= 0 && tempPos.second < boardDimensions.second
					// Check if it's already occupied by something
					&& !board.tile[tempPos.first][tempPos.second]->hasUnit()
					// Check if movable to by comparing sum of cost to total allowed
					&& (tempCost = temp.cost + board.tile[tempPos.first][tempPos.second]->getMVCost()) <= MAX(initialMovement, 1)
					// Check if blocked by something done already
					&& std::find(p_passedInfo.blockedPos.begin(), p_passedInfo.blockedPos.end(), tempPos) == p_passedInfo.blockedPos.end()
					) {
					MoveCoor newMove(tempPos, tempCost);
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
			if (p_passedInfo.curPos != it.coor)
				moves.push_back(Extract::Move(it.coor.first, it.coor.second));
		}

		return moves;
	}

	std::vector<Extract::UnitCard> controller::getSummonableCards()
	{
		std::vector<Extract::UnitCard> summonableCards;
		for (auto i = 0; i < m_model.hand.m_cards.size(); ++i) {
			if (m_model.powerTracker.checkPowerAmountUsable(m_model.hand.m_cards[i]->m_attributes[UNIT_COST]-1)) { // This is to allow commander who can manipulate tile to do so.
				summonableCards.push_back(Extract::UnitCard(m_model.hand.m_cards[i],i));
			}
		}
		return summonableCards;
	}

	void controller::setUnitLists(retainedInfo & rInfo)
	{
		std::vector<unit::Unit*> units;
		for (kitten::K_GameObject* unit : unit::InitiativeTracker::getInstance()->getUnitList()) {
			unit::Unit* temp = unit->getComponent<unit::Unit>();
			rInfo.allUnits.push_back(temp);
			if (temp->m_clientId == rInfo.source.clientId)
				rInfo.allyUnits.push_back(temp);
			else
				rInfo.enemyUnits.push_back(temp);
		}
	}

	void controller::generateSequences(retainedInfo & p_retainedInfo, passedInfo & p_passedInfo)
	{
		if (p_passedInfo.canMove) {

			// give the movement check a max value of  1
			// TODO take into account the various fields he's currently on.

			targettingInfo targetInfo = targettingInfo(Extract::move);

			for (auto move : getAvailableMoves(p_retainedInfo,p_passedInfo,targetInfo)) {
				targetInfo.targets = std::vector<std::pair<int, int>>(1, std::make_pair(move.targetX, move.targetY));
				passedInfo info(p_passedInfo);
				info.canMove = false;
				info.curPos = targetInfo.targets.back();
				info.sequence = Extract::Sequence(p_passedInfo.sequence);

				if (p_retainedInfo.source.source->m_AbilityBehavior.find(UNIT_MV) != p_retainedInfo.source.source->m_AbilityBehavior.end()) {
					info.sequence.weight += p_retainedInfo.source.source->m_AbilityBehavior[UNIT_MV]->calculateWeight(p_retainedInfo, info, targetInfo);
				}
				else {
					info.sequence.weight += defaultBehavior.calculateWeight(p_retainedInfo, info, targetInfo);
				}
				info.sequence.actions.push_back(
					std::make_shared<Extract::Move>(move)
				);
				p_retainedInfo.generatedSequences.push_back(info.sequence);

				generateSequences(p_retainedInfo, info);
			}
		}

		if (p_passedInfo.canAct) {
			// Logic for abilities
			for (auto ability : p_retainedInfo.source.ability) {				
				// TODO let the target max and min be controlled through datadriving 
				// give the damage a value boost of difference it does to enemy, which can be a lot. 

				// Give this weight based on closeness to lowest health targets. 

				// this is just for me to understand how to check for each different type of ability. 
				// this will be stripped down to essentials based on what ability information we get. 
				// TODO add general unit preferences, and ability preferences that override the general. these should affect the target and move

				std::vector<std::pair<int, int>> possibleTargets;
				targettingInfo targetInfo = targettingInfo(ability);

				// Now go through the list of target units available and pickout the ones we can target.
				for (auto pos : getTargetsInRange(p_retainedInfo, p_passedInfo, targetInfo)) {
					if (
						(ability.filter.unoccupiedTiles && m_model.board.unit[pos.first][pos.second]->m_attributes[UNIT_HP] <= 0) // if no HP left
						)
						continue;

					int numberOfRepetitionsPossibleOnATarget = 1;
					unit::Unit* target = m_model.board.unit[pos.first][pos.second];
					if (ability.selectRepeat) // if the targets can be repeatedly selected
						numberOfRepetitionsPossibleOnATarget = MAX(ability.targets, 1);
					possibleTargets.resize(possibleTargets.size() + numberOfRepetitionsPossibleOnATarget, pos);
				}

				if (possibleTargets.size() == 0
					|| (!ability.selectRepeat && possibleTargets.size() < ability.targets)
					) continue;

				// If the number of possible targets end up being less than the target needed, repeat the last one over again. 
				// Due to the earlier repeat check, this will always get triggered only if the target can be repeatedly targetted.
				if (possibleTargets.size() < ability.targets)
				{
					possibleTargets.resize(ability.targets, possibleTargets.back());
				}

				std::sort(possibleTargets.begin(), possibleTargets.end());
				do {
					targetInfo.targets = std::vector<std::pair<int, int>>(possibleTargets.begin(), possibleTargets.begin() + ability.targets);
					passedInfo info(p_passedInfo);
					info.canAct = false;
					info.sequence = Extract::Sequence(p_passedInfo.sequence);

					if (p_retainedInfo.source.source->m_AbilityBehavior.find(ability.name) != p_retainedInfo.source.source->m_AbilityBehavior.end()) {
						info.sequence.weight += p_retainedInfo.source.source->m_AbilityBehavior[ability.name]->calculateWeight(p_retainedInfo, info, targetInfo);
					}
					else {
						info.sequence.weight += defaultBehavior.calculateWeight(p_retainedInfo, info, targetInfo);
					}
					info.sequence.actions.push_back(
						std::make_shared<Extract::MultiTargetAbility>(Extract::MultiTargetAbility(targetInfo.targets, ability.name))
					);
					p_retainedInfo.generatedSequences.push_back(info.sequence);

					generateSequences(p_retainedInfo, info);

				} while (next_combination(possibleTargets.begin(), possibleTargets.begin() + ability.targets, possibleTargets.end()));
			}

			if (p_retainedInfo.source.isCommander) {
				// Logic for Tile Manipulation
				{
					targettingInfo targetInfo = targettingInfo(Extract::manipulateTile);

					for (auto tile : getTargetsInRange(p_retainedInfo,p_passedInfo,targetInfo)) {
						targetInfo.targets = std::vector<std::pair<int, int>>(1, tile);
						passedInfo info(p_passedInfo);
						info.canAct = false;
						info.availableEnergy++;
						info.sequence = Extract::Sequence(p_passedInfo.sequence);

						if (p_retainedInfo.source.source->m_AbilityBehavior.find(ABILITY_MANIPULATE_TILE) != p_retainedInfo.source.source->m_AbilityBehavior.end()) {
							info.sequence.weight += p_retainedInfo.source.source->m_AbilityBehavior[ABILITY_MANIPULATE_TILE]->calculateWeight(p_retainedInfo, info, targetInfo);
						}
						else {
							info.sequence.weight += defaultBehavior.calculateWeight(p_retainedInfo, info, targetInfo);
						}
						info.sequence.actions.push_back(
							std::make_shared<Extract::ManipulateTile>(Extract::ManipulateTile(tile.first, tile.second))
						);
						p_retainedInfo.generatedSequences.push_back(info.sequence);

						generateSequences(p_retainedInfo, info);
					}
				}

				// Logic for Summoning
				if (p_passedInfo.summoned < 2) { // Limiting the AI to only summoning 2 from his hand for now
					for (int i = 0; i < p_passedInfo.summonableUnits.size();++i) { // Go through all summonable units
						if (p_passedInfo.availableEnergy < p_passedInfo.summonableUnits[i].cost)
							continue;

						targettingInfo targetInfo = targettingInfo(Extract::summon);

						for (auto target : getTargetsInRange(p_retainedInfo,p_passedInfo,targetInfo)) {
							targetInfo.targets = std::vector<std::pair<int, int>>(1, target);

							passedInfo info(p_passedInfo);
							info.blockedPos.push_back(target);
							info.summonableUnits.erase(info.summonableUnits.begin() + i);
							info.availableEnergy -= p_passedInfo.summonableUnits[i].cost;
							++info.summoned;
							info.handCardsPicked.push_back(p_passedInfo.summonableUnits[i].handIndex);

							int handOffset = 0; // TODO this is bothering me, it JUST WORKS but find a better way later
							for (auto handCardPicked : p_passedInfo.handCardsPicked)
								if (handCardPicked < p_passedInfo.summonableUnits[i].handIndex)
									++handOffset;

							info.sequence = Extract::Sequence(p_passedInfo.sequence);
							info.sequence.actions.push_back(
								std::make_shared<Extract::Summon>(Extract::Summon(target.first, target.second, p_passedInfo.summonableUnits[i].handIndex, handOffset))
							);

							
							// TODO change Weight Strategy
							if (p_retainedInfo.source.source->m_AbilityBehavior.find(ABILITY_SUMMON_UNIT) != p_retainedInfo.source.source->m_AbilityBehavior.end()) {
								info.sequence.weight += p_retainedInfo.source.source->m_AbilityBehavior[ABILITY_SUMMON_UNIT]->calculateWeight(p_retainedInfo, info, targetInfo);
							}
							else {
								info.sequence.weight += defaultBehavior.calculateWeight(p_retainedInfo, info, targetInfo);
							}

							p_retainedInfo.generatedSequences.push_back(info.sequence);

							generateSequences(p_retainedInfo, info);
						}
					}
				}
			}
			else {
				// Logic for Join
				targettingInfo targetInfo = targettingInfo(Extract::join);

				for (auto unit : getTargetsInRange(p_retainedInfo, p_passedInfo, targetInfo)) {
					if ( p_retainedInfo.source.lv >= 3 ||
						p_retainedInfo.board.unit[unit.first][unit.second]->m_attributes[UNIT_LV] != p_retainedInfo.source.lv) continue;
					targetInfo.targets = std::vector<std::pair<int, int>>(1, unit);
					passedInfo info(p_passedInfo);
					info.canAct = false;
					info.sequence = Extract::Sequence(p_passedInfo.sequence);

					if (p_retainedInfo.source.source->m_AbilityBehavior.find(ACTION_JOIN) != p_retainedInfo.source.source->m_AbilityBehavior.end()) {
						info.sequence.weight += p_retainedInfo.source.source->m_AbilityBehavior[ACTION_JOIN]->calculateWeight(p_retainedInfo, info, targetInfo);
					}
					else {
						info.sequence.weight += defaultBehavior.calculateWeight(p_retainedInfo, info, targetInfo);
					}
					info.sequence.actions.push_back(
						std::make_shared<Extract::Join>(Extract::Join(unit.first, unit.second))
					);
					p_retainedInfo.generatedSequences.push_back(info.sequence);

					generateSequences(p_retainedInfo, info);
				}
			}
		}
	}

	void controller::nextActionInSequenceHandler(kitten::Event::EventType p_type, kitten::Event * p_data)
	{
		unit::Unit* currentUnit = unit::InitiativeTracker::getInstance()->getCurrentUnit()->getComponent<unit::Unit>();
		if (currentUnit->m_clientId != m_playerID || !m_unit->isTurn()) return;
		this->m_attachedObject->getComponent<DisableAfterTime>()->setTime(1);
		if (!m_attachedObject->isEnabled())
			this->m_attachedObject->setEnabled(true);
		else
			this->m_attachedObject->setEnabled(false);
	}

	void controller::nextTurnHandler(kitten::Event::EventType p_type, kitten::Event* p_data) {
		unit::Unit* currentUnit = unit::InitiativeTracker::getInstance()->getCurrentUnit()->getComponent<unit::Unit>();
		if (currentUnit->m_clientId == m_playerID) {
			runTurn(currentUnit);
		}
	}

	controller * controller::getAIController(int p_playerId)
	{
		return AIcontrollers[p_playerId];
	}

	int controller::getAIControllerSize()
	{
		return AIcontrollerlist.size();
	}

	Model * controller::getAIModel(int p_playerId)
	{
		return &AIcontrollers[p_playerId]->m_model;
	}

	bool controller::AIPresent()
	{
		return AIcontrollerlist.size()> 0;
	}

	void controller::onDisabled()
	{
		if (m_unit != nullptr && m_unit == unit::InitiativeTracker::getInstance()->getCurrentUnit()->getComponent<unit::Unit>()) {
			m_sequence.step(m_unit);
		}
	}

	void controller::setupEventListeners() {
		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Next_Units_Turn_Start,
			this,
			std::bind(&controller::nextTurnHandler, this, std::placeholders::_1, std::placeholders::_2));
		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Action_Complete,
			this,
			std::bind(&controller::nextActionInSequenceHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	void controller::tearDownEventListeners() {

		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Next_Units_Turn_Start, this);
		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Action_Complete, this);
	}
}