#include "K_GameObject.h"
#include "K_Component.h"
#include "K_ComponentManager.h"

#include <iostream>

namespace kitten
{
	K_GameObject::K_GameObject() : m_survivesSceneChange(false), m_isEnabled(true)
	{
		m_transform = new Transform(*this);
	}

	K_GameObject::~K_GameObject()
	{
		K_ComponentManager* compMan = K_ComponentManager::getInstance();

		//Delete attached components
		auto it = m_components.begin();
		while (it != m_components.end())
		{
			K_ComponentManager::getInstance()->destroyComponentImmediate(it->second);
			it = m_components.begin();
		}

		delete m_transform;
	}

	bool K_GameObject::addComponent(K_Component* p_toAdd)
	{
		//If you are here from an error, make sure the name of your component in data matches the name
		//in ComponentDataType's jsonComponentMap

		if (m_components.count(std::type_index(typeid(*p_toAdd))) > 0)
		{
			return false;
		}
		//m_components[std::type_index(typeid(*p_toAdd))] = p_toAdd;

		m_components.insert(std::make_pair(std::type_index(typeid(*p_toAdd)), p_toAdd));

		p_toAdd->m_attachedObject = this;

		return true;
	}

	void K_GameObject::removeComponent(K_Component* p_toRemove)
	{
		if (m_components.count(std::type_index(typeid(*p_toRemove))) == 0)
		{
			for (auto it = m_components.begin(); it != m_components.end(); ++it)
			{
				if ((*it).second == p_toRemove)
				{
					assert(false);
				}
			}
			assert(false);
		}
		m_components.erase(std::type_index(typeid(*p_toRemove)));
	}

	bool K_GameObject::isEnabled() const
	{
		return m_isEnabled;
	}

	void K_GameObject::setEnabled(bool p_enabled)
	{
		if ((m_isEnabled && !p_enabled) || (!m_isEnabled && p_enabled))
		{
			m_isEnabled = p_enabled;
			//enable / disable components
			auto end = m_components.cend();
			for (auto it = m_components.cbegin(); it != end; ++it)
			{
				(*it).second->setEnabled(p_enabled);
			}

			//Set children
			auto children = m_transform->getChildren();
			if (!children.empty())
			{
				auto childrenEnd = children.cend();
				for (auto it = children.cbegin(); it != childrenEnd; ++it)
				{
					(*it)->getAttachedGameObject().setEnabled(p_enabled);
				}
			}
		}
		//else, trying to set something enabled/disabled that is already enabled/disabled
	}

	//This does not work, LNK 2019
	/*
	template <class T>
	T* K_GameObject::getComponent()
	{
		if (m_components.count(std::type_index(typeid(T))) != 0)
		{
			return static_cast<T*>(m_components[std::type_index(typeid(T))]);
		}
		else
		{
			return nullptr;
		}
	}
	*/
}