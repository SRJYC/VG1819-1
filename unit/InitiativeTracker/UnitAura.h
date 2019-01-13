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
		const float m_speed;
		const float m_max;
		const float m_min;
		float m_scale;
		bool m_pos;
	public:
		UnitAura(float p_speed=0.001, float p_max=1.2, float p_min=0.8);
		~UnitAura();

		void start() override;
		bool hasUpdate() const override;
		void update() override;
	};
}