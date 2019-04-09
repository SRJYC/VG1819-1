#include "CommanderContext.h"
#include "kitten\K_ComponentManager.h"
#include "kitten\K_GameObjectManager.h"
#include "kitten\K_GameObject.h"
#include "puppy\Text\Font.h"
#include "puppy\Text\FontTable.h"
#include "puppy\Text\TextBox.h"
#include "puppy\Text\TextTable.h"
#include "puppy\Text\PivotTextBox.h"
#include "kibble\kibble.hpp"
#include "kitten\InputManager.h"
#include "unit\Unit.h"
#include "kibble\databank\databank.hpp"
#include "ability\AbilityManager.h"
#include "UI\Borders\BorderPiece.h"
#include <string>
#include <map>
#include "networking/ClientGame.h"

namespace userinterface
{
	//TO DO
	/*
		I need to make this support images more elegantly. I suppose just tossing in a blank frame would work
	*/
	CommanderContext::CommanderContext(): ContextMenu()
	{
		
	}

	CommanderContext::~CommanderContext()
	{
		for (Row* r : m_rows)
		{
			delete r;
		}
		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Client_Commander_Loaded, this);
	}


	void CommanderContext::start()
	{
		ContextMenu::start();
		//network
		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Client_Commander_Loaded,
			this,
			std::bind(&CommanderContext::commanderLoadListener, this, std::placeholders::_1, std::placeholders::_2));
		
		if (!networking::ClientGame::getInstance()->isNetworkValid())
		{
			//REMOVE WHEN NOT TESTING
			attachCommander(kibble::getUnitFromId(29));
			//REMOVE WHEN NOT TESTING
		}
	}

	void CommanderContext::commanderLoadListener(kitten::Event::EventType p_type, kitten::Event* p_event)
	{
		attachCommander(p_event->getGameObj(COMMANDER_GO_KEY)->getComponent<unit::Unit>());
	}

	void CommanderContext::attachCommander(unit::Unit* p_unit)
	{
		m_attachedCommander = p_unit;

		//screen position stuff
		input::InputManager* inMan = input::InputManager::getInstance();
		//game object positions

		//TODO
		//I want to be able to datadrive values like this
		int winX, winY;
		winY = inMan->getWindowHeight();
		winX = inMan->getWindowWidth();
		
		//portrait object
		kitten::K_GameObject* portrait = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/commander_portrait.txt");
		//put portrait on screen
		userinterface::UIElement* portraitComp = portrait->getComponent<userinterface::UIElement>();
		portraitComp->setTexture(m_attachedCommander->getPortraitTexturePath().c_str());
		portrait->getTransform().place2D(0.0f, winY);
		glm::vec3 porTrans = portrait->getTransform().getTranslation();

		//build partial borders
		//bottom
		kitten::K_GameObject* botBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/border_bottom.txt");
		BorderPiece* botBorderComp = botBorder->getComponent<BorderPiece>();
		botBorderComp->setFramedObject(portrait);
		//bottomright
		kitten::K_GameObject* botrightBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/border_bottomright.txt");
		BorderPiece* botrightBorderComp = botrightBorder->getComponent<BorderPiece>();
		botrightBorderComp->setFramedObject(portrait);
		//right
		kitten::K_GameObject* rightBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/border_right.txt");
		BorderPiece* rightBorderComp = rightBorder->getComponent<BorderPiece>();
		rightBorderComp->setFramedObject(portrait);
	

		//TO DO, DATADRIVE THIS POSITIONAL DATA.
		glm::vec3 translate = getTransform().getTranslation();
		getTransform().place(120.0f, winY, -0.05f);
		setPivotType(piv_TopLeft);
		setTexBehaviour(tbh_Repeat);

		//TO DO, DATADRIVE THE PADDING FOR CHANGING OUTSIDE OF CODE
		m_padding = 15;
		static int rowMargin = 5;

		defineVerts();

		//make a GO based on the json textbox data ->
		kitten::K_GameObject* GO_name = kitten::K_GameObjectManager::getInstance()->createNewGameObject("commander_name.txt");
		puppy::TextBox* nameComp = GO_name->getComponent<puppy::TextBox>();
		int nameLength = m_attachedCommander->m_name.length();
		if (nameLength > 8)
			nameComp->setFont(puppy::FontTable::getInstance()->getFont("../fonts/nsimsun_14pt.fnt"));
		nameComp->setText(m_attachedCommander->m_name);
		nameComp->setEnabled(true);

		ctxElement nmEm;
		nmEm.isText = true;
		nmEm.GO = GO_name;

		//new row
		Row* r = addRow(rt_OneElement);
		//PLEASE NOTE. YOU MUST DEFINE THESE VARIABLES.
		r->height = nameComp->getBoxHeight() + 25;
		r->width = winX;
		r->margin = rowMargin;
		r->elements.push_back(nmEm);


		Row* row2 = addRow(rt_Overflow);
		kitten::K_GameObject* hpIcon = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UI/Icon.txt");
		userinterface::UIElement* hpComp = hpIcon->getComponent<userinterface::UIElement>();
		hpComp->setTexture("textures/ui/icons/stat_icons/stat_icons/hp.png");

		ctxElement hpEm;
		hpEm.GO = hpIcon;

		row2->height = 15;
		row2->elements.push_back(hpEm);

		kitten::K_GameObject* hpText = kitten::K_GameObjectManager::getInstance()->createNewGameObject("context_number_3digits.txt");
		puppy::TextBox* hptxtComp = hpText->getComponent<puppy::TextBox>();
		hptxtComp->setText(std::to_string(m_attachedCommander->m_attributes["hp"]));

		ctxElement hptxtEm;
		hptxtEm.GO = hpText;
		hptxtEm.isText = true;

		row2->elements.push_back(hptxtEm);

		kitten::K_GameObject* spIcon = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UI/Icon.txt");
		userinterface::UIElement* spComp = spIcon->getComponent<userinterface::UIElement>();
		spComp->setTexture("textures/ui/icons/stat_icons/stat_icons/movement.png");

		ctxElement spEm;
		spEm.GO = spIcon;

		row2->elements.push_back(spEm);

		kitten::K_GameObject* spText = kitten::K_GameObjectManager::getInstance()->createNewGameObject("context_number_3digits.txt");
		puppy::TextBox* sptxtComp = spText->getComponent<puppy::TextBox>();
		sptxtComp->setText(std::to_string(m_attachedCommander->m_attributes["mv"]));

		ctxElement sptxtEm;
		sptxtEm.GO = spText;
		sptxtEm.isText = true;

		row2->elements.push_back(sptxtEm);

		kitten::K_GameObject* inIcon = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UI/Icon.txt");
		userinterface::UIElement* inComp = inIcon->getComponent<userinterface::UIElement>();
		inComp->setTexture("textures/ui/icons/stat_icons/stat_icons/initiative.png");

		ctxElement inEm;
		inEm.GO = inIcon;

		row2->elements.push_back(inEm);

		kitten::K_GameObject* inText = kitten::K_GameObjectManager::getInstance()->createNewGameObject("context_number_3digits.txt");
		puppy::TextBox* intxtComp = inText->getComponent<puppy::TextBox>();
		intxtComp->setText(std::to_string(m_attachedCommander->m_attributes["in"]));

		ctxElement intxtEm;
		intxtEm.GO = inText;
		intxtEm.isText = true;

		row2->elements.push_back(intxtEm);

		arrange();

		//context bottom border
		//this has to be set up AFTER the arrange() method because that is where the scale of the context is set.
		kitten::K_GameObject* ctxBotBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/border_bottom.txt");
		BorderPiece* ctxBotBorderComp = ctxBotBorder->getComponent<BorderPiece>();
		ctxBotBorderComp->setFramedObject(m_attachedObject);

		//background frame for initiative tracker
		kitten::K_GameObject* backGround = kibble::getGameObjectDataParserInstance()->getGameObject("UI/backgrounds/initiative_background.txt");
		backGround->getTransform().place(300, winY - 8, -0.04);


		//build partial borders
		//top
		kitten::K_GameObject* topBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/gold_border_top.txt");
		userinterface::BorderPiece* topBorderComp = topBorder->getComponent<userinterface::BorderPiece>();
		topBorderComp->setFramedObject(backGround);
		//topeleft
		kitten::K_GameObject* topleftBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/gold_border_topleft.txt");
		userinterface::BorderPiece* topleftBorderComp = topleftBorder->getComponent<userinterface::BorderPiece>();
		topleftBorderComp->setFramedObject(backGround);
		//topright
		kitten::K_GameObject* toprightBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/gold_border_topright.txt");
		userinterface::BorderPiece* toprightBorderComp = toprightBorder->getComponent<userinterface::BorderPiece>();
		toprightBorderComp->setFramedObject(backGround);
		//left
		kitten::K_GameObject* leftBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/gold_border_left.txt");
		userinterface::BorderPiece* leftBorderComp = leftBorder->getComponent<userinterface::BorderPiece>();
		leftBorderComp->setFramedObject(backGround);
		//right
		kitten::K_GameObject* initRightBorder = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/borders/gold_border_right.txt");
		userinterface::BorderPiece* initRightBorderComp = initRightBorder->getComponent<userinterface::BorderPiece>();
		initRightBorderComp->setFramedObject(backGround);

		kitten::K_GameObject* nextTurnButton = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/gameplay_ui/next_turn_button.json");
		
		int borderOffset = initRightBorder->getTransform().getScale2D().x;
		const glm::vec3 bgPos = backGround->getTransform().getTranslation();
		const glm::vec2 bgScale = backGround->getTransform().getScale2D();
		nextTurnButton->getTransform().place(winX - 160, winY - bgScale.y - 5, -0.01f);
		nextTurnButton->getTransform().scale2D(160, 105);
	}
}
