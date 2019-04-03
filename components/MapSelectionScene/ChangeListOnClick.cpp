#include "ChangeListOnClick.h"
#include "MapListController.h"

ChangeListOnClick::ChangeListOnClick(int p_move)
	:m_move(p_move),
	m_controller(nullptr)
{
}

ChangeListOnClick::~ChangeListOnClick()
{
}

void ChangeListOnClick::setController(MapListController * p_controller)
{
	m_controller = p_controller;
}

void ChangeListOnClick::onClick()
{
	assert(m_controller != nullptr);

	m_controller->changeButton(m_move);
}
