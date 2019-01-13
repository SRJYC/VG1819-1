#pragma once

#include "P_Common.h"
#include "Texture.h"
#include "Material.h"
#include "VertexEnvironment.h"
#include "kitten\Camera.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace puppy
{
	class P_Instance;
	class P_UIRenderable;
	class P_Renderable;

	/*
		@TODO: add support for vertex's with normals
	*/
	class StaticRenderables
	{
		friend class P_Instance;
		friend class P_UIRenderable;
		friend class P_Renderable;
	private:
		typedef std::unordered_map<Material*, std::pair<std::unordered_map<const void*, std::vector<TexturedVertex>>, bool>> render_map;
		typedef std::unordered_map<Material*, std::pair<std::unordered_map<const void*, std::vector<NormalVertex>>, bool>> normalRender_map;

		//Singleton stuff
		StaticRenderables();
		virtual ~StaticRenderables();
		static StaticRenderables* sm_instance;
		static void createInstance() { assert(sm_instance == nullptr); sm_instance = new StaticRenderables(); };
		static void destroyInstance() { assert(sm_instance != nullptr); delete sm_instance; sm_instance = nullptr; };
	
		std::unordered_map<Material*, VertexEnvironment*> m_toRender;
		std::unordered_map<Material*, VertexEnvironment*> m_normalToRender;
		render_map m_texturedData;
		normalRender_map m_normalData;

		std::unordered_map<Material*, VertexEnvironment*> m_toRenderUI;
		render_map m_texturedDataUI;

		std::vector<Material*> m_ownedMaterials;
		//std::unordered_map<GLuint, Texture*> m_idToTex;
		
		/*
		addToRender() takes the texture needed to render the object and the
		vertex data needed.  Assumes the vertex data has already been transformed
		into world space. This data is then later combined into a single draw call.
		*/
		void addToRender(const void* p_owner, const Material* p_mat, const TexturedVertex p_data[], int p_numElements);
		void addToRender(const void* p_owner, const Material* p_mat, const NormalVertex p_data[], int p_numElements);
		void removeFromRender(const void* p_owner, const Material* p_mat, bool p_usedNormals = false);

		void addToUIRender(const void* p_owner, const Material* p_mat, const TexturedVertex p_data[], int p_numElements);
		void removeFromUIRender(const void* p_owner, const Material* p_mat);

		/*
			Helper methods to construct TexturedVertex's into
			one draw call
		*/
		void constructRenderable(Material* p_where, render_map* p_from, std::unordered_map<Material*, VertexEnvironment*>* p_toChange);
		void constructNormalRenderable(Material* p_where);

		void addToAppropriateRender(const void* p_owner, Material* p_mat, const TexturedVertex p_data[], int p_numElements, bool p_isUi);
		void addToAppropriateRender(const void* p_owner, Material* p_mat, const NormalVertex p_data[], int p_numElements);

		//Helper method to reduce code duplication
		void renderStatic(const std::unordered_map<Material*, VertexEnvironment*>& p_toRender, const glm::mat4& p_viewProj);
		void renderNormaled(const glm::mat4& p_viewProj) const;

		inline Material* getMatchingOwnedMaterial(const Material* p_mat) const
		{
			auto end = m_ownedMaterials.cend();
			for (auto it = m_ownedMaterials.begin(); it != end; ++it)
			{
				if (*p_mat == *(*it))
				{
					return (*it);
				}
			}
			return nullptr;
		}

	public:
		//Singleton stuff
		static StaticRenderables* getInstance() { return sm_instance; };

		void render(kitten::Camera* p_cam);

		void clearAllData();

		//Method for things wanting to be in world space
		static void putInWorldSpace(TexturedVertex p_toTransform[], int p_numElements, const glm::mat4& p_worldMat);
		static void putInWorldSpace(NormalVertex p_toTransform[], int p_numElements, const glm::mat4& p_worldMat, const glm::quat& p_rotation);
	};
}
