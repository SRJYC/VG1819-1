#pragma once
#include "kitten/mouse picking/ClickableUI.h"
#include "_Project/LerpController.h"

class DragNDrop : public kitten::ClickableUI
{
protected:
	bool m_isDragging, m_backToOrigin= false;
	static bool m_focused;
	glm::vec3 m_origin;

	LerpController* m_lerpController = nullptr;
public:
	DragNDrop(bool p_backToOrigin = false);
	~DragNDrop();

	virtual void onClick() override;

	virtual void start() override;
	virtual void update() override;

	virtual void onDrop();

	glm::vec3& getOrigin() { return m_origin; }

	bool hasUpdate() const override { return true; }

};