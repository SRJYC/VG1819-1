#pragma once

#include "kitten\mouse picking\ClickableUI.h"
#include "components\PlayerNameController.h"

class ChangeNameOnClick : public kitten::ClickableUI
{
private:
	PlayerNameController* m_controller;

	virtual void start() override;
	virtual void onClick() override;
public:
	ChangeNameOnClick();
	~ChangeNameOnClick();
};