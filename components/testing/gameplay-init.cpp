#include "gameplay-init.h"
#include "unit/UnitTest.h"
#include "UI/HandFrame.h"
#include "kitten/K_ComponentManager.h"
#include "unitInteraction/UnitInteractionManager.h"
#include "board/BoardManager.h"
#include "components/initializers/DrawCardsFromDeckWithDelay.h"
#include "AI/controller.h"
#include "board/tile/GameMode/GameModeManager.h"
#include "networking/ClientGame.h"

GameplayInit::GameplayInit(bool p_testing)
{ 
	m_testing = p_testing; 
}

GameplayInit::~GameplayInit()
{
	//BoardManager::getInstance()->destroyComponents();
	//UnitInteractionManager::destroyInstance();
	unit::InitiativeTracker::destroyInstance();
	unit::UnitSpawn::destroyInstance();

	GameModeManager::destroyInstance();
}

void GameplayInit::onDisabled()
{
	unit::InitiativeTracker::getInstance()->gameTurnStart();
}

void GameplayInit::start() {
	// TODO put this in a separate component or something. 
	GameModeManager::createInstance();
	BoardManager::getInstance()->resetComponents();
	unit::InitiativeTracker::createInstance();
	unit::UnitSpawn::createInstance();
	//UnitInteractionManager::createInstance();
	UnitInteractionManager::getInstance()->reset();

	if (m_testing) {
		unit::UnitTest::getInstanceSafe()->test();
		DrawCardsFromDeckWithDelay::getActiveInstance()->setCardCountToDispense(5);
		DrawCardsFromDeckWithDelay::getActiveInstance()->addDelayToStart(7);
	}

	if (AI::controller::AIPresent()) {
		if (AI::controller::getAIControllerSize() <= 1) {
			networking::ClientGame::setClientId(0);
		}
		AI::controller::setupAIControllers();
	}
}