#include "PlayerNameController.h"
#include "kitten\InputManager.h"
#include "kitten\K_Instance.h"
#include "settings_menu\PlayerPrefs.h"

PlayerNameController::PlayerNameController(int p_nameMinLimit, int p_nameMaxLimit) 
	: 
	m_nameMinLimit(p_nameMinLimit),
	m_nameMaxLimit(p_nameMaxLimit),
	m_update(false)
{

}

PlayerNameController::~PlayerNameController()
{
	input::InputManager::getInstance()->setPollMode(true);
}

void PlayerNameController::start()
{
	m_input = input::InputManager::getInstance();
	const auto children = getTransform().getChildren();

	kitten::K_GameObject* controllerInput = &children[0]->getAttachedGameObject();
	m_inputTextbox = controllerInput->getComponent<puppy::TextBox>();
	m_inputTextbox->setText("");
	m_stringInputDisplay = controllerInput->getComponent<StringInputDisplay>();
	m_stringInputDisplay->setCharLimit(m_nameMaxLimit);

	kitten::K_GameObject* confirmButtonGO = &children[1]->getAttachedGameObject();
	m_confirmButton = confirmButtonGO->getComponent<userinterface::ClickableButton>();
	m_confirmButtonFrame = confirmButtonGO->getComponent<kitten::ClickableFrame>();

	kitten::K_GameObject* nameStatus = &children[2]->getAttachedGameObject();
	m_nameStatusTextBox = nameStatus->getComponent<puppy::TextBox>();
	m_nameStatusTextBox->setText("");

	std::string savedName = PlayerPrefs::getPlayerName();
	if (savedName != "")
	{
		m_playerName = savedName;
	}

	setConfirmButtonEnabled(false);
}

void PlayerNameController::onEnabled()
{
	if (m_inputTextbox->getText() != m_playerName)
	{
		m_inputTextbox->setText(m_playerName);
	}
	m_nameStatusTextBox->setText("");
	setConfirmButtonEnabled(false);
}

void PlayerNameController::onDisabled()
{
	m_update = false;
	input::InputManager::getInstance()->setPollMode(true);
}

void PlayerNameController::update()
{
	// Using flag to update since setEnabled(false) will stop rendering text
	if (m_update)
	{
		std::string name = m_stringInputDisplay->getString();
		int nameLength = name.length();
		if (nameLength < m_nameMinLimit || nameLength > m_nameMaxLimit)
		{
			m_nameStatusTextBox->setText("Name must be between " + std::to_string(m_nameMinLimit)
				+ " and " + std::to_string(m_nameMaxLimit) + " characters.");
			setConfirmButtonEnabled(false);
		}
		else
		{
			m_nameStatusTextBox->setText("");
			setConfirmButtonEnabled(true);
		}

		if (m_input->keyDown(GLFW_KEY_ENTER) && !m_input->keyDownLast(GLFW_KEY_ENTER))
		{
			std::string name = m_stringInputDisplay->getString();
			if (name.length() >= m_nameMinLimit && name.length() <= m_nameMaxLimit)
			{
				confirmPlayerName();
				m_inputTextbox->setText(m_playerName);
			}
			else
			{
				m_input->setPollMode(false);
			}
		}

		if (m_input->keyDown(GLFW_KEY_ESC) && !m_input->keyDownLast(GLFW_KEY_ESC))
		{
			reset();
		}
	}
}

void PlayerNameController::changePlayerName()
{
	m_update = true;

	// Resets the buffer
	m_input->setPollMode(true);
	m_input->setPollMode(false);
	m_stringInputDisplay->onStringChanged("");
}

void PlayerNameController::confirmPlayerName()
{
	m_update = false;

	m_playerName = m_stringInputDisplay->getString();
	PlayerPrefs::setPlayerName(m_playerName);
	m_nameStatusTextBox->setText("");

	input::InputManager::getInstance()->setPollMode(true);
	setConfirmButtonEnabled(false);
}

void PlayerNameController::setConfirmButtonEnabled(bool p_enabled)
{
	m_confirmButton->setActive(p_enabled);
	m_confirmButtonFrame->setEnabled(p_enabled);
}

void PlayerNameController::reset()
{
	m_inputTextbox->setText(m_playerName);
	m_stringInputDisplay->onStringChanged("");
	setConfirmButtonEnabled(false);
	m_update = false;
}