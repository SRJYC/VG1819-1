#include "K_RenderNode.h"

#include "K_UIRenderable.h"
#include "puppy\Renderer.h"
#include "kitten\K_ComponentManager.h"

#include <iostream>

namespace kitten
{
	K_RenderNode::K_RenderNode() : m_parentRenderNode(nullptr), m_disabledCalled(false), m_isRendering(false)
	{

	}

	K_RenderNode::~K_RenderNode()
	{
		getTransform().removeParentListener(this);

		if (m_isEnabled && m_hasStarted)
		{
			onDisabled();
		}
		else
		{
			if (m_parentRenderNode != nullptr)
			{
				m_parentRenderNode->removeChild(this);
			}	
		}
		
		// m_childRenderNodes will likely always be empty, but just in case ...
		const auto end = m_childRenderNodes.cend();
		for (auto it = m_childRenderNodes.cbegin(); it != end; ++it)
		{
			(*it)->m_parentRenderNode = nullptr;
		}

		const auto rEnd = m_uiRenderables.cend();
		for (auto it = m_uiRenderables.cbegin(); it != rEnd; ++it)
		{
			(*it)->m_cachedRenderNode = nullptr;
		}

		const auto rTEnd = m_trasparentUIRenderables.cend();
		for (auto it = m_trasparentUIRenderables.cbegin(); it != rTEnd; ++it)
		{
			(*it)->m_cachedRenderNode = nullptr;
		}
	}

	void K_RenderNode::start()
	{
		getTransform().addParentListener(this);

		auto parent = getTransform().getParent();
		if (parent != nullptr)
		{
			m_parentRenderNode = parent->getAttachedGameObject().getComponent<K_RenderNode>();
			if (m_parentRenderNode == nullptr)
			{
				m_parentRenderNode = static_cast<K_RenderNode*>(K_ComponentManager::getInstance()->createComponent("K_RenderNode"));
				parent->getAttachedGameObject().addComponent(m_parentRenderNode);
			}
		}

		if (m_parentRenderNode != nullptr)
		{
			m_parentRenderNode->addChild(this);
		}
		else
		{
			puppy::Renderer::getInstance()->addUIToRender(this);
			m_isRendering = true;
		}
	}

	void K_RenderNode::onEnabled()
	{
		if (m_hasStarted)
		{
			if (m_parentRenderNode != nullptr)
			{
				m_parentRenderNode->addChild(this);
			}
			else
			{
				puppy::Renderer::getInstance()->addUIToRender(this);
				m_isRendering = true;
			}
		}
	}

	void K_RenderNode::onDisabled()
	{
		if (m_hasStarted)
		{
			m_disabledCalled = true;
			if (m_parentRenderNode != nullptr)
			{
				m_parentRenderNode->removeChild(this);
			}
			else
			{
				puppy::Renderer::getInstance()->removeUIFromRender(this);
				m_isRendering = false;
			}
		}
	}

	void K_RenderNode::addUIRenderable(K_UIRenderable* p_toAdd)
	{
		m_uiRenderables.insert(p_toAdd);
	}

	void K_RenderNode::removeUIRenderable(K_UIRenderable* p_toRemove)
	{
		auto found = m_uiRenderables.find(p_toRemove);
		if (found != m_uiRenderables.end())
		{
			m_uiRenderables.erase(found);
		}
		else
		{
			std::cout << "removeUIRenderable: could not find thing being removed!" << std::endl;
		}
	}

	void K_RenderNode::addTransparentUIRenderable(K_UIRenderable * p_toAdd)
	{
		m_trasparentUIRenderables.insert(p_toAdd);
	}

	void K_RenderNode::removeTransparentUIRenderable(K_UIRenderable * p_toRemove)
	{
		m_trasparentUIRenderables.erase(p_toRemove);
	}

	void K_RenderNode::addChild(K_RenderNode* p_toAdd)
	{
		m_childRenderNodes.insert(p_toAdd);
	}

	void K_RenderNode::removeChild(K_RenderNode* p_toRemove)
	{
		m_childRenderNodes.erase(p_toRemove);
	}

	void K_RenderNode::onParentChanged(Transform* p_newParent)
	{
		if (m_parentRenderNode != nullptr)
		{
			m_parentRenderNode->removeChild(this);
		}

		if (p_newParent != nullptr)
		{
			if (m_isRendering)
			{
				puppy::Renderer::getInstance()->removeUIFromRender(this);
				m_isRendering = false;
			}

			m_parentRenderNode = p_newParent->getAttachedGameObject().getComponent<K_RenderNode>();
			if (m_parentRenderNode == nullptr)
			{
				m_parentRenderNode = static_cast<K_RenderNode*>(K_ComponentManager::getInstance()->createComponent("K_RenderNode"));
				p_newParent->getAttachedGameObject().addComponent(m_parentRenderNode);
			}
		}
		else
		{
			assert(!m_isRendering);
			if (m_isEnabled) //our old parent was not null if our new parent is null -- don't have to check if we are already rendering
			{
				puppy::Renderer::getInstance()->addUIToRender(this);
				m_isRendering = true;
			}
		}
	}
}