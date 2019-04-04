#include "ConfirmNameOnClick.h"

ConfirmNameOnClick::ConfirmNameOnClick() : m_controller(nullptr)
{
	m_enabledOnPause = true;
}

ConfirmNameOnClick::~ConfirmNameOnClick()
{

}

void ConfirmNameOnClick::start()
{
	ClickableUI::start();

	auto parent = getTransform().getParent();
	assert(parent != nullptr);

	m_controller = parent->getAttachedGameObject().getComponent<PlayerNameController>();
	assert(m_controller != nullptr);
}

void ConfirmNameOnClick::onClick()
{
	m_controller->confirmPlayerName();
}