// PauseMenuToggle
//
// Component used to open/close PauseMenu upon key press
// Built similarly to ToggleSiblingEnabledOnClick
// Should be implemented as a child GO with the PauseMenu as a sibling GO
// This allows the attached GO for this to also have the ToggleSiblingEnabledOnClick
// allowing for the PauseMenu to be opened by key press or button click
//
// @Ken
#include "PauseMenuToggle.h"

PauseMenuToggle::PauseMenuToggle()
{

}

PauseMenuToggle::~PauseMenuToggle()
{

}

void PauseMenuToggle::start()
{
	m_input = input::InputManager::getInstance();
	assert(m_input != nullptr);

	const auto& transform = getTransform();
	const auto parent = getTransform().getParent();
	assert(parent != nullptr);

	const auto& children = parent->getChildren();

	auto end = children.cend();
	for (auto it = children.cbegin(); it != end; ++it)
	{
		m_sibling = &(*it)->getAttachedGameObject();

		if (m_sibling != m_attachedObject)
		{
			m_sibling->setEnabled(false);
			return;
		}
	}

	assert(false, "PauseMenuToggle has no sibling to toggle, but has PauseMenuToggle component");
}

void PauseMenuToggle::update()
{
	if (m_input->keyDown(GLFW_KEY_ESC) && !m_input->keyDownLast(GLFW_KEY_ESC))
	{
		m_sibling->setEnabled(!m_sibling->isEnabled());
	}
}