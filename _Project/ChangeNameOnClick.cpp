#include "ChangeNameOnClick.h"

ChangeNameOnClick::ChangeNameOnClick() : m_controller(nullptr)
{
	m_enabledOnPause = true;
}

ChangeNameOnClick::~ChangeNameOnClick()
{

}

void ChangeNameOnClick::start()
{
	ClickableUI::start();

	auto parent = getTransform().getParent();
	assert(parent != nullptr);

	m_controller = parent->getAttachedGameObject().getComponent<PlayerNameController>();
	assert(m_controller != nullptr);
}

void ChangeNameOnClick::onClick()
{
	m_controller->changePlayerName();
}