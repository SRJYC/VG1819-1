#include "K_UIRenderable.h"
#include "K_RenderNode.h"
#include "K_ComponentManager.h"

#include "puppy\StaticRenderables.h"
#include "puppy\Renderer.h"

#include <iostream>

namespace kitten
{
	K_UIRenderable::K_UIRenderable() : m_cachedRenderNode(nullptr)
	{
		
	}

	K_UIRenderable::~K_UIRenderable()
	{
		if (m_cachedRenderNode != nullptr)
		{
			m_cachedRenderNode->removeUIRenderable(this);
		}
	}

	void K_UIRenderable::addToStaticUIRender(const puppy::Material& p_mat, puppy::TexturedVertex p_data[], int p_numElements)
	{
		puppy::StaticRenderables::getInstance()->addToUIRender(this, &p_mat, p_data, p_numElements);
	}

	void K_UIRenderable::addToDynamicUIRender()
	{
		if (m_cachedRenderNode == nullptr)
		{
			m_cachedRenderNode = m_attachedObject->getComponent<K_RenderNode>();

			if (m_cachedRenderNode == nullptr)
			{
				m_cachedRenderNode = static_cast<K_RenderNode*>(K_ComponentManager::getInstance()->createComponent("K_RenderNode"));
				m_attachedObject->addComponent(m_cachedRenderNode);
			}
		}

		m_cachedRenderNode->addUIRenderable(this);
	}

	void K_UIRenderable::addToDynamicTransparentUIRender()
	{
		if (m_cachedRenderNode == nullptr)
		{
			m_cachedRenderNode = m_attachedObject->getComponent<K_RenderNode>();

			if (m_cachedRenderNode == nullptr)
			{
				m_cachedRenderNode = static_cast<K_RenderNode*>(K_ComponentManager::getInstance()->createComponent("K_RenderNode"));
				m_attachedObject->addComponent(m_cachedRenderNode);
			}
		}

		m_cachedRenderNode->addTransparentUIRenderable(this);
	}

	void K_UIRenderable::removeFromStaticUIRender(const puppy::Material& p_mat)
	{
		puppy::StaticRenderables::getInstance()->removeFromUIRender(this, &p_mat);
	}

	void K_UIRenderable::removeFromDynamicUIRender()
	{
		if (m_cachedRenderNode != nullptr)
		{
			m_cachedRenderNode->removeUIRenderable(this);
			m_cachedRenderNode = nullptr;
		}
	}

	void K_UIRenderable::removeFromDynamicTransparentUIRender()
	{
		if (m_cachedRenderNode != nullptr)
		{
			m_cachedRenderNode->removeTransparentUIRenderable(this);
			m_cachedRenderNode = nullptr;
		}
	}
}