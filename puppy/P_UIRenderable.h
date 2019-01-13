#pragma once
#include "P_Common.h"
#include "Texture.h"

namespace puppy
{
	class P_UIRenderable
	{
	public:
		virtual void uiRender(const glm::mat4& p_ortho) = 0;

	protected:
		void addToStaticUIRender(const Texture* p_texNeeded, TexturedVertex p_data[], int p_numElements);
		void addToDynamicUIRender();

		void removeFromStaticUIRender(const Texture* p_tex);
		void removeFromDynamicUIRender();
	};
}