#pragma once

#include "puppy\P_Common.h"
#include "puppy\Texture.h"
#include "puppy\Material.h"
#include "puppy\VertexEnvironment.h"

#include <list>



namespace puppy
{
	class P_Mesh
	{
	private:
		Material m_mat;
		VertexEnvironment m_vao;

	public:
		P_Mesh(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices, const char* p_pathToTexture);
		~P_Mesh();

		void render(const glm::mat4& p_worldViewProj);
	};
}