#include "StaticRenderables.h"
#include "ShaderManager.h"

namespace puppy
{
	StaticRenderables* StaticRenderables::sm_instance = nullptr;

	StaticRenderables::StaticRenderables()
	{

	}

	StaticRenderables::~StaticRenderables()
	{
		clearAllData();
	}
	
	void StaticRenderables::addToAppropriateRender(const void* p_owner, Material* p_mat, const TexturedVertex p_data[], int p_numElements, bool p_isUI)
	{
		render_map *texData;
		if (p_isUI)
		{
			texData = &m_texturedDataUI;
		}
		else
		{
			texData = &m_texturedData;
		}

		auto found = texData->find(p_mat);

		std::vector<TexturedVertex> toInsert(p_data, p_data + p_numElements);

		if (found != texData->end())
		{
			//add to existing map
			auto& map = found->second.first;

			toInsert.insert(toInsert.begin(), p_data, p_data + p_numElements);

			map.insert(std::make_pair(p_owner, toInsert));

			//mark dirty
			found->second.second = true;
		}
		else
		{
			//make new map
			std::unordered_map<const void*, std::vector<TexturedVertex>> newMap;
			newMap.insert(std::make_pair(p_owner, toInsert));

			//insert map into data
			texData->insert(std::make_pair(p_mat, std::make_pair(newMap, true)));
		}
	}

	void StaticRenderables::addToAppropriateRender(const void* p_owner, Material* p_mat, const NormalVertex p_data[], int p_numElements)
	{
		auto found = m_normalData.find(p_mat);

		std::vector<NormalVertex> toInsert(p_data, p_data + p_numElements);

		if (found != m_normalData.end())
		{
			//add to existing map
			auto& map = found->second.first;

			toInsert.insert(toInsert.begin(), p_data, p_data + p_numElements);

			map.insert(std::make_pair(p_owner, toInsert));

			//mark dirty
			found->second.second = true;
		}
		else
		{
			//make new map
			std::unordered_map<const void*, std::vector<NormalVertex>> newMap;
			newMap.insert(std::make_pair(p_owner, toInsert));

			//insert map into data
			m_normalData.insert(std::make_pair(p_mat, std::make_pair(newMap, true)));
		}
	}

	void StaticRenderables::addToRender(const void* p_owner, const Material* p_mat, const TexturedVertex p_data[], int p_numElements)
	{
		Material* usingMat = getMatchingOwnedMaterial(p_mat);

		if (usingMat == nullptr)
		{
			usingMat = p_mat->clone();
			m_ownedMaterials.push_back(usingMat);
		}

		addToAppropriateRender(p_owner, usingMat, p_data, p_numElements, false);
	}

	void StaticRenderables::addToRender(const void* p_owner, const Material* p_mat, const NormalVertex p_data[], int p_numElements)
	{
		Material* usingMat = getMatchingOwnedMaterial(p_mat);

		if (usingMat == nullptr)
		{
			usingMat = p_mat->clone();
			m_ownedMaterials.push_back(usingMat);
		}

		addToAppropriateRender(p_owner, usingMat, p_data, p_numElements);
	}

	void StaticRenderables::removeFromRender(const void* p_owner, const Material* p_mat, bool p_usedNormals)
	{
		//Search for texture
		Material* ownedMat = getMatchingOwnedMaterial(p_mat);
		if (ownedMat != nullptr)
		{
			if (!p_usedNormals) // Unfortunately, these have to be pretty much copy/pasted
			{
				auto found = m_texturedData.find(ownedMat); // Because of what this line returns
				if (found != m_texturedData.end())
				{
					//Search for owner in texture's map
					auto& vecMap = (*found).second.first;
					auto ownerFound = vecMap.find(p_owner);
					if (ownerFound != vecMap.end())
					{
						vecMap.erase(ownerFound);
						//set dirty
						(*found).second.second = true;
					}
				}
			}
			else
			{
				auto found = m_normalData.find(ownedMat);
				if (found != m_normalData.end())
				{
					//Search for owner in texture's map
					auto& vecMap = (*found).second.first;
					auto ownerFound = vecMap.find(p_owner);
					if (ownerFound != vecMap.end())
					{
						vecMap.erase(ownerFound);
						//set dirty
						(*found).second.second = true;
					}
				}
			}
		}
	}

