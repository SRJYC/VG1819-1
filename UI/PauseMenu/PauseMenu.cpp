// PauseMenu
//
// Acts like a pause menu (but the game doesn't actually pause), which shows 
// a main menu button and the options menu
// Hitting 'Esc' will either open the menu or close it depending on the state
// This also functions as a way to display the end game screen when a Commander is defeated, 
// alerted through events
//
// @Ken

#include "UI\PauseMenu\PauseMenu.h"
#include "kitten\K_GameObjectManager.h"
#include "kitten\K_ComponentManager.h"

#include "UI\UIObject.h"

#define VICTORY_TEXTURE "textures/ui/victory.tga"
#define DEFEAT_TEXTURE "textures/ui/defeat.tga"
#define DISCONNECT_TEXTURE "textures/ui/disconnect_message.tga"
#define DESYNC_TEXTURE "textures/ui/desync_message.tga"

PauseMenu::PauseMenu(const char* p_pathToTex)
	:
	UIFrame(p_pathToTex),
	m_opened(false),
	m_gameEnded(false)
{

}

PauseMenu::~PauseMenu()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Network_End_Game, this);
}

void PauseMenu::start()
{
	removeFromDynamicUIRender();

	m_input = input::InputManager::getInstance();
	assert(m_input != nullptr);

	// Add Listener for enabling end game screen on Commander death
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Network_End_Game,
		this,
		std::bind(&PauseMenu::enableEndGameScreen, this, std::placeholders::_1, std::placeholders::_2));

	const auto& children = getTransform().getChildren();
	assert(children.size() == 2, "Need options menu and main menu button as children");

	// Get Options Menu
	m_optionsMenu = &children[0]->getAttachedGameObject();

	// Get Return to Main Menu Button
	m_returnToMainButton = &children[1]->getAttachedGameObject();
	addToFrame(static_cast<userinterface::UIObject*>(m_returnToMainButton->getComponent<userinterface::UIObject>()));

	// Create End Game screen UI
	kitten::K_GameObject* endGameScreen = kitten::K_GameObjectManager::getInstance()->createNewGameObject("game_result_screen.txt");
	m_endGameScreenObj = static_cast<userinterface::UIObject*>(endGameScreen->getComponent<userinterface::UIObject>());
	addToFrame(m_endGameScreenObj);
	endGameScreen->setEnabled(false);
}

void PauseMenu::onEnabled()
{
	if (!m_gameEnded)
	{
		m_opened = true;
		kitten::Event* eventData = new kitten::Event(kitten::Event::Pause_Menu_Open);
		eventData->putInt(PAUSE_MENU_OPEN, m_opened);
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Pause_Menu_Open, eventData);
	}
}

void PauseMenu::onDisabled()
{
	if (!m_gameEnded)
	{
		m_opened = false;
		kitten::Event* eventData = new kitten::Event(kitten::Event::Pause_Menu_Open);
		eventData->putInt(PAUSE_MENU_OPEN, m_opened);
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Pause_Menu_Open, eventData);
	}
}

void PauseMenu::enableEndGameScreen(kitten::Event::EventType p_type, kitten::Event* p_data)
{
	int gameResult = p_data->getInt(GAME_END_RESULT);
	switch (gameResult)
	{
		case HOST_COMMANDER_DIED:
			m_endGameScreenObj->setTexture(DEFEAT_TEXTURE);
			break;
		case CLIENT_COMMANDER_DIED:
			// m_endGameScreenObj starting texture is Victory texture
			break;
		case PLAYER_DISCONNECTED:
			m_endGameScreenObj->setTexture(DISCONNECT_TEXTURE);
			break;
		case CLIENT_DESYNCED:
			m_endGameScreenObj->setTexture(DESYNC_TEXTURE);
			break;
	}		

	m_endGameScreenObj->setEnabled(true);	// Show UIObject with appropriate end game message
	setEnabled(true); 
	m_gameEnded = true; // Permanently show main menu button
	m_opened = true;
	printf("Pause menu open\n");
	// Disables interacting with other game features
	kitten::Event* eventData = new kitten::Event(kitten::Event::Pause_Menu_Open);
	eventData->putInt(PAUSE_MENU_OPEN, m_opened);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Pause_Menu_Open, eventData);
}
