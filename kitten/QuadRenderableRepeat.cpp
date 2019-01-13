#include "QuadRenderableRepeat.h"
#include "puppy\P_Common.h"
#include "puppy\StaticRenderables.h"
#include "puppy\Renderer.h"
#include "K_ComponentManager.h"

namespace kitten
{
	QuadRenderableRepeat::QuadRenderableRepeat(const std::string& p_texPath, bool p_isStatic, bool p_texRepeat, GLfloat p_uRepeat, GLfloat p_vRepeat)
		:
		m_isStatic(p_isStatic),
		m_isRenderingStatic(false),
		m_texRepeat(p_texRepeat),
		m_uRepeat(p_uRepeat),
		m_vRepeat(p_vRepeat)
	{
		if (!p_texPath.empty())
		{
			m_mat.setTexture(p_texPath.c_str());
		}

		//setup the vao
		puppy::TexturedVertex verts[] =
		{
			{ -0.5f, 0.0f, 0.5f,		0.0f,	   0.0f },
			{ 0.5f, 0.0f, 0.5f,			0.0f,	   m_vRepeat },
			{ 0.5f, 0.0f,-0.5f,			m_uRepeat, m_vRepeat },
			{ 0.5f, 0.0f,-0.5f,			m_uRepeat, m_vRepeat },
			{ -0.5f, 0.0f,-0.5f,		m_uRepeat, 0.0f },
			{ -0.5f, 0.0f, 0.5f,		0.0f,	   0.0f },
		};
		m_vao = new puppy::VertexEnvironment(verts, puppy::ShaderManager::getShaderProgram(puppy::ShaderType::texture_blend_zero), 6);
	}

	QuadRenderableRepeat::~QuadRenderableRepeat()
	{
		onDisabled();
	}

	void QuadRenderableRepeat::addToStaticRender()
	{
		puppy::TexturedVertex verts[] =
		{
			{ -0.5f, 0.0f, 0.5f,		0.0f, 0.0f },
			{ 0.5f, 0.0f, 0.5f,			0.0f, m_vRepeat },
			{ 0.5f, 0.0f,-0.5f,			m_uRepeat, m_vRepeat },
			{ 0.5f, 0.0f,-0.5f,			m_uRepeat, m_vRepeat },
			{ -0.5f, 0.0f,-0.5f,		m_uRepeat, 0.0f },
			{ -0.5f, 0.0f, 0.5f,		0.0f, 0.0f },
		};

		//Transform into world space
		puppy::StaticRenderables::putInWorldSpace(verts, 6, getTransform().getWorldTransform());

		K_Renderable::addToStaticRender(m_mat, verts, 6);
	}

	void QuadRenderableRepeat::setTexture(const char* p_pathToTex)
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

	void QuadRenderableRepeat::addTexture(puppy::Texture* p_tex, const float& p_weight)
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

	void QuadRenderableRepeat::removeTexture(puppy::Texture* p_tex)
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

	void QuadRenderableRepeat::changeWeight(puppy::Texture* p_tex, const float& p_weight)
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

	void QuadRenderableRepeat::start()
	{
		onEnabled();
	}

	void QuadRenderableRepeat::onDisabled()
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

	void QuadRenderableRepeat::onEnabled()
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
	void QuadRenderableRepeat::render(const glm::mat4& p_viewProj)
	{
		m_mat.apply();

		//Set world matrix
		glm::mat4 wvp = p_viewProj * getTransform().getWorldTransform();
		m_mat.setUniform(WORLD_VIEW_PROJ_UNIFORM_NAME, wvp);

		//render
		m_vao->drawArrays(GL_TRIANGLES);

	}
}