	void StaticRenderables::addToUIRender(const void* p_owner, const Material* p_mat, const TexturedVertex p_data[], int p_numElements)
	{
		Material* usingMat = getMatchingOwnedMaterial(p_mat);

		if (usingMat == nullptr)
		{
			usingMat = new Material(*p_mat);
			m_ownedMaterials.push_back(usingMat);
		}

		addToAppropriateRender(p_owner, usingMat, p_data, p_numElements, true);
	}

	void StaticRenderables::removeFromUIRender(const void* p_owner, const Material* p_mat)
	{
		Material* ownedMat = getMatchingOwnedMaterial(p_mat);
		if (ownedMat != nullptr)
		{
			auto found = m_texturedDataUI.find(ownedMat);
			if (found != m_texturedDataUI.end())
			{
				//Search for owner in texture's map
				auto& vecMap = (*found).second.first;
				auto ownerFound = vecMap.find(p_owner);
				if (ownerFound != vecMap.end())
				{
					vecMap.erase(ownerFound);
					//set dirty
					(*found).second.second = true;
				}
			}
		}
	}

	void StaticRenderables::constructRenderable(Material* p_where, render_map* p_from, std::unordered_map<Material*, VertexEnvironment*>* p_toChange)
	{
		//get vector to create buffer from
		auto found = p_from->find(p_where);
		auto map = found->second.first;

		//Get size of vector needed
		
		auto it = map.begin();
		auto end = map.cend();
		if (it != end)
		{
			std::vector<TexturedVertex>::size_type sum = (*it).second.size();

			while (it != end)
			{
				sum += (*it).second.size();
				++it;
			}

			//allocate in one go
			std::vector<TexturedVertex> createdData;
			createdData.reserve(sum);

			//insert into vector
			for (auto it = map.begin(); it != end; ++it)
			{
				auto vec = (*it).second;
				createdData.insert(createdData.end(), vec.begin(), vec.end());
			}

			//construct single buffer from data
			VertexEnvironment* toRender = new VertexEnvironment(createdData.data(),
				ShaderManager::getShaderProgram(ShaderType::basic), createdData.size());

			auto foundRender = p_toChange->find(p_where);
			if (foundRender != p_toChange->end())
			{
				delete foundRender->second;
				foundRender->second = toRender;
			}
			else
			{
				p_toChange->insert(std::make_pair(p_where, toRender));
			}
		}
		else
		{
			auto foundRender = p_toChange->find(p_where);
			if (foundRender != p_toChange->end())
			{
				delete foundRender->second;
				p_toChange->erase(p_where);
			}
		}

		//clean dirty flag
		found->second.second = false;
	}

	void StaticRenderables::constructNormalRenderable(Material* p_where)
	{
		//get vector to create buffer from
		auto found = m_normalData.find(p_where);
		auto map = found->second.first;

		//Get size of vector needed

		auto it = map.begin();
		auto end = map.cend();
		if (it != end)
		{
			std::vector<NormalVertex>::size_type sum = (*it).second.size();

			while (it != end)
			{
				sum += (*it).second.size();
				++it;
			}

			//allocate in one go
			std::vector<NormalVertex> createdData;
			createdData.reserve(sum);

			//insert into vector
			for (auto it = map.begin(); it != end; ++it)
			{
				auto vec = (*it).second;
				createdData.insert(createdData.end(), vec.begin(), vec.end());
			}

			//construct single buffer from data
			VertexEnvironment* toRender = new VertexEnvironment(createdData.data(),
				ShaderManager::getShaderProgram(ShaderType::basic_directional_light), createdData.size());

			auto foundRender = m_normalToRender.find(p_where);
			if (foundRender != m_normalToRender.end())
			{
				delete foundRender->second;
				foundRender->second = toRender;
			}
			else
			{
				m_normalToRender.insert(std::make_pair(p_where, toRender));
			}
		}
		else
		{
			auto foundRender = m_normalToRender.find(p_where);
			if (foundRender != m_normalToRender.end())
			{
				delete foundRender->second;
				m_normalToRender.erase(p_where);
			}
		}

		//clean dirty flag
		found->second.second = false;
	}

