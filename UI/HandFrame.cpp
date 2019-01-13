#include "ui/HandFrame.h"
#include "glm/glm.hpp"
#include "UIObject.h"
#include "kitten/Transform.h"
#include "ClickableCard.h"
#include "ContextMenu.h"
#include "kibble/databank/databank.hpp"
#include "unit/Unit.h"
#include "components/PowerTracker.h"
#include "board/BoardManager.h"

#include "kitten/K_GameObjectManager.h"
#include "kitten/K_ComponentManager.h"
#include "UI/CardUIO.h"
#include "kitten/InputManager.h"
#include "_Project/LerpController.h"

#include "components\DragNDrop\SpawnUnitOnDrop.h"
#include "components/clickables/DiscardCardOnClick.h"
#include "components/clickables/HoverOverCardBehavior.h"
#include "UI\CardContext.h"

#define MAX_CARDS_IN_HAND 5
#define TIME_FOR_CARDS_TO_ORDER 0.1
#define TEMP_POWER_CHARGE 1

namespace userinterface
{
	HandFrame* instance = nullptr;
	HandFrame* HandFrame::getActiveInstance() 
	{
		return instance;
	}

	HandFrame::HandFrame(const char* p_pathToTex) : UIFrame(p_pathToTex)
	{
		m_totalCards = 0;
		m_cardX = 100;
		m_cardY = 170;
		m_padding = 10;
		m_contentMargin = 10;

		instance = this;
		m_texBehaviour = tbh_Repeat;
	}

	HandFrame::~HandFrame()
	{
		instance = nullptr;
		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Card_Drawn, this);
	}

	void HandFrame::addCardToEnd(UIObject* p_cardToAdd)
	{
		m_innerObjects.push_back(p_cardToAdd);
		m_totalCards++;
		reorderAllCards();
	}

	//TODO: Assign parent frame when object is added
	void HandFrame::addCardToFront(UIObject* p_cardToAdd)
	{
		m_innerObjects.push_front(p_cardToAdd);
		m_totalCards++;
		reorderAllCards();
	}

	void HandFrame::removeCard(UIObject* p_cardToRemove)
	{
		auto end = m_innerObjects.end();
		for (auto it = m_innerObjects.begin(); it != end; ++it)
		{
			if (*it == p_cardToRemove)
			{
				m_innerObjects.erase(it);
				break;
			}
		}
		m_totalCards--;
		reorderAllCards();
	}

	void HandFrame::reorderAllCards()
	{
		glm::vec3 currentPos = getTransform().getTranslation();
		getTransform().place(currentPos.x, currentPos.y, -0.03f);
		kitten::Transform T = getTransform();
		glm::vec3 trans = T.getTranslation();
		float offset = m_padding;
		auto end = m_innerObjects.end();
		glm::vec3 placement;
		for (auto it = m_innerObjects.begin(); it != end; ++it)
		{
			placement = glm::vec3( trans.x + offset, trans.y + m_padding - (m_cardY / 2), 0 );
			if (m_isInit) {
				LerpController* lerpC = (*it)->getGameObject().getComponent<LerpController>();
				if (lerpC == nullptr)
				{
					(*it)->getTransform().place2D(placement.x,placement.y);
				}
				else
				{
					lerpC->positionLerp(glm::vec3(placement.x, placement.y, 0), TIME_FOR_CARDS_TO_ORDER);
				}
			}
			else
			{
				(*it)->getTransform().place2D(placement.x, placement.y);
			}
			
			(*it)->getGameObject().getComponent<SpawnUnitOnDrop>()->getOrigin() = placement;
			(*it)->getGameObject().getComponent<HoverOverCardBehavior>()->getOrigin() = placement;

			offset += m_cardX;
			offset += m_contentMargin;
		}
	}

	void HandFrame::receiveDrawnCard(kitten::Event::EventType p_type, kitten::Event* p_event)
	{
		if(m_playerID != p_event->getInt(PLAYER_ID)) return;
		// Find the number of cards to add to hand
		int countToAdd = std::min(p_event->getInt(CARD_COUNT), MAX_CARDS_IN_HAND - (int)m_innerObjects.size());

		// Generate Cards to add
		for (int i = 0; i < countToAdd; i++) {
			kitten::K_GameObject* card = kitten::K_GameObjectManager::getInstance()->createNewGameObject("handcard.json");
			userinterface::CardUIO* cardCasted = card->getComponent<userinterface::CardUIO>();
			cardCasted->scaleAsCard();

			this->addCardToEnd(cardCasted);
			cardCasted->assignParentHand(this);

			kitten::K_Component* clickableCom = kitten::K_ComponentManager::getInstance()->createComponent("ClickableCard");
			card->addComponent(clickableCom);

			card->addComponent(
				kibble::getUnitInstanceFromId(
					p_event->getInt(CARD_ID + std::to_string(i))
				)
			);

			// TODO prompt a newer event linking the Unit component for buffs or debuffs. 
		}

		// Add the extras as power
		if (countToAdd < p_event->getInt(CARD_COUNT))
		{
			BoardManager::getInstance()->getPowerTracker()->changeCurrentPower(TEMP_POWER_CHARGE * (p_event->getInt(CARD_COUNT) - countToAdd));
		}

		reorderAllCards();
	}

	void HandFrame::start()
	{
		UIFrame::start();
		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Card_Drawn,
			this,
			std::bind(&HandFrame::receiveDrawnCard, this, std::placeholders::_1, std::placeholders::_2));

		userinterface::HandFrame* frameCasted = m_attachedObject->getComponent<HandFrame>();
		m_isInit = true;
	}

	void HandFrame::onEnabled()
	{
		UIFrame::onEnabled();
		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Card_Drawn,
			this,
			std::bind(&HandFrame::receiveDrawnCard, this, std::placeholders::_1, std::placeholders::_2));
	}

	void HandFrame::onDisabled()
	{
		UIFrame::onDisabled();
		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Card_Drawn, this);
	}


	void HandFrame::setPlayerID(int p_playerId) {
		this->m_playerID = p_playerId;
	}
	void HandFrame::setDiscardMode(bool p_flag) {
		this->m_isDiscardMode = p_flag;

		// Set the notifying object state
		if (m_discardNotifyingObject != nullptr) m_discardNotifyingObject->setEnabled(p_flag);

		for (auto card : m_innerObjects) {
			// These should be enabled if we want discarding on, disabled if off
			card->getGameObject().getComponent<DiscardCardOnClick>()->setEnabled(p_flag);

			// These should be disabled if we want discarding on, enabled if off
			card->getGameObject().getComponent<SpawnUnitOnDrop>()->setEnabled(!p_flag);
		}
	}

	void HandFrame::setPointCountToDiscard(unsigned int p_count)
	{ // TODO fix this
		if (p_count == 0) return;
		m_toDiscard = p_count;
		this->setDiscardMode(true);
	}
	void HandFrame::decreasePointCountBy(int p_value)
	{
		m_toDiscard -= p_value;
		if (m_toDiscard <= 0) this->setDiscardMode(false);
	}
}

void userinterface::HandFrame::makeAHand() {
	input::InputManager* inman = input::InputManager::getInstance();
	kitten::K_GameObject* hand = kitten::K_GameObjectManager::getInstance()->createNewGameObject("handframe.json");
	
}