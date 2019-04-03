#include "PlayerNameController.h"
#include "kitten\K_GameObjectManager.h"
#include "kitten\event_system\EventManager.h"
#include "networking\ClientGame.h"
#include "kitten\InputManager.h"
#include "kitten\K_Instance.h"
#include "settings_menu\PlayerPrefs.h"

#define ENTER_NAME_MSG "Enter new name"

PlayerNameController::PlayerNameController(int p_nameMinLimit, int p_nameMaxLimit) 
	: 
	m_nameMinLimit(p_nameMinLimit),
	m_nameMaxLimit(p_nameMaxLimit),
	m_update(false)
{

}

PlayerNameController::~PlayerNameController()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Scene_Change, this);
	input::InputManager::getInstance()->setPollMode(true);
}

void PlayerNameController::start()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Scene_Change,
		this,
		std::bind(&PlayerNameController::sceneChangeListener, this, std::placeholders::_1, std::placeholders::_2));

	const auto children = getTransform().getChildren();

	kitten::K_GameObject* controllerInput = &children[0]->getAttachedGameObject();
	m_inputTextbox = controllerInput->getComponent<puppy::TextBox>();
	m_stringInputDisplay = controllerInput->getComponent<StringInputDisplay>();
	m_stringInputDisplay->setCharLimit(m_nameMaxLimit);

	kitten::K_GameObject* confirmButtonGO = &children[3]->getAttachedGameObject();
	m_confirmButton = confirmButtonGO->getComponent<userinterface::ClickableButton>();
	m_confirmButtonFrame = confirmButtonGO->getComponent<kitten::ClickableFrame>();

	kitten::K_GameObject* nameStatus = &children[4]->getAttachedGameObject();
	m_nameStatusTextBox = nameStatus->getComponent<puppy::TextBox>();
	m_nameStatusTextBox->setText("");

	std::string savedName = PlayerPrefs::getPlayerName();
	if (savedName != "")
	{
		m_playerName = savedName;
	}
}

void PlayerNameController::onEnabled()
{
	if (m_inputTextbox->getText() != m_playerName)
	{
		m_inputTextbox->setText(m_playerName);
	}
	setConfirmButtonEnabled(false);
}

void PlayerNameController::onDisabled()
{
	input::InputManager::getInstance()->setPollMode(true);
}

void PlayerNameController::update()
{
	// Using flag to update since setEnabled(false) will stop rendering text
	if (m_update)
	{
		std::string name = m_inputTextbox->getText();
		int nameLength = name.length();
		if (nameLength < m_nameMinLimit || nameLength > m_nameMaxLimit || name == ENTER_NAME_MSG)
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

		input::InputManager* inputMan = input::InputManager::getInstance();
		if (inputMan->keyDown(GLFW_KEY_ENTER) && !inputMan->keyDownLast(GLFW_KEY_ENTER))
		{
			std::string name = m_stringInputDisplay->getString();
			if (name.length() >= m_nameMinLimit && name.length() <= m_nameMaxLimit)
			{
				confirmPlayerName();
				m_inputTextbox->setText(m_playerName);
			}
			else
			{
				inputMan->setPollMode(false);
			}
		}
	}
}

void PlayerNameController::changePlayerName()
{
	m_update = true;
	m_inputTextbox->setText(ENTER_NAME_MSG);
	input::InputManager::getInstance()->setPollMode(false);
}

void PlayerNameController::confirmPlayerName()
{
	m_update = false;

	m_playerName = m_stringInputDisplay->getString();
	PlayerPrefs::setPlayerName(m_playerName);
	//m_inputTextbox->setText(m_playerName);
	m_nameStatusTextBox->setText("");

	input::InputManager::getInstance()->setPollMode(true);
	setConfirmButtonEnabled(false);
}

void PlayerNameController::sceneChangeListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	std::string scene = p_event->getString(NEXT_SCENE_PATH_KEY);

	if (scene == DECK_MENU_SCENE)
	{
		networking::ClientGame::setPlayerName(m_stringInputDisplay->getString());
	}
}

void PlayerNameController::setConfirmButtonEnabled(bool p_enabled)
{
	m_confirmButton->setActive(p_enabled);
	m_confirmButtonFrame->setEnabled(p_enabled);
}