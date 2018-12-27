#include "UIElement.h"
#include "puppy\Renderer.h"
#include "puppy\StaticRenderables.h"

#define DEFAULT_TEXTURE "textures/ui/blankFrame.tga"

namespace userinterface
{ 
	std::map<UIElement::pivotType, puppy::VertexEnvironment*> UIElement::sm_vao;
	std::map<UIElement::pivotType, int> UIElement::sm_instances;

	UIElement::UIElement(const char* p_pathToTex)
	{
		m_texPath = p_pathToTex;
		m_tex = new puppy::Texture(p_pathToTex);
		m_mat = new puppy::Material(puppy::ShaderType::alphaTest);
		if (p_pathToTex != nullptr)
		{
			m_mat->setTexture(p_pathToTex);
		}
		else {
			m_mat->setTexture(DEFAULT_TEXTURE);
			m_texPath = DEFAULT_TEXTURE;
		}
		m_texBehaviour = tbh_Stretch;
		m_pivotType = piv_BotLeft;

		m_isEnabled = true;

	}

	UIElement::UIElement(const char* p_pathToTex, pivotType p_pivot, textureBehaviour p_texBehaviour)
	{
		m_texPath = p_pathToTex;
		m_tex = new puppy::Texture(p_pathToTex);
		m_mat = new puppy::Material(puppy::ShaderType::alphaTest);
		if (p_pathToTex != nullptr)
		{
			m_mat->setTexture(p_pathToTex);
		}
		else {
			m_mat->setTexture(DEFAULT_TEXTURE);
			m_texPath = DEFAULT_TEXTURE;
		}

		m_texBehaviour = p_texBehaviour;
		m_pivotType = p_pivot;

		m_isEnabled = true;

	}

	UIElement::~UIElement()
	{
		delete m_mat;
		if (--sm_instances[m_pivotType] == 0)
		{
			delete sm_vao[m_pivotType];
			sm_vao[m_pivotType] = nullptr;
		}
		if (m_isEnabled)
		{
			removeFromDynamicUIRender();
		}
	}

	void UIElement::start()
	{
		//empty to not force override
	}
	
	void UIElement::defineVerts()
	{

		//quad coords (ortho)
		float xmin, ymin, xmax, ymax, z, u, v;
		z = 0.0;

		//change UV coords to meet type expectation
		switch (m_texBehaviour)
		{

			//TO-DO. 
			/*
			There must be a good way to set the UV's on creation
			of this object so that you can have the texture repeat
			if the object is particularily large to avoid stretching
			the texture out. Maybe just have a param that tells
			the frame how many times to tile the texture in both dimensions
			*/

		case tbh_Stretch: {
			u = 1.0;
			v = 1.0;
			m_tex->setWrapping(GL_CLAMP_TO_EDGE);
			break;
		};

		case tbh_Repeat: {
			u = 1.0;
			v = 1.0;
			m_tex->setWrapping(GL_REPEAT);
			break;
		};

		case tbh_RepeatMirrored: {
			u = 1.0f;
			v = 1.0f;
			m_tex->setWrapping(GL_MIRRORED_REPEAT);
			break;
		};
		}

		switch (m_pivotType)
		{
		case piv_Left: {
			xmin = 0.0f;
			ymin = -0.5f;
			xmax = 1.0f;
			ymax = 0.5f;
			break;
		}
		case piv_Right: {
			xmin = -1.0f;
			ymin = -0.5f;
			xmax = 0.0f;
			ymax = 0.5f;
			break;
		}
		case piv_Bot: {
			xmin = -0.5f;
			ymin = 0.0f;
			xmax = 0.5f;
			ymax = 1.0f;
			break;
		}
		case piv_Top: {
			xmin = -0.5f;
			ymin = -1.0f;
			xmax = 0.5f;
			ymax = 0.0f;
			break;
		}
		case piv_BotLeft: {
			xmin = 0.0f;
			ymin = 0.0f;
			xmax = 1.0f;
			ymax = 1.0f;
			break;
		}
		case piv_BotRight: {
			xmin = -1.0f;
			ymin = 0.0f;
			xmax = 0.0f;
			ymax = 1.0f;
			break;
		}
		case piv_TopLeft: {
			xmin = 0.0f;
			ymin = -1.0f;
			xmax = 1.0f;
			ymax = 0.0f;
			break;
		}
		case piv_TopRight: {
			xmin = -1.0f;
			ymin = -1.0f;
			xmax = 0.0f;
			ymax = 0.0f;
			break;
		}
		case piv_Center: {
			xmin = -0.5;
			ymin = -0.5;
			xmax = 0.5;
			ymax = 0.5;
			break;
		}
		}

		puppy::TexturedVertex verts[] =
		{
			//a nice lil quad that takes the pivot into account
			{ xmin, ymin, z, 0.0,  0.0 },
			{ xmax, ymin, z, u,    0.0 },
			{ xmax, ymax, z, u,    v },
			{ xmax, ymax, z, u,    v },
			{ xmin,	ymax, z, 0.0f, v },
			{ xmin, ymin, z, 0.0f, 0.0f },
		};

		if (sm_vao[m_pivotType] == nullptr)
		{
			sm_vao[m_pivotType] = new puppy::VertexEnvironment(verts, puppy::ShaderManager::getShaderProgram(puppy::ShaderType::alphaTest), 6);
		}

		sm_instances[m_pivotType]++;

		this->addToDynamicUIRender();
	}

		void UIElement::onDisabled()
		{
			removeFromDynamicUIRender();
		}

		void UIElement::onEnabled()
		{
			addToDynamicUIRender();
		}

		void UIElement::uiRender(const glm::mat4& p_ortho)
		{
			m_mat->apply();

			glm::mat4 wvp = p_ortho * getTransform().getWorldTransform();
			m_mat->setUniform(WORLD_VIEW_PROJ_UNIFORM_NAME, wvp);

			sm_vao[m_pivotType]->drawArrays(GL_TRIANGLES);
		}

		void UIElement::setTexture(const char* p_pathToTex)
		{
			//delete m_tex;
			//m_tex = new puppy::Texture(p_pathToTex);

			m_mat->setTexture(p_pathToTex);
			m_texPath = p_pathToTex;
		}

		void UIElement::setPivotType(const pivotType p_piv)
		{
			m_pivotType = p_piv;
		}

		void UIElement::setTexBehaviour(const textureBehaviour p_tb)
		{
			m_texBehaviour = p_tb;
		}
	}
