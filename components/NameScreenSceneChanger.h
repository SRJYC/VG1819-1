// NameScreenSceneChanger
//
// Component that controls when to change to the deck scene after a name
// is input into PlayerNameController
// Changes to deck scene when: PlayerPrefs has already loaded a name or
// player hits enter and the name grabbed from PlayerNameController
//
// @Ken
#pragma once
#include "kitten\K_Component.h"
#include "kitten/InputManager.h"
#include "PlayerNameController.h"

class NameScreenSceneChanger : public kitten::K_Component
{
private:
	input::InputManager* m_input = nullptr;
	PlayerNameController* m_controller = nullptr;

	virtual void start() override;
	virtual bool hasUpdate() const override { return true; }
	virtual void update() override;


public:
	NameScreenSceneChanger();
	~NameScreenSceneChanger();
};