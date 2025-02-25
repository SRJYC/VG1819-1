#include "Action.h"
#include <iostream>
#include "AI/controller.h"

void AI::Extract::Sequence::run(unit::Unit * p_unit) {
	if (!p_unit->isTurn()) return;
	for (auto action : actions) {
		action->run(p_unit);
	}
	if (p_unit->canAct()) p_unit->actDone();
	if (p_unit->canMove()) p_unit->moveDone();
}

void AI::Extract::Sequence::step(unit::Unit * p_unit)
{
	if (!p_unit->isTurn()) {
		this->complete = true;
		return;
	}
	if (this->cStep < actions.size()) {
		actions[cStep++]->run(p_unit);
	}
	else {
		if (p_unit->canAct()) p_unit->actDone();
		if (p_unit->canMove()) p_unit->moveDone();
		this->complete = true;
	}
}

AI::Extract::Sequence::Sequence() {
	actions = std::vector<std::shared_ptr<Action>>();
}

AI::Extract::Sequence::~Sequence() {
	actions.clear();
}

void AI::Extract::Move::run(unit::Unit * p_unit) {
	std::cout << "Move to x=" + std::to_string(targetX) + " y=" + std::to_string(targetX) + "\n";
	Board& board = controller::getAIModel(p_unit->m_clientId)->board;
	if (p_unit->canMove()) {
		p_unit->move();
		BoardManager::getInstance()->autoClick(&board.tile[targetX][targetY]->getGameObject());
	}
}

void AI::Extract::ManipulateTile::run(unit::Unit * p_unit) {
	std::cout << "Manipulate tile on x=" + std::to_string(targetX) + " y=" + std::to_string(targetX) + "\n";
	Board& board = controller::getAIModel(p_unit->m_clientId)->board;
	PowerTracker& powerTracker = controller::getAIModel(p_unit->m_clientId)->powerTracker;
	p_unit->useAbility(ABILITY_MANIPULATE_TILE);
	BoardManager::getInstance()->autoClick(&board.tile[targetX][targetY]->getGameObject());
	// update tracker
	powerTracker.increaseMaxPower(1);
}

void AI::Extract::Summon::run(unit::Unit * p_unit) {
	// Setup needed references
	Board& board = controller::getAIModel(p_unit->m_clientId)->board;
	PowerTracker& powerTracker = controller::getAIModel(p_unit->m_clientId)->powerTracker;

	//	take out card from hand 
	unit::Unit* unit = controller::getAIModel(p_unit->m_clientId)->hand.takeOutCard(targetToSummon - handOffset);

	std::cout << "Summon " + unit->m_name + " on tile x=" + std::to_string(targetX) + " y=" + std::to_string(targetX) + "\n";

	// update tracker
	powerTracker.useCurrentPower(unit->m_attributes[UNIT_COST]);

	//send summon event
	kitten::Event* e = new kitten::Event(kitten::Event::Summon_Unit);
	kitten::K_GameObject* g = kitten::K_GameObjectManager::getInstance()->createNewGameObject();
	g->addComponent(unit);
	e->putGameObj(UNIT, g);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Summon_Unit, e);

	// Click where we need to click
	BoardManager::getInstance()->autoClick(&board.tile[targetX][targetY]->getGameObject());
	
	// destroy that card, we don't need it anymore
	kitten::K_GameObjectManager::getInstance()->destroyGameObject(g);
}

void AI::Extract::Ability::run(unit::Unit * p_unit) {
	std::cout << "Ability run \n";
	Board& board = controller::getAIModel(p_unit->m_clientId)->board;
	p_unit->useAbility(abilityName);
	BoardManager::getInstance()->autoClick(&board.tile[targetX][targetY]->getGameObject());
}

void AI::Extract::MultiTargetAbility::run(unit::Unit * p_unit)
{
	std::cout << "Ability " + abilityName + " run \n";
	Board& board = controller::getAIModel(p_unit->m_clientId)->board;
	p_unit->useAbility(abilityName);

	for (int i = 0; i < targetPositions.size();i++) {
		std::cout << "Used  on tile x=" + std::to_string(targetPositions[i].first) + " y=" + std::to_string(targetPositions[i].second) + "\n";
		BoardManager::getInstance()->autoClick(&board.tile[targetPositions[i].first][targetPositions[i].second]->getGameObject());
	}

}

void AI::Extract::Join::run(unit::Unit * p_unit)
{
	std::cout << "Ability run \n";
	Board& board = controller::getAIModel(p_unit->m_clientId)->board;
	p_unit->join();
	BoardManager::getInstance()->autoClick(&board.tile[targetX][targetY]->getGameObject());
}
