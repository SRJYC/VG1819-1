#pragma once
#include "kitten\mouse picking\ClickableBox.h"
#include "kitten\K_Renderable.h"
#include "puppy\VertexEnvironment.h"
#include "puppy\Material.h"

class ClickableBoxRenderable : public kitten::K_Renderable
{
private:
	virtual void start() override;
	virtual void onEnabled() override;
	virtual void onDisabled() override;

	puppy::VertexEnvironment* m_vao;
	puppy::Material* m_mat;
public:
	ClickableBoxRenderable();
	~ClickableBoxRenderable();

	virtual void render(kitten::Camera* p_cam) override;
};