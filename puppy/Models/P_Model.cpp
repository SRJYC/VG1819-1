#include "P_Model.h"

#include <iostream>

namespace puppy
{
	P_Model::P_Model(const char* p_pathToModel, bool p_flipUVs) : m_name(p_pathToModel)
	{
		Assimp::Importer importer;
		
		int proccessing = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ImproveCacheLocality;
		
		if (p_flipUVs)
		{
			proccessing = proccessing | aiProcess_FlipUVs;
		}

		auto scene = importer.ReadFile(p_pathToModel, proccessing);
		
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			assert(false); //load failed
		}

		aiNode* rootNode = scene->mRootNode;
		
		processNode(rootNode, scene);
	}

	P_Model::~P_Model()
	{

	}

	void P_Model::processNode(aiNode* p_node, const aiScene* p_scene)
	{
		for (unsigned int i = 0; i < p_node->mNumMeshes; ++i)
		{
			auto mesh = p_scene->mMeshes[p_node->mMeshes[i]];
			processMesh(mesh, p_scene);
		}

		for (unsigned int i = 0; i < p_node->mNumChildren; ++i)
		{
			processNode(p_node->mChildren[i], p_scene);
		}
	}

	void P_Model::processMesh(aiMesh* p_mesh, const aiScene* p_scene)
	{
		std::vector<NormalVertex> vertices;
		std::vector<unsigned int> indices;
		const char* tex = nullptr;

		// Vertices
		vertices.reserve(p_mesh->mNumVertices);
		for (unsigned int i = 0; i < p_mesh->mNumVertices; ++i)
		{
			//There will always be a position vertex
			const auto& aiVertex = p_mesh->mVertices[i];

			//Normals and UVs are optional
			glm::vec3 normal(0, 0, 0);
			glm::vec2 uv(0, 0);

			if (p_mesh->HasNormals())
			{
				const auto& aiNormal = p_mesh->mNormals[i];
				normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
			}

			if (p_mesh->mTextureCoords[0])
			{
				const auto& aiUV = p_mesh->mTextureCoords[0][i];
				uv = glm::vec2(aiUV.x, aiUV.y);
			}

			if (p_mesh->HasVertexColors(i))
			{
				std::cout << p_mesh->mName.C_Str() << " uses vertex colors, but these are not supported (yet?)" << std::endl;
			}

			NormalVertex puppyVertex = 
			{
				//Position
				aiVertex.x, aiVertex.y, aiVertex.z, 
				//Normal
				normal.x, normal.y, normal.z,
				//UV
				uv.x, uv.y
			};

			vertices.push_back(puppyVertex);
		}

		// Indicies
		indices.reserve(p_mesh->mNumFaces);
		for (unsigned int i = 0; i < p_mesh->mNumFaces; ++i)
		{
			const auto& face = p_mesh->mFaces[i];
			
			for(unsigned int n = 0; n < face.mNumIndices; ++n)
			{
				indices.push_back(face.mIndices[n]);
			}
		}

		// Material(s)
		bool usesColour = false;
		aiColor3D color;

		if (p_mesh->mMaterialIndex >= 0)
		{
			auto material = p_scene->mMaterials[p_mesh->mMaterialIndex];

			int numDiffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
			if (numDiffuse > 0)
			{
				if (numDiffuse > 1)
				{
					std::cout << p_mesh->mName.C_Str() << " has more than one diffuse texture! Using the first..." << std::endl;
				}

				aiString texPath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
				tex = texPath.C_Str();
			}
			else
			{
				if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
				{
					usesColour = true;
				}
				else
				{
					std::cerr << "Could not get any diffuse for mesh: " << p_mesh->mName.C_Str() << std::endl;
				}
			}
		}
		else
		{
			std::cerr << "No material for found for mesh: " << p_mesh->mName.C_Str() << std::endl;
		}

		if (!usesColour)
		{
			m_meshes.push_back(new P_Mesh(vertices, indices, tex));
		}
		else
		{
			m_meshes.push_back(new P_Mesh(vertices, indices, glm::vec4(color.r, color.g, color.b, 1)));
		}
		
	}

	const std::vector<P_Mesh*> P_Model::getMeshes() const
	{
		return m_meshes;
	}

	void P_Model::render(const glm::mat4& p_worldViewProj, const glm::mat3& p_worldIT, const glm::mat4& p_world)
	{
		auto end = m_meshes.cend();
		for (auto it = m_meshes.cbegin(); it != end; ++it)
		{
			(*it)->render(p_worldViewProj, p_worldIT, p_world);
		}
	}
}