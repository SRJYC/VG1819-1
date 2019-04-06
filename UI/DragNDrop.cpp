#include "DragNDrop.h"
#include "kitten/InputManager.h"

#define TIME_TO_GO_BACK_TO_ORIGIN 0.025

DragNDrop::DragNDrop(bool p_backToOrigin) : m_backToOrigin(p_backToOrigin), m_isDragging(false)
{
}

DragNDrop::~DragNDrop()
{
}

void DragNDrop::onClick() 
{
	if (m_focused && m_isDragging)
	{
		getTransform().move(0,0,-0.1);
		m_focused = false;
		m_isDragging = false;
		onDrop();
	}
	else if(!m_focused && !m_isDragging)
	{
		getTransform().move(0, 0, 0.1);
		m_focused = true;
		m_isDragging = true;
	}
}

void DragNDrop::update()
{
	if (m_isDragging)
	{
		m_attachedObject->getTransform().place2D(
			input::InputManager::getInstance()->getMouseXPos() - 25,
			input::InputManager::getInstance()->getMouseYOpenGLPos() - getTransform().getScale2D().y + 25
		);
	}
}

void DragNDrop::start()
{
	m_isDragging = false;
	ClickableUI::start();
	m_lerpController = m_attachedObject->getComponent<LerpController>();
}

void DragNDrop::onDrop() // Override to set your custom own behavior
{
	if (m_backToOrigin)
	{
		//getTransform().place2D(m_origin.x, m_origin.y);
		m_lerpController->positionLerp(m_origin, TIME_TO_GO_BACK_TO_ORIGIN);
	}
	MousePicker::m_isFocused = false;
}