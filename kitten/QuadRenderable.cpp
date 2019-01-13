#include "QuadRenderable.h"
#include "puppy\P_Common.h"
#include "puppy\StaticRenderables.h"
#include "puppy\Renderer.h"
#include "K_ComponentManager.h"

namespace kitten
{
	puppy::VertexEnvironment* QuadRenderable::sm_vao = nullptr;
	int QuadRenderable::sm_instances = 0;

	QuadRenderable::QuadRenderable(const std::string& p_texPath, bool p_isStatic) : m_isStatic(p_isStatic), m_isRenderingStatic(false)
	{
		if (!p_texPath.empty())
		{
			m_mat.setTexture(p_texPath.c_str());
		}

		//If we have not initialized the vao yet
		if (sm_instances < 1)
		{
			//setup the vao
			puppy::TexturedVertex verts[] =
			{
			{ -0.5f, 0.0f, 0.5f,		0.0f, 0.0f },
			{ 0.5f, 0.0f, 0.5f,			0.0f, 1.0f },
			{ 0.5f, 0.0f,-0.5f,			1.0f, 1.0f },
			{ 0.5f, 0.0f,-0.5f,			1.0f, 1.0f },
			{ -0.5f, 0.0f,-0.5f,		1.0f, 0.0f },
			{ -0.5f, 0.0f, 0.5f,		0.0f, 0.0f },
			};
			sm_vao = new puppy::VertexEnvironment(verts, puppy::ShaderManager::getShaderProgram(puppy::ShaderType::colorTint_alphaTest), 6);
		}
		++sm_instances;
	}

	QuadRenderable::~QuadRenderable()
	{
		if (!m_isStatic)
		{
			if (--sm_instances == 0)
			{
				delete sm_vao;
			}

			if (m_isEnabled)
			{
				removeFromDynamicRender();
			}
		}
		else
		{
			if (m_isEnabled && m_isRenderingStatic)
			{
				removeFromStaticRender(m_staticTex);
			}
			else if(m_isEnabled)
			{
				removeFromDynamicRender();
			}
		}
	}

	void QuadRenderable::addToStaticRender()
	{
		if (m_mat.getNumberOfTextures() == 1)
		{
			puppy::TexturedVertex verts[] =
			{
				{ -0.5f, 0.0f, 0.5f,		0.0f, 0.0f },
				{ 0.5f, 0.0f, 0.5f,			0.0f, 1.0f },
				{ 0.5f, 0.0f,-0.5f,			1.0f, 1.0f },
				{ 0.5f, 0.0f,-0.5f,			1.0f, 1.0f },
				{ -0.5f, 0.0f,-0.5f,		1.0f, 0.0f },
				{ -0.5f, 0.0f, 0.5f,		0.0f, 0.0f },
			};

			//Transform into world space
			puppy::StaticRenderables::putInWorldSpace(verts, 6, getTransform().getWorldTransform());

			m_staticTex = m_mat.getOwnedTexture();

			K_Renderable::addToStaticRender(m_staticTex, verts, 6);
		}
	}

	void QuadRenderable::setTexture(const char* p_pathToTex)
	{
		m_mat.setTexture(p_pathToTex);

		if (m_isStatic && m_isEnabled)
		{
			if (m_isRenderingStatic && m_mat.getNumberOfTextures() > 1)
			{
				removeFromStaticRender(m_staticTex);
				addToDynamicRender();
				m_isRenderingStatic = false;
			}
			else if (m_hasStarted && m_mat.getNumberOfTextures() == 1)
			{
				m_isRenderingStatic = true;
				addToStaticRender(); //Started, but we were not added to static render because we had no texture
			}
		}
	}

	void QuadRenderable::addTexture(puppy::Texture* p_tex, const float& p_weight)
	{
		m_mat.addTexture(p_tex, p_weight);

		if (m_isStatic && m_isEnabled)
		{
			if (m_isRenderingStatic && m_mat.getNumberOfTextures() > 1)
			{
				removeFromStaticRender(m_staticTex);
				addToDynamicRender();
				m_isRenderingStatic = false;
			}
			else if (m_hasStarted && m_mat.getNumberOfTextures() == 1)
			{
				m_isRenderingStatic = true;
				addToStaticRender(); //Started, but we were not added to static render because we had no texture
			}
		}
	}

	void QuadRenderable::removeTexture(puppy::Texture* p_tex)
	{
		m_mat.removeTexture(p_tex);
		
		if (m_isStatic)
		{
			if (m_mat.getNumberOfTextures() == 1 && m_isEnabled)
			{
				removeFromDynamicRender();
				addToStaticRender();
				m_isRenderingStatic = true;
			}
		}
	}

	void QuadRenderable::changeWeight(puppy::Texture* p_tex, const float& p_weight)
	{
		m_mat.changeWeight(p_tex, p_weight);
	}

	void QuadRenderable::start()
	{
		if (m_isStatic && m_mat.getNumberOfTextures() == 1)
		{
			m_isRenderingStatic = true;
			addToStaticRender();
		}
		else if(!m_isStatic)
		{
			addToDynamicRender();
		}
	}

	void QuadRenderable::onDisabled()
	{
		if (m_isStatic)
		{
			if (m_mat.getNumberOfTextures() == 1)
			{
				removeFromStaticRender(m_staticTex);
			}
			else
			{
				removeFromDynamicRender();
			}
		}
		else
		{
			removeFromDynamicRender();
		}
	}

	void QuadRenderable::onEnabled()
	{
		if (m_isStatic)
		{
			if (m_mat.getNumberOfTextures() == 1)
			{
				addToStaticRender();
			}
			else
			{
				addToDynamicRender();
			}
		}
		else
		{
			addToDynamicRender();
		}
	}

	//Only called if not static
	void QuadRenderable::render(const glm::mat4& p_viewProj)
	{
		m_mat.apply();
		
		//Set world matrix
		glm::mat4 wvp = p_viewProj * getTransform().getWorldTransform();
		m_mat.setUniform(WORLD_VIEW_PROJ_UNIFORM_NAME, wvp);

		//render
		sm_vao->drawArrays(GL_TRIANGLES);
		
	}
}