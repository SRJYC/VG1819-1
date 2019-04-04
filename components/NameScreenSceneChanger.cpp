#include "NameScreenSceneChanger.h"
#include "settings_menu\PlayerPrefs.h"
#include "kitten\K_Instance.h"

NameScreenSceneChanger::NameScreenSceneChanger()
{
	assert(true);
}

NameScreenSceneChanger::~NameScreenSceneChanger()
{
	m_input->setPollMode(true);
}

void NameScreenSceneChanger::start()
{
	auto parent = getTransform().getParent();
	assert(parent != nullptr);

	m_controller = parent->getAttachedGameObject().getComponent<PlayerNameController>();
	assert(m_controller != nullptr);
	m_controller->setUpdate(true);

	m_input = input::InputManager::getInstance();
	m_input->setPollMode(false);

	std::string name = PlayerPrefs::getPlayerName();
	if (name != "")
	{
		kitten::K_Instance::changeScene("deck_menu.json");
	}
}

void NameScreenSceneChanger::update()
{
	if (m_input->keyDown(GLFW_KEY_ENTER) && !m_input->keyDownLast(GLFW_KEY_ENTER))
	{
		std::string name = m_controller->getCurrentPlayerName();
		if (name != "")
		{
			kitten::K_Instance::changeScene("deck_menu.json");
		}
		else
		{
			m_input->setPollMode(false);
		}
	}
}