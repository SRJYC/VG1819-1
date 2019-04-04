#include "DeckCountTextureBind.h"
#include "kitten/event_system/EventManager.h"
#include "kitten/K_GameObjectManager.h"
#include "UI/UIFrame.h"

void DeckCountTextureBind::deckEventReceiver(kitten::Event::EventType p_type, kitten::Event * p_data)
{
	int deckCount = p_data->getInt(DECK_CARD_COUNT_LEFT);
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

	kitten::Event* t = new kitten::Event(kitten::Event::EventType::Card_Drawn);
	t->putInt(CARD_COUNT, deckCount);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::EventType::Card_Drawn, t);
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

	kitten::K_GameObject* counter = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/deck/deck_counter_textbox.json");
	puppy::TextBox* txtBoxComp = counter->getComponent<puppy::TextBox>();
	txtBoxComp->setText("Loading...");
	counter->getTransform().place2D(deckTrans.x + 20, deckTrans.y + deckScale.y);
	m_countText = txtBoxComp;

	kitten::K_GameObject* enemyCounter = kitten::K_GameObjectManager::getInstance()->createNewGameObject("ui/deck/deck_counter_textbox.json");
	puppy::TextBox* entxtBoxComp = enemyCounter->getComponent<puppy::TextBox>();
	enemyCounter->getTransform().place2D(deckTrans.x + 40, deckTrans.y + deckScale.y);
	m_enemyCountText = entxtBoxComp;
	entxtBoxComp->setText("?");

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
