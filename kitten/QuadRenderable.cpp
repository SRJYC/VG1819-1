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
		onDisabled();

		if (--sm_instances == 0)
		{
			delete sm_vao;
		}
	}

	void QuadRenderable::addToStaticRender()
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

		K_Renderable::addToStaticRender(m_mat, verts, 6);
	}

	void QuadRenderable::setTexture(const char* p_pathToTex)
	{
		if (m_isStatic && m_isEnabled)
		{
			if (m_isRenderingStatic)
			{
				removeFromStaticRender(m_mat);
			}

			m_mat.setTexture(p_pathToTex);

			addToStaticRender();
			m_isRenderingStatic = true;
		}
		else
		{
			m_mat.setTexture(p_pathToTex);
		}
	}

	void QuadRenderable::addTexture(puppy::Texture* p_tex, const float& p_weight)
	{
		if (m_isStatic && m_isEnabled)
		{
			if (m_isRenderingStatic)
			{
				removeFromStaticRender(m_mat);
			}

			m_mat.addTexture(p_tex, p_weight);

			addToStaticRender();
			m_isRenderingStatic = true;
		}
		else
		{
			m_mat.addTexture(p_tex, p_weight);
		}
	}

	void QuadRenderable::removeTexture(puppy::Texture* p_tex)
	{
		if (m_isStatic && m_isEnabled)
		{
			if (m_isRenderingStatic)
			{
				removeFromStaticRender(m_mat);
			}

			m_mat.removeTexture(p_tex);

			addToStaticRender();
			m_isRenderingStatic = true;
		}
		else
		{
			m_mat.removeTexture(p_tex);
		}
	}

	void QuadRenderable::changeWeight(puppy::Texture* p_tex, const float& p_weight)
	{
		if (m_isStatic && m_isEnabled)
		{
			if (m_isRenderingStatic)
			{
				removeFromStaticRender(m_mat);
			}

			m_mat.changeWeight(p_tex, p_weight);

			addToStaticRender();
			m_isRenderingStatic = true;
		}
		else
		{
			m_mat.changeWeight(p_tex, p_weight);
		}
	}

	void QuadRenderable::start()
	{
		onEnabled();
	}

	void QuadRenderable::onDisabled()
	{
		if (m_isStatic && m_isRenderingStatic)
		{
			removeFromStaticRender(m_mat);
			m_isRenderingStatic = false;
		}
		else if (!m_isStatic)
		{
			removeFromDynamicRender();
		}
	}

	void QuadRenderable::onEnabled()
	{
		if (m_isStatic && m_mat.getNumberOfTextures() > 0)
		{
			addToStaticRender();
			m_isRenderingStatic = true;
		}
		else if (!m_isStatic)
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