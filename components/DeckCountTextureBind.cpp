#include "DeckCountTextureBind.h"
#include "kitten/event_system/EventManager.h"
#include "kitten/K_GameObjectManager.h"
#include "UI/UIFrame.h"
#include "networking\ClientGame.h"

void DeckCountTextureBind::deckEventReceiver(kitten::Event::EventType p_type, kitten::Event * p_data)
{
	int deckCount = p_data->getInt("count");
	if (m_deckCount > deckCount)
	{
		if (m_currentTexPair->first > deckCount)
		{
			while (m_currentTexPair->first > deckCount)
			{
				m_currentTexPair++;
			}
			m_attachedObject->getComponent<userinterface::UIFrame>()->setTexture(m_currentTexPair->second.c_str());
		}
	}

	m_deckCount = deckCount;
	if (m_deckCount > 0)
	{
		m_countText->setText(std::to_string(deckCount));
	}
	else {
		kitten::K_GameObjectManager::getInstance()->destroyGameObject(&m_countText->getGameObject());
	}

	kitten::Event* t = new kitten::Event(kitten::Event::EventType::Send_Deck_Count);
	t->putInt(CARD_COUNT, deckCount);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::EventType::Send_Deck_Count, t);
}

void DeckCountTextureBind::enemyDrawEventReceiver(kitten::Event::EventType p_type, kitten::Event* p_data)
{
	int count = p_data->getInt(CARD_COUNT);
	m_enemyCountText->setText(std::to_string(count));
}

void DeckCountTextureBind::start()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Card_Drawn,
		this,
		std::bind(&DeckCountTextureBind::deckEventReceiver, this, std::placeholders::_1, std::placeholders::_2));
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Card_Discarded,
		this,
		std::bind(&DeckCountTextureBind::deckEventReceiver, this, std::placeholders::_1, std::placeholders::_2));
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Enemy_Draw_Card,
		this,
		std::bind(&DeckCountTextureBind::enemyDrawEventReceiver, this, std::placeholders::_1, std::placeholders::_2));

	m_currentTexPair = m_texPairs.rbegin();
	m_attachedObject->getComponent<userinterface::UIFrame>()->setTexture(m_currentTexPair->second.c_str());

	const glm::vec2 deckScale = getTransform().getScale2D();
	const glm::vec3 deckTrans = getTransform().getTranslation();

	bool host = false;
	if (networking::ClientGame::getClientId == 0)
	{
		host = true;
	}

	//make the tex
	kitten::K_GameObject* counter = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/deck/deck_counter_textbox.json");
	puppy::TextBox* txtBoxComp = counter->getComponent<puppy::TextBox>();
	txtBoxComp->setText("?");
	counter->getTransform().place2D(deckTrans.x + 20, deckTrans.y + deckScale.y);
	m_countText = txtBoxComp;

	kitten::K_GameObject* enemyCounter = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/deck/deck_counter_textbox.json");
	puppy::TextBox* entxtBoxComp = enemyCounter->getComponent<puppy::TextBox>();
	entxtBoxComp->setColor(1, 0, 0);
	enemyCounter->getTransform().place2D(deckTrans.x + 70, deckTrans.y + deckScale.y);
	m_enemyCountText = entxtBoxComp;
	entxtBoxComp->setText("?");

	//set colors
	if (host)
	{
		txtBoxComp->setColor(0.1, 0.1, 1);
		entxtBoxComp->setColor(1, 1, 0);

	}
	else {
		entxtBoxComp->setColor(1, 1, 0);
		entxtBoxComp->setColor(0.1, 0.1, 1);
	}

}

void DeckCountTextureBind::addTexPair(int p_atCount, const std::string& p_tex)
{
	m_texPairs[p_atCount] = p_tex;
}

DeckCountTextureBind::DeckCountTextureBind()
{
}

DeckCountTextureBind::~DeckCountTextureBind()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Card_Drawn, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Card_Discarded, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Enemy_Draw_Card, this);
}
