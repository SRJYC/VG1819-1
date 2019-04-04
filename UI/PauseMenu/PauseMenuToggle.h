#pragma once
#include "kitten\K_Component.h"
#include "kitten\InputManager.h"

class PauseMenuToggle : public kitten::K_Component
{
private:
	int m_key;
	input::InputManager* m_input;
	kitten::K_GameObject* m_sibling;

	virtual void start() override;
	virtual bool hasUpdate() const override { return true; };
	virtual void update() override;

public:
	PauseMenuToggle();
	~PauseMenuToggle();	
};