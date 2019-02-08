#include <iostream>

#include "kitten\K_Game.h"
#include "kitten\K_Common.h"

#include "kitten\K_Instance.h"
#include "puppy\P_Instance.h"
#include "kitten\audio\AudioEngineWrapper.h"

#include "kitten\K_GameObject.h"

#include "puppy\Text\TextBox.h"

#include "kitten\Camera.h"
#include "kitten\CubeRenderable.h"
#include "kitten\QuadRenderable.h"
#include "_Project\MoveByMouseRightClickDrag.h"
#include "kitten\mouse picking\ClickableBox.h"

#include "gameworld\GameBoard.h"

#include "kibble/kibble.hpp"
#include "kibble/databank/databank.hpp"
#include "unit/UnitTest.h"

//board
#include "board/clickable/PrintWhenClicked.h"
#include "board/BoardManager.h"
#include "board/tile/landInfo/LandInfoManager.h"

// Only for testing the event system
#include "kitten\event_system\EventExample.h"

#include "ui\InterfaceBuilder.h"

//ui testing
#include "ui/CardUIO.h"
#include "ui/HandFrame.h"
#include "kitten/InputManager.h"
#include "networking\menu\NetworkJoinButton.h"

//ui clickable testing
#include "kitten/mouse picking/ClickableUI.h"
#include "kitten/mouse picking/ClickableFrame.h"
#include "UI\TabMenu\TabMenu.h"

//unit interaction
#include "unitInteraction/UnitInteractionManager.h"

// Networking
#include "networking\ClientGame.h"
#include "networking\ServerGame.h"

#define DEBUG

namespace kitten
{
	void createSingletons()
	{
		K_Instance::createInstance();

		puppy::P_Instance::createInstance();

		ability::StatusManager::createInstance();

		kibble::initializeKibbleRelatedComponents();
		ability::AbilityManager::createInstance();
		ability::AbilityNodeManager::createInstance();

		BoardManager::createInstance();

		UnitInteractionManager::createInstance();

		LandInfoManager::createInstance();
		
		kibble::loadSpriteSheets("MasterSpriteSheet.json");
	}

	void movedExamples()
	{
		//// Deck Data importing, and exporting
		//// Note that kibble assumes it's a newly generated DeckData, passing on existant DeckData would could deletion error because it's already deleted. 
		//DeckData* data = new DeckData(*kibble::getDeckDataFromId(0)); // Assumes there's already atleast one deck loaded
		//data->cards.push_back({ 9,2 });
		////kibble::addNewDeckData(data); // this line works, comment out or FEAR THE WRATH OF THE EVER GROWING DECK NUMBAHS INFESTING YA SAVES
		//


		//Board Creation Block
		//board creator doesn't done by board manager
		//BoardManager::getInstance()->createBoard();
		

		////Parent Child Block
		////Example of Parent / Children : REMOVE WHEN TESTING DONE OR BUGS NOT BEING FOUND
		//K_GameObject* parentTest = K_GameObjectManager::getInstance()->createNewGameObject();
		//parentTest->getTransform().place(-3, 0, 0);
		//CubeRenderable* renderable = static_cast<CubeRenderable*>(compMan->createComponent("CubeRenderable"));
		//renderable->setTexture("textures/crap/cartoon_cobble.tga");
		//parentTest->addComponent(renderable);

		//K_GameObject* firstChild = K_GameObjectManager::getInstance()->createNewGameObject();
		//firstChild->getTransform().setIgnoreParent(false);
		//firstChild->getTransform().place(3, -1, 0);
		//firstChild->getTransform().setParent(&parentTest->getTransform());
		//renderable = static_cast<CubeRenderable*>(compMan->createComponent("CubeRenderable"));
		//PrintWhenClicked* onClick = static_cast<PrintWhenClicked*>(compMan->createComponent("PrintWhenClicked"));
		//K_Component* box = compMan->createComponent("ClickableBoxBox");
		//onClick->setMessage("First Child! (brick)");
		//renderable->setTexture("textures/crap/cartoon_brick.tga");
		//firstChild->addComponent(renderable);
		//firstChild->addComponent(onClick);
		//firstChild->addComponent(box);

		//K_GameObject* secondChild = K_GameObjectManager::getInstance()->createNewGameObject();
		//secondChild->getTransform().setIgnoreParent(false);
		//secondChild->getTransform().place(-3, -2, 0);
		//secondChild->getTransform().setParent(&firstChild->getTransform());
		//renderable = static_cast<CubeRenderable*>(compMan->createComponent("CubeRenderable"));
		//renderable->setTexture("textures/crap/cartoon_cottage.tga");
		//secondChild->addComponent(renderable);

		//parentTest->getTransform().scaleAbsolute(0.5f, 0.5f, 0.5f);
		//secondChild->getTransform().scaleAbsolute(2.0f, 2.0f, 2.0f);

		//parentTest->getTransform().rotateAbsolute(glm::vec3(0, 45, 0));
		//secondChild->getTransform().rotateAbsolute(glm::vec3(0, -45, 0));


		// Interface Builder Block 
		/*userinterface::InterfaceBuilder* builder = new userinterface::InterfaceBuilder();
		builder->start();
		delete builder;*/

		// test unit Block
		//unit::UnitTest::getInstanceSafe()->test();





		/*  really old UI testing block
		//testing ui frame and textbox
		K_GameObject* go = K_GameObjectManager::getInstance()->createNewGameObject();
		puppy::TextBox* tt = static_cast<puppy::TextBox*>(compMan->createComponent("TextBox"));
		tt->setColor(1, 1, 1);
		tt->setText("Any text");
		go->addComponent(tt);
		go->getTransform().place2D(640+640.0f*0.1,360+360.0f*0.1);

		kitten::K_GameObject* com = kitten::K_GameObjectManager::getInstance()->createNewGameObject();
		K_Component* comFrame = kitten::K_ComponentManager::getInstance()->createComponent("Frame");
		com->addComponent(comFrame);
		//com->addComponent(tt);
		com->getTransform().scale2D(0.15, 0.2);
		com->getTransform().place2D(0.1, 0.1);
		//*/
		
		//UIO TESTING BLOCK
		/*
		K_GameObject* hand = K_GameObjectManager::getInstance()->createNewGameObject();
		K_Component* handFrame = compMan->createComponent("Hand");
		hand->addComponent(handFrame);
		hand->getTransform().scale2D(600.0f, 150.0f);
		hand->getTransform().place2D(50.0, 50.0);
		hand->setEnabled(false);

		for (int x = 0; x < 5; x++)
		{

			K_GameObject* card = K_GameObjectManager::getInstance()->createNewGameObject();
			K_Component* cardObj = compMan->createComponent("Card");
			card->addComponent(cardObj);
			userinterface::CardUIO* cardCasted = static_cast<userinterface::CardUIO*>(cardObj);
			cardCasted->scaleAsCard();
			glm::vec3 cardScale = card->getTransform().getScale();

			userinterface::HandFrame* frameCasted = static_cast<userinterface::HandFrame*>(handFrame);
			frameCasted->addCardToEnd(cardCasted);
			cardCasted->assignParentHand(frameCasted);

			K_Component* cardCF = compMan->createComponent("ClickableFrame");
			K_Component* clickUI = compMan->createComponent("ClickableUI");

			card->addComponent(cardCF);
			card->addComponent(clickUI);

			ClickableFrame* cfCasted = static_cast<ClickableFrame*>(cardCF);
			ClickableUI* cUI = static_cast<ClickableUI*>(clickUI);
			cfCasted->addCLickable(cUI);

			cardCF->start();
			clickUI->start();


		}
		*/

	

	}

