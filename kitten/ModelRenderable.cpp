#include "ModelRenderable.h"
#include "puppy\Material.h"
#include "puppy\P_Common.h"
#include "puppy\StaticRenderables.h"

std::unordered_map<std::string, puppy::P_Model*> ModelRenderable::m_loadedModels;

ModelRenderable::ModelRenderable(const char* p_pathToModel, bool p_flipUVs, bool p_isStatic) : m_isStatic(p_isStatic)
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
	if (m_isStatic)
	{
		auto& meshes = m_model->getMeshes();
		auto end = meshes.cend();
		for (auto it = meshes.begin(); it != end; ++it)
		{
			auto mesh = (*it);
			auto vertices = mesh->getVertices();

			puppy::StaticRenderables::putInWorldSpace(vertices.data(), vertices.size(), getTransform().getWorldTransform(), getTransform().getRotation());

			addToStaticRender(mesh->getMaterial(), vertices.data(), vertices.size());
		}
	}
	else
	{
		addToDynamicRender();
	}
}

void ModelRenderable::onDisabled()
{
	if (m_isStatic)
	{
		auto& meshes = m_model->getMeshes();
		auto end = meshes.cend();
		for (auto it = meshes.begin(); it != end; ++it)
		{
			auto mesh = (*it);
			removeFromStaticRender(mesh->getMaterial(), true);
		}
	}
	else
	{
		removeFromDynamicRender();
	}
}

void ModelRenderable::render(const glm::mat4& p_viewProj)
{
	m_model->render(p_viewProj * getTransform().getWorldTransform(), getTransform().getWorldIT(), getTransform().getWorldTransform());
}
