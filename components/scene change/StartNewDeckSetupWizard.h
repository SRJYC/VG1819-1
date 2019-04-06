#pragma once
#include "kitten/mouse picking/ClickableUI.h"
#include "kitten\InputManager.h"

#include "_Project\StringInputDisplay.h"

#include <string>

class StartNewDeckSetupWizard : public kitten::K_Component, public input::StringListener
{
private:
	StringInputDisplay* m_stringInputDisplay;
	bool m_switchSceneOnEnterPressed;
	const std::string m_sceneToSwitch;

	virtual void start() override;
public:
	StartNewDeckSetupWizard(bool p_switchSceneOnEnterPressed = false, const std::string& p_scene = "");
	~StartNewDeckSetupWizard();

	
	virtual void onStringFinished(const std::string& p_string) override;
	virtual void onStringChanged(const std::string& p_string) override;
};