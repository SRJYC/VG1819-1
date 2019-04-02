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
		std::vector<unsigned int> m_indices;

		bool m_usesColour = false;
		const glm::vec4 m_colour;
	public:
		P_Mesh(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices, const char* p_pathToTexture, bool p_useAlphaTest, const glm::vec4& p_matAmb);
		P_Mesh(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices, const glm::vec4& p_colour, const glm::vec4& p_matAmb);
		~P_Mesh();

		const std::vector<NormalVertex>& getVertices() const;
		const std::vector<unsigned int>& getIndices() const;
		const Material& getMaterial() const;

		void render(const glm::mat4& p_worldViewProj, const glm::mat3& p_worldIT, const glm::mat4& p_world);
	};
}