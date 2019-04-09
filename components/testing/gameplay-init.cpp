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
#include "unit/UnitSpawn.h"
#include "unit/unitComponent/UnitMove.h"
#include "components/DeckInitializingComponent.h"

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


	
	if (AI::controller::AIPresent()) {
		if (AI::controller::getAIControllerSize() <= 1) {
			networking::ClientGame::setClientId(0);
			if (m_testing) {
				unit::UnitTest::getInstanceSafe()->test();
				DrawCardsFromDeckWithDelay::getActiveInstance()->setCardCountToDispense(5);
				DrawCardsFromDeckWithDelay::getActiveInstance()->addDelayToStart(7);
			}
			else {

				kitten::K_GameObject* playerCommander;
				if (DeckInitializingComponent::getActiveInstance() == nullptr)
					playerCommander = unit::UnitSpawn::getInstance()->spawnUnitObject(14); // queen !!!
				else
					playerCommander = unit::UnitSpawn::getInstance()->spawnUnitObject(DeckInitializingComponent::getActiveInstance()->getDeckData()->commanderID);
				playerCommander->getComponent<unit::UnitMove>()->setTile(BoardManager::getInstance()->getSpawnPoint(0));
				playerCommander->getComponent<unit::Unit>()->m_clientId = 0;
				DrawCardsFromDeckWithDelay::getActiveInstance()->setCardCountToDispense(5);
				DrawCardsFromDeckWithDelay::getActiveInstance()->addDelayToStart(7);
			}
		}
		else {
			networking::ClientGame::setClientId(-1);
		}
		AI::controller::setupAIControllers();
	}
	else {

		if (m_testing) {
			unit::UnitTest::getInstanceSafe()->test();
			DrawCardsFromDeckWithDelay::getActiveInstance()->setCardCountToDispense(5);
			DrawCardsFromDeckWithDelay::getActiveInstance()->addDelayToStart(7);
		}
	}
}