	void StaticRenderables::render(kitten::Camera* p_cam)
	{
		//Check for dirty data
		auto texEnd = m_texturedData.cend();
		for (auto it = m_texturedData.begin(); it != texEnd; ++it)
		{
			if (it->second.second)
			{
				constructRenderable(it->first, &m_texturedData, &m_toRender);
			}
		}

		texEnd = m_texturedDataUI.cend();
		for (auto it = m_texturedDataUI.begin(); it != texEnd; ++it)
		{
			if (it->second.second)
			{
				constructRenderable(it->first, &m_texturedDataUI, &m_toRenderUI);
			}
		}

		auto normEnd = m_normalData.cend();
		for (auto it = m_normalData.cbegin(); it != normEnd; ++it)
		{
			if (it->second.second)
			{
				constructNormalRenderable(it->first);
			}
		}

		// Render
		renderStatic(m_toRender, p_cam->getViewProj());
		renderStatic(m_toRenderUI, p_cam->getOrtho());
		renderNormaled(p_cam->getViewProj());
	}

	void StaticRenderables::renderStatic(const std::unordered_map<Material*, VertexEnvironment*>& p_toRender, const glm::mat4& p_viewProj)
	{
		auto end = p_toRender.cend();
		for (auto it = p_toRender.cbegin(); it != end; ++it)
		{
			auto& pair = (*it);
			auto& mat = pair.first;
			auto& vertices = pair.second;

			mat->apply();
			mat->setUniform(WORLD_VIEW_PROJ_UNIFORM_NAME, p_viewProj);

			vertices->drawArrays(GL_TRIANGLES);
		}
	}

	void StaticRenderables::renderNormaled(const glm::mat4& p_viewProj) const
	{
		auto end = m_normalToRender.cend();
		for (auto it = m_normalToRender.cbegin(); it != end; ++it)
		{
			auto& pair = (*it);
			auto& mat = pair.first;
			auto& vertices = pair.second;

			mat->apply();
			mat->setUniform(WORLD_VIEW_PROJ_UNIFORM_NAME, p_viewProj);

			mat->setUniform("world", glm::mat4());
			mat->setUniform("worldIT", glm::mat3());
			mat->setUniform("matAmbient", glm::vec3(0.4, 0.4, 0.4));

			vertices->drawArrays(GL_TRIANGLES);
		}
	}

	void StaticRenderables::clearAllData()
	{
		//Delete all existing renderables
		for (auto it = m_toRender.begin(); it != m_toRender.end(); it = m_toRender.erase(it))
		{
			delete (*it).second;
		}
		
		for (auto it = m_toRenderUI.begin(); it != m_toRenderUI.end(); it = m_toRenderUI.erase(it))
		{
			delete (*it).second;
		}
		
		for (auto it = m_ownedMaterials.begin(); it != m_ownedMaterials.end(); it = m_ownedMaterials.erase(it))
		{
			delete (*it);
		}

		//Clear everything else
		m_texturedData.clear();
	}

	//Static method for putting vertices in world space
	void StaticRenderables::putInWorldSpace(TexturedVertex p_toTransform[], int p_numElements, const glm::mat4& p_worldMat)
	{
		glm::vec4 tempVec(0,0,0,1);
		for (int i = 0; i < p_numElements; ++i)
		{
			TexturedVertex& currentVert = p_toTransform[i];
			tempVec.x = currentVert.x;
			tempVec.y = currentVert.y;
			tempVec.z = currentVert.z;
			
			tempVec = p_worldMat * tempVec;
			
			currentVert.x = tempVec.x;
			currentVert.y = tempVec.y;
			currentVert.z = tempVec.z;
		}
	}

	void StaticRenderables::putInWorldSpace(NormalVertex p_toTransform[], int p_numElements, const glm::mat4& p_worldMat, const glm::quat& p_rotation)
	{
		glm::vec4 tempVec(0, 0, 0, 1);
		for (int i = 0; i < p_numElements; ++i)
		{
			NormalVertex& currentVert = p_toTransform[i];
			// Coordinates
			tempVec.x = currentVert.x;
			tempVec.y = currentVert.y;
			tempVec.z = currentVert.z;

			tempVec = p_worldMat * tempVec;

			currentVert.x = tempVec.x;
			currentVert.y = tempVec.y;
			currentVert.z = tempVec.z;

			// Normals
			tempVec.x = currentVert.nx;
			tempVec.y = currentVert.ny;
			tempVec.z = currentVert.nz;

			tempVec = p_rotation * tempVec;

			currentVert.nx = tempVec.x;
			currentVert.ny = tempVec.y;
			currentVert.nz = tempVec.z;
		}
	}
}
