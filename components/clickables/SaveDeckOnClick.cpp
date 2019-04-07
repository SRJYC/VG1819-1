#include "SaveDeckOnClick.h"
#include "components/DeckAlterationComponent.h"
#include "kitten/K_GameObjectManager.h"
#include "_Project/DisableAfterTime.h"

SaveDeckOnClick::SaveDeckOnClick()
{ 
	input::InputManager::getInstance()->addStringListener(this);
}

SaveDeckOnClick::~SaveDeckOnClick() 
{
	input::InputManager::getInstance()->removeStringListener(this);
}

void SaveDeckOnClick::onClick()
{
	if (m_message == nullptr)
	{
		m_message = kitten::K_GameObjectManager::getInstance()->createNewGameObject("saved_deck_message.json");
	}
	else
	{
		m_message->setEnabled(true);
	}
	m_message->getComponent<DisableAfterTime>()->setTime(3.0);

	DeckAlterationComponent::getActiveInstance()->saveDeck();

}

void SaveDeckOnClick::onStringChanged(const std::string& p_str)
{

}

void SaveDeckOnClick::onStringFinished(const std::string& p_str)
{
	onClick();

	input::InputManager::getInstance()->setPollMode(false);
}