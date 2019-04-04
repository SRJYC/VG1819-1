/*
@Rock Apr 4
*/
#pragma once

#include "kitten/mouse picking/ClickableUI.h"
#include "UI/UIObject.h"

class ModeDetailController;
class ChangeModeDetailOnClick : public kitten::ClickableUI
{
public:
	ChangeModeDetailOnClick();
	~ChangeModeDetailOnClick();

	void start() override;

	void setController(ModeDetailController* p_controller);
	void setInfo(const std::string& p_img, int p_id);

	void onClick() override;

private:
	const std::string m_textBoxData;

	ModeDetailController* m_controller;
	userinterface::UIObject* m_ui;

	std::string m_img;
	int m_id;
};