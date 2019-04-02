#include "ActiveClickables.h"
#include "Clickable.h"
#include "kitten\K_ComponentManager.h"

namespace kitten 
{
	ClickableBox::ClickableBox(const glm::vec3& p_minPoint, const glm::vec3& p_maxPoint) : m_originalMinPoint(p_minPoint), m_originalMaxPoint(p_maxPoint), m_gamePaused(false)
	{
		
		m_minPoint = m_originalMinPoint;
		m_maxPoint = m_originalMaxPoint;

		kitten::EventManager::getInstance()->addListener(
			kitten::Event::EventType::Pause_Menu_Open,
			this,
			std::bind(&ClickableBox::toggleGamePausedListener, this, std::placeholders::_1, std::placeholders::_2));
	}

	ClickableBox::~ClickableBox()
	{
		ActiveClickables::getInstance()->removeFromActive(this);

		auto it = m_listeners.begin();
		while(it != m_listeners.end())
		{
			kitten::K_ComponentManager::getInstance()->destroyComponentImmediate(*it);
			it = m_listeners.begin();
		}

		kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Pause_Menu_Open, this);
	}

	void ClickableBox::start()
	{
		m_minPoint = m_originalMinPoint * getTransform().getScale();
		m_maxPoint = m_originalMaxPoint * getTransform().getScale();

		getTransform().addScaleListener(this);

		ActiveClickables::getInstance()->addToActive(this);
	}

	void ClickableBox::onDisabled()
	{
		ActiveClickables::getInstance()->removeFromActive(this);
	}

	void ClickableBox::onEnabled()
	{
		ActiveClickables::getInstance()->addToActive(this);
	}

	void ClickableBox::addClickable(Clickable* p_toAdd)
	{
		m_listeners.insert(p_toAdd);
	}

	void ClickableBox::removeClickable(Clickable* p_toRemove)
	{
		m_listeners.erase(p_toRemove);
	}

	void ClickableBox::onScaleChanged(const glm::vec3& p_newScale)
	{
		m_minPoint = m_originalMinPoint * p_newScale;
		m_maxPoint = m_originalMaxPoint * p_newScale;
	}

	void ClickableBox::onHoverStart()
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
	}

	void ClickableBox::onClick()
	{
		auto end = m_listeners.cend();
		for (auto it = m_listeners.begin(); it != end; ++it)
		{
			if (!m_gamePaused || (*it)->isEnabledOnPause())
			{
				if ((*it)->isEnabled())
					(*it)->onClick();
			}
		}
	}

	void ClickableBox::onHoverEnd()
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
	}

	void ClickableBox::toggleGamePausedListener(kitten::Event::EventType p_type, kitten::Event* p_data)
	{
		m_gamePaused = p_data->getInt(PAUSE_MENU_OPEN);;

		auto end = m_listeners.cend();
		for (auto it = m_listeners.begin(); it != end; it++)
		{
			if (m_gamePaused && (*it)->isEnabled())
			{
				(*it)->onPause();
			} else
			{
				(*it)->onUnpause();
			}
		}
	}
}