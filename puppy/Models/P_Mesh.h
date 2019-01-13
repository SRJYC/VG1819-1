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
		std::vector<NormalVertex> m_vertices;

		bool m_usesColour = false;
		const glm::vec4 m_colour;

		void setVertices(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices);
	public:
		P_Mesh(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices, const char* p_pathToTexture);
		P_Mesh(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices, glm::vec4& p_colour);
		~P_Mesh();

		const std::vector<NormalVertex>& getVertices() const;
		const Material& getMaterial() const;

		void render(const glm::mat4& p_worldViewProj, const glm::mat3& p_worldIT, const glm::mat4& p_world);
	};
}