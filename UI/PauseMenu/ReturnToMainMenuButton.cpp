// ReturnToMainMenuButton
//
// Button used to leave the current game state and return to the
// main menu
// Triggers an event onClick to signal to NetworkingConsoleMenu to
// call the appropriate disconnect function
//
// @Ken

#include "UI\PauseMenu\ReturnToMainMenuButton.h"
#include "kitten\event_system\EventManager.h"
#include "kitten\K_Instance.h"
#include "networking\ClientGame.h"

namespace userinterface
{
	ReturnToMainMenuButton::ReturnToMainMenuButton() { }

	ReturnToMainMenuButton::~ReturnToMainMenuButton() {	}

	void ReturnToMainMenuButton::onClick()
	{
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Return_to_Main_Menu, nullptr);
		if (networking::ClientGame::getInstance() == nullptr)
			kitten::K_Instance::changeScene("mainmenu.json");
	}
}