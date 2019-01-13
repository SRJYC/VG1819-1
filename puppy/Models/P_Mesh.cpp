#include "P_Mesh.h"

namespace puppy
{
	P_Mesh::P_Mesh(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices, const char* p_pathToTexture) : m_mat(ShaderType::basic_directional_light),
		m_vao(p_vertices.data(), p_indices.data(), ShaderManager::getShaderProgram(ShaderType::basic_directional_light), p_vertices.size(), p_indices.size())
	{
		if (p_pathToTexture != nullptr)
		{
			std::string newTexPath(p_pathToTexture);

			auto lastSlash = newTexPath.find_last_of('/');

			if (lastSlash > newTexPath.length())
			{
				lastSlash = newTexPath.find_last_of('\\');
			}

			if (lastSlash < newTexPath.length())
			{
				newTexPath = newTexPath.substr(lastSlash+1, newTexPath.length());
			}

			newTexPath = "models/textures/" + newTexPath;
			m_mat.setTexture(newTexPath.c_str());
		}
		else
		{
			m_mat.setTexture("textures/black.bmp");
		}

		// transfrom vertices + indices to just vertices for static rendering
		setVertices(p_vertices, p_indices);
	}

	P_Mesh::P_Mesh(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices, glm::vec4& p_colour) : m_mat(ShaderType::solid_color_directional_light),
		m_vao(p_vertices.data(), p_indices.data(), ShaderManager::getShaderProgram(ShaderType::solid_color_directional_light), p_vertices.size(), p_indices.size()), m_usesColour(true),
		m_colour(p_colour)
	{
		// transfrom vertices + indices to just vertices for static rendering
		setVertices(p_vertices, p_indices);
		m_mat.setColour(m_colour);
	}

	P_Mesh::~P_Mesh()
	{
		
	}

	void P_Mesh::setVertices(std::vector<NormalVertex>& p_vertices, std::vector<unsigned int>& p_indices)
	{
		auto indicesEnd = p_indices.cend();
		for (auto indIt = p_indices.cbegin(); indicesEnd != indIt; ++indIt)
		{
			m_vertices.push_back(p_vertices[*indIt]);
		}
	}

	const std::vector<NormalVertex>& P_Mesh::getVertices() const
	{
		return m_vertices;
	}

	const Material& P_Mesh::getMaterial() const
	{
		return m_mat;
	}

	void P_Mesh::render(const glm::mat4& p_worldViewProj, const glm::mat3& p_worldIT, const glm::mat4& p_world)
	{
		m_mat.apply();
		m_mat.setUniform(WORLD_VIEW_PROJ_UNIFORM_NAME, p_worldViewProj);
		m_mat.setUniform("matAmbient", glm::vec3(0.4, 0.4, 0.4));
		m_mat.setUniform("worldIT", p_worldIT);
		m_mat.setUniform("world", p_world);

		m_vao.drawArrays(GL_TRIANGLES);
	}
}