/*
@Rock 12.10
This class create a gameobject contain a sprite 
to show the unit is currently active.
*/

#pragma once
#include "kitten/sprites/SpriteAnimator.h"

namespace unit
{
	class UnitAura : public kitten::K_Component
	{
	private:
		kitten::K_GameObject* m_auraGO;
		const float m_speed = 0.1;
	public:
		UnitAura();
		~UnitAura();

		void start() override;
		bool hasUpdate() const override;
		void update() override;
	};
}