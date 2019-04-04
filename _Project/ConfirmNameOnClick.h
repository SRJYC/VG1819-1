#pragma once

#include "kitten\mouse picking\ClickableUI.h"
#include "components\PlayerNameController.h"

class ConfirmNameOnClick : public kitten::ClickableUI
{
private:
	PlayerNameController* m_controller;

	virtual void start() override;
	virtual void onClick() override;
public:
	ConfirmNameOnClick();
	~ConfirmNameOnClick();
};