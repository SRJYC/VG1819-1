/*
@Rock Apr 2
*/
#pragma once

#include "kitten/mouse picking/ClickableUI.h"
#include "puppy\Text\TextBox.h"

class MapListController;
class ChangeMapDetailOnClick : public kitten::ClickableUI
{
public:
	ChangeMapDetailOnClick(const std::string& p_textBoxData);
	~ChangeMapDetailOnClick();

	void start() override;

	void setController(MapListController* p_controller);
	void setInfo(const std::string& p_text, int p_mapId);

	void onClick() override;

private:
	const std::string m_textBoxData;

	MapListController* m_controller;
	puppy::TextBox* m_textBox;

	std::string m_text;
	int m_mapId;
};