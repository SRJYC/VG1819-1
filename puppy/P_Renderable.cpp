#include "P_Renderable.h"
#include "Renderer.h"
#include "StaticRenderables.h"

namespace puppy
{
	void P_Renderable::addToStaticRender(const Material& p_mat, TexturedVertex p_data[], int p_numElements)
	{
		puppy::StaticRenderables::getInstance()->addToRender(this, &p_mat, p_data, p_numElements);
	}

	void P_Renderable::addToDynamicRender()
	{
		puppy::Renderer::getInstance()->addToRender(this);
	}

	void P_Renderable::removeFromStaticRender(const Material& p_mat)
	{
		puppy::StaticRenderables::getInstance()->removeFromRender(this, &p_mat);
	}

	void P_Renderable::removeFromDynamicRender()
	{
		puppy::Renderer::getInstance()->removeFromRender(this);
	}
}