#pragma once

#include "kitten\K_Renderable.h"
#include "puppy\Models\P_Model.h"

#include <string>

class ModelRenderable : public kitten::K_Renderable
{
private:
	static std::unordered_map<std::string, puppy::P_Model*> m_loadedModels;
	puppy::P_Model* m_model;
	bool m_isStatic;

	virtual void start() override;
	virtual void onEnabled() override;
	virtual void onDisabled() override;

public:
	ModelRenderable(const char* p_pathToModel, bool p_flipUVS, bool p_isStatic);
	~ModelRenderable();

	virtual void render(const glm::mat4& p_viewProj);
};