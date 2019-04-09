#pragma once
#include "kitten/K_Component.h"

class GameplayInit : public kitten::K_Component
{
private:
	bool m_testing = false;;

	void onDisabled() override;
public:
	void start();

	GameplayInit(bool p_testing);
	~GameplayInit();
};