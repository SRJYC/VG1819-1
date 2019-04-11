#include "HoverOverCardBehavior.h"
#include "UI/HandFrame.h"
#include "kibble/databank/databank.hpp"

#define CARD_HOVER_MOVE_TIME 0.2

HoverOverCardBehavior::HoverOverCardBehavior()
{
}

HoverOverCardBehavior::~HoverOverCardBehavior()
{
	if (m_isHovered) {
		kitten::Event* e = new kitten::Event(kitten::Event::Card_Context_Set_Enabled);
		e->putInt(CARD_CONTEXT_SET_ENABLED_KEY, FALSE);
		kitten::EventManager::getInstance()->queueEvent(kitten::Event::Card_Context_Set_Enabled, e);
	}
}

void HoverOverCardBehavior::start()
{
	ClickableUI::start();
	m_lerpController = m_attachedObject->getComponent<LerpController>();
	m_unit = m_attachedObject->getComponent<unit::Unit>();
	m_cardContext = userinterface::HandFrame::getActiveInstance()->getGameObject().getComponent<CardContext>();
}

void HoverOverCardBehavior::onHoverStart()
{
	m_lerpController->positionLerp(glm::vec3(m_origin.x, m_origin.y + 50, m_origin.z), CARD_HOVER_MOVE_TIME);
	m_isHovered = true;

	// Get CardUIO attached unit ID
	kitten::Event* updateContextEvent = new kitten::Event(kitten::Event::Update_Card_Context_By_GO);
	updateContextEvent->putGameObj(UPDATE_CARD_CONTEXT_KEY, m_attachedObject);
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Update_Card_Context_By_GO, updateContextEvent);

	// Enable Card Context
	kitten::Event* enableContextEvent = new kitten::Event(kitten::Event::Card_Context_Set_Enabled);
	enableContextEvent->putInt(CARD_CONTEXT_SET_ENABLED_KEY, TRUE);
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Card_Context_Set_Enabled, enableContextEvent);
}

void HoverOverCardBehavior::onHoverEnd()
{
	m_lerpController->positionLerp(m_origin, CARD_HOVER_MOVE_TIME);
	m_isHovered = false;
	kitten::Event* e = new kitten::Event(kitten::Event::Card_Context_Set_Enabled);
	e->putInt(CARD_CONTEXT_SET_ENABLED_KEY, FALSE);
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Card_Context_Set_Enabled, e);
}
