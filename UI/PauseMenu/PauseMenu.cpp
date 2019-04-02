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

	// Add Listeners for enabling end game screen on Commander death
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Network_End_Game,
		this,
		std::bind(&PauseMenu::enableEndGameScreen, this, std::placeholders::_1, std::placeholders::_2));

	kitten::K_GameObjectManager* goManager = kitten::K_GameObjectManager::getInstance();

	// Create Options Menu
	m_optionsMenu = goManager->createNewGameObject("UI/options_menu/options_menu_no_close.json");
	m_optionsMenu->setEnabled(false);

	// Create Return to Main Menu Button
	m_returnToMainButton = goManager->createNewGameObject("return_to_main_menu_button.txt");
	addToFrame(static_cast<userinterface::UIObject*>(m_returnToMainButton->getComponent<userinterface::UIObject>()));

	// Create End Game screen UI
	kitten::K_GameObject* endGameScreen = goManager->createNewGameObject("game_result_screen.txt");
	m_endGameScreenObj = static_cast<userinterface::UIObject*>(endGameScreen->getComponent<userinterface::UIObject>());
	addToFrame(m_endGameScreenObj);

	m_returnToMainButton->setEnabled(false);
	m_endGameScreenObj->setEnabled(false);
}

void PauseMenu::update()
{
	if (m_input->keyDown(GLFW_KEY_ESC) && !m_input->keyDownLast(GLFW_KEY_ESC) && !m_gameEnded)
	{
		m_opened = !m_opened;
		setEnabledUI(m_opened);

		kitten::Event* eventData = new kitten::Event(kitten::Event::Pause_Menu_Open);
		eventData->putInt(PAUSE_MENU_OPEN, m_opened);
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Pause_Menu_Open, eventData);
	}	
}

void PauseMenu::setEnabledUI(bool p_enabled)
{
	m_returnToMainButton->setEnabled(p_enabled);
	m_optionsMenu->setEnabled(p_enabled);
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
	m_returnToMainButton->setEnabled(true); 
	m_gameEnded = true; // Permanently show main menu button
	m_opened = true;

	// Disables interacting with other game features
	kitten::Event* eventData = new kitten::Event(kitten::Event::Pause_Menu_Open);
	eventData->putInt(PAUSE_MENU_OPEN, m_opened);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Pause_Menu_Open, eventData);
}
