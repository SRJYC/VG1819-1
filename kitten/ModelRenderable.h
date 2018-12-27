#pragma once

#include "kitten\K_Renderable.h"
#include "puppy\Models\P_Model.h"

#include <string>

class ModelRenderable : public kitten::K_Renderable
{
private:
	static std::unordered_map<std::string, puppy::P_Model*> m_loadedModels;
	puppy::P_Model* m_model;

	virtual void start() override;
	virtual void onEnabled() override;
	virtual void onDisabled() override;

public:
	ModelRenderable(const char* p_pathToModel);
	~ModelRenderable();

	virtual void render(const glm::mat4& p_viewProj);
};