	// This is called once at the beginning of the game
	bool initGame()
	{
		createSingletons();
		input::InputManager::getInstance()->resetMouse(false);

		// Temporary stuff until Kibble is ready
		K_ComponentManager* compMan = K_ComponentManager::getInstance();

		//Creating a gameobject
		//K_GameObject* camGameObj = K_GameObjectManager::getInstance()->createNewGameObject(std::string("camgameobj.txt"));
		kibble::setSceneFrom("bootstrap.json");

		// Networking
		//networking::ServerGame::createInstance();
		//_beginthread(serverLoop, 0, (void*)12);
		//networking::ClientGame::createInstance();
		//K_GameObject* networkingMenu = K_GameObjectManager::getInstance()->createNewGameObject();
		//networkingMenu->addComponent(kitten::K_ComponentManager::getInstance()->createComponent("NetworkingConsoleMenu"));

		//K_GameObject* spawnUnitOnKeyPress = K_GameObjectManager::getInstance()->createNewGameObject();
		//spawnUnitOnKeyPress->addComponent(kitten::K_ComponentManager::getInstance()->createComponent("SpawnUnitOnKeyPress"));

		//// Tab Menu
		//K_GameObject* tabMenu = K_GameObjectManager::getInstance()->createNewGameObject();
		//TabMenu* tabMenuFrame = static_cast<TabMenu*>(compMan->createComponent("TabMenu")); // Create parent frame
		//tabMenu->addComponent(tabMenuFrame);
		//tabMenu->getTransform().scale2D(1280, 720);
		//tabMenu->getTransform().place2D(0, 0);



		return true;
	}

	void destroySingletons()
	{
		kibble::destroyKibbleRelatedComponents();

		K_Instance::destroyInstance();

		puppy::P_Instance::destroyInstance();

		ability::StatusManager::destroyInstance();
		ability::AbilityManager::destroyInstance();
		ability::AbilityNodeManager::destroyInstance();

		// move to gameplay-init component
		//unit::InitiativeTracker::destroyInstance();
		BoardManager::destroyInstance();
		UnitInteractionManager::destroyInstance();

		LandInfoManager::destroyInstance();

		if (networking::ClientGame::getInstance())
		{
			networking::ClientGame::destroyInstance();
		}

		if (networking::ServerGame::getInstance())
		{
			networking::ServerGame::destroyInstance();
		}
	}

	void updateGame()
	{
		K_Instance::update();
	}

	void renderGame()
	{
		//@TODO: Combine these? 
		puppy::P_Instance::render();
	}

	// This is called every frame
	void gameCycle()
	{
		updateGame();
		renderGame();
	}

	void shutdownGame()
	{
		kitten::K_GameObjectManager::getInstance()->destroyAllGameObjects();
		destroySingletons();

		glfwTerminate();
		exit(EXIT_SUCCESS);
	}
}