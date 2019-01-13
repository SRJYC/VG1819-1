#include "ModelRenderable.h"

std::unordered_map<std::string, puppy::P_Model*> ModelRenderable::m_loadedModels;

ModelRenderable::ModelRenderable(const char* p_pathToModel, bool p_flipUVs)
{
	auto found = m_loadedModels.find(p_pathToModel);
	if (found != m_loadedModels.end())
	{
		m_model = (*found).second;
	}
	else
	{
		m_model = new puppy::P_Model(p_pathToModel, p_flipUVs);
		m_loadedModels.insert(std::make_pair(p_pathToModel, m_model));
	}
}

ModelRenderable::~ModelRenderable()
{
	if (m_isEnabled)
	{
		onDisabled();
	}
}

void ModelRenderable::start()
{
	onEnabled();
}

void ModelRenderable::onEnabled()
{
	addToDynamicRender();
}

void ModelRenderable::onDisabled()
{
	removeFromDynamicRender();
}

void ModelRenderable::render(const glm::mat4& p_viewProj)
{
	m_model->render(p_viewProj * getTransform().getWorldTransform(), getTransform().getWorldIT(), getTransform().getWorldTransform());
}
