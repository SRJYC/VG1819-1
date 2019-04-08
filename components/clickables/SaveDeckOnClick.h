#pragma once

#include "UI\ClickableButton.h"
#include "kitten/K_GameObject.h"
#include "kitten\InputManager.h"

class SaveDeckOnClick : public userinterface::ClickableButton, public input::StringListener
{
private:
	kitten::K_GameObject* m_message = nullptr;

	virtual void onClick() override;
public:
	SaveDeckOnClick();
	~SaveDeckOnClick();

	virtual void onStringFinished(const std::string& p_string) override;
	virtual void onStringChanged(const std::string& p_string) override;
};