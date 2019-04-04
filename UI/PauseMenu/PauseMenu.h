#pragma once
#include "UI\UIFrame.h"
#include "kitten\InputManager.h"
#include "kitten\event_system\EventManager.h"

class PauseMenu : public userinterface::UIFrame
{
private:
	input::InputManager* m_input;
	bool m_opened;
	bool m_gameEnded;

	kitten::K_GameObject* m_optionsMenu = nullptr;
	kitten::K_GameObject* m_returnToMainButton = nullptr;

	userinterface::UIObject* m_endGameScreenObj = nullptr;

	virtual void start() override;
	virtual void onEnabled();
	virtual void onDisabled();

	void enableEndGameScreen(kitten::Event::EventType p_type, kitten::Event* p_data);
public:
	PauseMenu(const char* p_pathToTex);
	~PauseMenu();
};