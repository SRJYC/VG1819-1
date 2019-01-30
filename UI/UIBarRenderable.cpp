#include "UIBarRenderable.h"

namespace userinterface 
{
	std::map<UIElement::pivotType, puppy::VertexEnvironment*> UIElement::sm_vao;
	std::map<UIElement::pivotType, int> UIElement::sm_instances;

	UIBarRenderable::UIBarRenderable(const char* p_pathToTexture = nullptr) : UIElement(p_pathToTexture)
	{
	}


	UIBarRenderable::~UIBarRenderable()
	{
	}

	void UIBarRenderable::setUScale(const float& p_uScale)
	{
		m_uScale = 1 / p_uScale;
	}

	void UIBarRenderable::uiRender(kitten::Camera* p_cam)
	{
		auto& transform = getTransform();
		glm::vec3 centerPos = transform.getTranslation();
		centerPos.x += 0.5f * transform.getScale().x;

		m_mat->apply();

		m_mat->setUniform("mView", p_cam->getView());
		m_mat->setUniform("centerPos", centerPos);
		m_mat->setUniform("size", (glm::vec2)transform.getScale());
		m_mat->setUniform("mViewProj", p_cam->getOrtho() * p_cam->getView());

		m_mat->setUniform("xScale", getTransform().getLocalScale().x);
		m_mat->setUniform("uScale", m_uScale);

		//sm_vao[m_pivotType]->drawArrays(GL_TRIANGLES);

		sm_vao[m_pivotType]->drawArrays(GL_TRIANGLES);
	}
}
