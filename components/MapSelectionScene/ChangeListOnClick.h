/*
@Rock Apr 3
*/
#pragma once

#include "kitten/mouse picking/ClickableUI.h"

class MapListController;
class ChangeListOnClick : public kitten::ClickableUI
{
public:
	ChangeListOnClick(int p_move);
	~ChangeListOnClick();

	void setController(MapListController* p_controller);

	void onClick() override;

private:
	const int m_move;
	MapListController* m_controller;
};
