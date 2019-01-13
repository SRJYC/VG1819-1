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
	
	void StaticRenderables::addToAppropriateRender(const void* p_owner, const Texture* p_texNeeded, TexturedVertex p_data[], int p_numElements, bool p_isUI)
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

		auto found = texData->find(*p_texNeeded->getTex());

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
			texData->insert(std::make_pair(*p_texNeeded->getTex(), std::make_pair(newMap, true)));

			//Check if we need to create a copy of the texture
			auto textureFound = m_idToTex.find(*p_texNeeded->getTex());
			if (textureFound == m_idToTex.end())
			{
				puppy::Texture* tex = new puppy::Texture(p_texNeeded->getPath());
				m_idToTex.insert(std::make_pair(*tex->getTex(), tex));
			}	
		}
	}

	void StaticRenderables::addToRender(const void* p_owner, const Texture* p_texNeeded, TexturedVertex p_data[], int p_numElements)
	{
		addToAppropriateRender(p_owner, p_texNeeded, p_data, p_numElements, false);
	}

	void StaticRenderables::removeFromRender(const void* p_owner, const Texture* p_tex)
	{
		//Search for texture
		auto found = m_texturedData.find(*p_tex->getTex());
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

	void StaticRenderables::addToUIRender(const void* p_owner, const Texture* p_texNeeded, TexturedVertex p_data[], int p_numElements)
	{
		addToAppropriateRender(p_owner, p_texNeeded, p_data, p_numElements, true);
	}

	void StaticRenderables::removeFromUIRender(const void* p_owner, const Texture* p_tex)
	{
		//Search for texture
		auto found = m_texturedDataUI.find(*p_tex->getTex());
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

	void StaticRenderables::constructRenderable(GLuint p_where, render_map* p_from, std::unordered_map<Texture*, VertexEnvironment*>* p_toChange)
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

			auto foundRender = p_toChange->find(m_idToTex[p_where]);
			if (foundRender != p_toChange->end())
			{
				delete foundRender->second;
				foundRender->second = toRender;
			}
			else
			{
				p_toChange->insert(std::make_pair(m_idToTex[p_where], toRender));
			}
		}
		else
		{
			auto foundRender = p_toChange->find(m_idToTex[p_where]);
			if (foundRender != p_toChange->end())
			{
				delete foundRender->second;
				p_toChange->erase(m_idToTex[p_where]);
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

		renderStatic(m_toRender, p_cam->getViewProj());
		renderStatic(m_toRenderUI, p_cam->getOrtho());
	}

	void StaticRenderables::renderStatic(const std::unordered_map<Texture*, VertexEnvironment*>& p_toRender, const glm::mat4& p_viewProj)
	{
		auto end = p_toRender.cend();
		for (auto it = p_toRender.cbegin(); it != end; ++it)
		{
			//apply shaderProgram
			ShaderManager::applyShader(ShaderType::basic);

			//apply tex
			it->first->apply();

			//apply uniform (don't need a world matrix since everything should already be in world space)
			glUniformMatrix4fv(ShaderManager::getShaderProgram(ShaderType::basic)->getUniformPlace(WORLD_VIEW_PROJ_UNIFORM_NAME), 1, GL_FALSE, glm::value_ptr(p_viewProj));

			//Draw!
			it->second->drawArrays(GL_TRIANGLES);
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

		//Delete textures
		for (auto it = m_idToTex.begin(); it != m_idToTex.end(); it = m_idToTex.erase(it))
		{
			delete (*it).second;
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
}