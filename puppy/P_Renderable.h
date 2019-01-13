#pragma once
#include "P_Common.h"
#include "Material.h"

namespace puppy
{
	class P_Renderable
	{
	protected:
		void addToStaticRender(const Material& p_mat, const TexturedVertex p_data[], int p_numElements);
		void addToStaticRender(const Material& p_mat, const NormalVertex p_data[], int p_numElements);
		void addToDynamicRender();

		void removeFromStaticRender(const Material& p_mat, bool p_usedNormals = false);
		void removeFromDynamicRender();
	public:
		virtual void render(const glm::mat4& p_viewProj) = 0;
	};
}
