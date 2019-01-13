#include "P_UIRenderable.h"
#include "StaticRenderables.h"
#include "Renderer.h"

namespace puppy
{
	void P_UIRenderable::addToStaticUIRender(const Material& p_mat, TexturedVertex p_data[], int p_numElements)
	{
		puppy::StaticRenderables::getInstance()->addToUIRender(this, &p_mat, p_data, p_numElements);
	}

	void P_UIRenderable::addToDynamicUIRender()
	{
		puppy::Renderer::getInstance()->addUIToRender(this);
	}

	void P_UIRenderable::removeFromStaticUIRender(const Material& p_mat)
	{
		puppy::StaticRenderables::getInstance()->removeFromUIRender(this, &p_mat);
	}

	void P_UIRenderable::removeFromDynamicUIRender()
	{
		puppy::Renderer::getInstance()->removeUIFromRender(this);
	}
}