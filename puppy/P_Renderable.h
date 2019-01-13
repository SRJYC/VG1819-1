#pragma once
#include "P_Common.h"
#include "Texture.h"

namespace puppy
{
	class P_Renderable
	{
	protected:
		void addToStaticRender(const Texture* p_texNeeded, TexturedVertex p_data[], int p_numElements);
		void addToDynamicRender();

		void removeFromStaticRender(const Texture* p_tex);
		void removeFromDynamicRender();
	public:
		virtual void render(const glm::mat4& p_viewProj) = 0;
	};
}
