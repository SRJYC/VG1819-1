#include "ActiveClickables.h"
#include "ClickableUI.h"
#include "kitten\K_ComponentManager.h"
#include <iostream>

//austin's clickable ui frame (bounding box for clickables)

namespace kitten
{
	ClickableFrame::ClickableFrame(PivotType p_piv) : m_gamePaused(false)
	{
		ActiveClickables::getInstance()->addToActiveUI(this);
		m_piv = p_piv;

		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Pause_Menu_Open,
			this,
			std::bind(&ClickableFrame::toggleGamePausedListener, this, std::placeholders::_1, std::placeholders::_2));
	}

	ClickableFrame::~ClickableFrame()
	{
		ActiveClickables::getInstance()->removeFromActiveUI(this);

		auto it = m_listeners.begin();
		while (it != m_listeners.end())
		{
			kitten::K_ComponentManager::getInstance()->destroyComponentImmediate(*it);
			it = m_listeners.begin();
		}

		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Pause_Menu_Open, this);
		getTransform().removeScaleListener(this);
		getTransform().removePositionListener(this);
	}

	void ClickableFrame::start()
	{
		onPosChanged(getTransform().getTranslation());
		getTransform().addScaleListener(this);
		getTransform().addPositionListener(this);
	}

	void ClickableFrame::addCLickable(ClickableUI* p_toAdd)
	{
		m_listeners.insert(p_toAdd);
	}

	void ClickableFrame::removeClickable(ClickableUI* p_toRemove)
	{
		m_listeners.erase(p_toRemove);
	}

	void ClickableFrame::onScaleChanged(const glm::vec3& p_newScale)
	{ 
		m_maxPoint = glm::vec2(p_newScale.x, p_newScale.y);
	}

	void ClickableFrame::onHoverStart()
	{
		auto end = m_listeners.cend();
		for (auto it = m_listeners.begin(); it != end; ++it)
		{
			if (!m_gamePaused || (*it)->isEnabledOnPause())
			{
				if ((*it)->isEnabled())
					(*it)->onHoverStart();
			}
		}
		std::cout << "Frame Hovered...";
	}

	void ClickableFrame::onClick()
	{
		auto end = m_listeners.cend();
		for (auto it = m_listeners.begin(); it != end; ++it)
		{
			if (!m_gamePaused || (*it)->isEnabledOnPause())
			{
				if((*it)->isEnabled())
					(*it)->onClick();
			}
		}
		std::cout << "Frame Clicked!\n";
	}

	void ClickableFrame::onHoverEnd()
	{
		auto end = m_listeners.cend();
		for (auto it = m_listeners.begin(); it != end; ++it)
		{
			if (!m_gamePaused || (*it)->isEnabledOnPause())
			{
				if ((*it)->isEnabled())
					(*it)->onHoverEnd();
			}
		}
		std::cout << "Hover Ended...\n";
	}

	void ClickableFrame::release()
	{
		auto end = m_listeners.cend();
		for (auto it = m_listeners.begin(); it != end; it++)
		{
			if (!m_gamePaused || (*it)->isEnabledOnPause())
			{
				if ((*it)->isEnabled())
					(*it)->release();
			}
		}
		std::cout << "Frame released!\n";
	}

	void ClickableFrame::onPosChanged(const glm::vec3& p_newPos)
	{
		glm::vec2 trans2D = (glm::vec2)p_newPos;
		//get scale and position in usable form
		glm::vec2 scale = getTransform().getScale2D();
		float width = scale.x;
		float height = scale.y;
		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;

		switch (m_piv)
		{
		case piv_Right:
		{
			m_minPoint = glm::vec2(-width + trans2D.x, -halfHeight + trans2D.y);
			m_maxPoint = glm::vec2(0.0f + trans2D.x, halfHeight + trans2D.y);
			break;
		}
		case piv_Left:
		{
			m_minPoint = glm::vec2(0.0f + trans2D.x, -halfHeight + trans2D.y);
			m_maxPoint = glm::vec2(width + trans2D.x, halfHeight + trans2D.y);
			break;
		}
		case piv_Top:
		{
			m_minPoint = glm::vec2(-halfWidth + trans2D.x, -height + trans2D.y);
			m_maxPoint = glm::vec2(halfWidth + trans2D.x, 0.0f + trans2D.y);
			break;
		}
		case piv_Bot:
		{
			m_minPoint = glm::vec2(-halfWidth + trans2D.x, 0.0f + trans2D.y);
			m_maxPoint = glm::vec2(halfWidth + trans2D.x, height + trans2D.y);
			break;
		}
		case piv_TopRight:
		{
			m_minPoint = glm::vec2(-width + trans2D.x, -height + trans2D.y);
			m_maxPoint = glm::vec2(width + trans2D.x, height + trans2D.y);
			break;
		}
		case piv_BotRight:
		{
			m_minPoint = glm::vec2(-width + trans2D.x, 0.0f + trans2D.y);
			m_maxPoint = glm::vec2(0.0f + trans2D.x, height + trans2D.y);
			break;
		}
		case piv_TopLeft:
		{
			m_minPoint = glm::vec2(0.0f + trans2D.x, -height + trans2D.y);
			m_maxPoint = glm::vec2(width + trans2D.x, 0.0f + trans2D.y);
			break;
		}
		case piv_BotLeft:
		{
			m_minPoint = glm::vec2(0.0f + trans2D.x, 0.0f + trans2D.y);
			m_maxPoint = glm::vec2(width + trans2D.x, height + trans2D.y);
			break;
		}
		case piv_Center:
		{
			m_minPoint = glm::vec2(-halfWidth + trans2D.x, -halfHeight + trans2D.y);
			m_maxPoint = glm::vec2(halfWidth + trans2D.x, halfHeight + trans2D.y);
		}
		}

		m_originalMaxPoint = m_maxPoint;
		m_originalMinPoint = m_minPoint;
	}

	void ClickableFrame::onDisabled()
	{
		ActiveClickables::getInstance()->removeFromActiveUI(this);
	}

	void ClickableFrame::onEnabled()
	{
		ActiveClickables::getInstance()->addToActiveUI(this);
	}

	void ClickableFrame::toggleGamePausedListener(kitten::Event::EventType p_type, kitten::Event* p_data)
	{
		m_gamePaused = p_data->getInt(PAUSE_MENU_OPEN);;

		auto end = m_listeners.cend();
		for (auto it = m_listeners.begin(); it != end; it++)
		{
			if (m_gamePaused)
			{
				(*it)->onPause();
			}
			else
			{
				(*it)->onUnpause();
			}
		}
	}
}