#pragma once
#include "P_Common.h"
#include "Material.h"

namespace puppy
{
	class P_UIRenderable
	{
	public:
		virtual void uiRender(const glm::mat4& p_ortho) = 0;

	protected:
		void addToStaticUIRender(const Material& p_mat, TexturedVertex p_data[], int p_numElements);
		void addToDynamicUIRender();

		void removeFromStaticUIRender(const Material& p_mat);
		void removeFromDynamicUIRender();
	};
}