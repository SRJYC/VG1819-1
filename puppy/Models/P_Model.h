#pragma once

#include "puppy\P_Common.h"
#include "P_Mesh.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <vector>
#include <string>

// Based on: https://learnopengl.com/Model-Loading/Model

namespace puppy
{
	class P_Model
	{
	private:
		std::vector<P_Mesh*> m_meshes;
		std::string m_name;

		void processNode(aiNode* p_node, const aiScene* p_scene, bool p_useAlphaTest, const glm::vec4& p_matAmb);
		void processMesh(aiMesh* mesh, const aiScene* p_scene, bool p_useAlphaTest, const glm::vec4& p_matAmb);
	public:
		P_Model(const char* p_pathToModel, bool p_flipUVs, bool p_useAlphaTest = false, const glm::vec4& p_matAmb = glm::vec4(0.4,0.4,0.4,1));
		~P_Model();

		const std::vector<P_Mesh*> getMeshes() const;

		virtual void render(const glm::mat4& p_worldViewProj, const glm::mat3& p_worldIT, const glm::mat4& p_world);
	};
}