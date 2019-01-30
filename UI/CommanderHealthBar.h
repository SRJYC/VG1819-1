#pragma once
#include "puppy\VertexEnvironment.h"
#include "puppy\Material.h"
#include "kitten\K_Component.h"
#include "_Project\LerpController.h"
#include "kitten\BarRenderable.h"
#include "kitten\K_GameObject.h"
#include "unit\UnitCommon.h"
#include "UI\UIBarRenderable.h"

#define HEALTH_MAT_P1_TEXTURE_PATH "textures/ui/saphire.png"
#define HEALTH_MAT_P2_TEXTURE_PATH "textures/ui/topaz.png"

#define DAMAGE_MAT_TEXTURE_PATH "textures/ui/healthbar_back.png"

#define BAR_Y_SCALE 10

#define BACKGROUND_BAR_X_DIFFERENCE 2
#define BACKGROUND_BAR_Y_DIFFERENCE 2


//austin's commander hp bar, copied mostly from cal's unit hp bar
namespace unit {

	class Unit;
	class CommanderHealthBar : public kitten::K_Component
	{
	private:
		puppy::Material m_healthMat;
		Unit* m_attachedCommander;
		float m_oldHealthPercent;

		float m_lerpTimeScalar = 4.0f;

		userinterface::UIBarRenderable* m_contextBar;
		LerpController* m_contextLerpController;

		const glm::vec3 m_offset;

		virtual void start() override;

	public:
		CommanderHealthBar(unit::Unit* p_commander, kitten::K_GameObject* p_uie, float p_lerpTimeScalar = 4.0f);
		~CommanderHealthBar();

		void updateBar();

		LerpController* getForegroundBarLerpController() const;


	};
}