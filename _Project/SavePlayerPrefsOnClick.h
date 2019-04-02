#pragma once

#include "kitten\mouse picking\ClickableUI.h"

class SavePlayerPrefsOnClick : public kitten::ClickableUI
{
private:
	bool m_closeOnClick;

	virtual void onClick() override;

public:
	SavePlayerPrefsOnClick(bool p_closeOnClick = true);
	~SavePlayerPrefsOnClick();

	void setCloseOnClick(bool p_close) { m_closeOnClick = p_close; }
};