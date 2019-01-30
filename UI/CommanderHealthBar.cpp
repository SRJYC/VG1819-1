#include "CommanderHealthBar.h"

#include "unit\Unit.h"
#include "kitten\K_GameObject.h"
#include "kitten\K_GameObjectManager.h"
#include "kitten\K_ComponentManager.h"

#include "UI\UIBarRenderable.h"

namespace unit {
	CommanderHealthBar::CommanderHealthBar(unit::Unit* p_commander, kitten::K_GameObject* p_uie, float p_lerpTimeScalar = 4.0f)
		: m_attachedCommander(p_commander), m_oldHealthPercent(1.0), m_lerpTimeScalar(p_lerpTimeScalar),
		m_healthMat(puppy::ShaderType::basic)
	{

	}

	CommanderHealthBar::~CommanderHealthBar()
	{
	}

	void CommanderHealthBar::start()
	{
		//bar is made of background and foreground
		kitten::K_GameObject* background = kitten::K_GameObjectManager::getInstance()->createNewGameObject();
		kitten::K_GameObject* foreground = kitten::K_GameObjectManager::getInstance()->createNewGameObject();

		glm::vec3 comScale = m_attachedCommander->getTransform().getScale();
		glm::vec3 comTrans = m_attachedCommander->getTransform().getTranslation();

		background->getTransform().setIgnoreParent(false);
		foreground->getTransform().setIgnoreParent(false);
		background->getTransform().setParent(&getTransform());
		foreground->getTransform().setParent(&getTransform());

		background->getTransform().scale2D(comScale.x, BACKGROUND_BAR_Y_SCALE);
		foreground->getTransform().scale2D(comScale.x - BACKGROUND_BAR_X_DIFFERENCE, comScale.x - BACKGROUND_BAR_Y_DIFFERENCE);

		background->getTransform().place(comTrans.x, comTrans.y - comScale.y, 0.005);
		foreground->getTransform().place(comTrans.x, +BACKGROUND_BAR_X_DIFFERENCE / 2, 0.01);

		auto compMan = kitten::K_ComponentManager::getInstance();

		userinterface::UIBarRenderable* backgroundBar = static_cast<userinterface::UIBarRenderable*>(compMan->createComponent("UIBarRenderable"));
		backgroundBar->setTexture(DAMAGE_MAT_TEXTURE_PATH);
		background->addComponent(backgroundBar);

		m_contextBar = static_cast<userinterface::UIBarRenderable*>(compMan->createComponent("UIBarRenderable"));

		if (m_attachedCommander->m_clientId)
		{
			m_contextBar->setTexture(HEALTH_MAT_P1_TEXTURE_PATH);
		}
		else {
			m_contextBar->setTexture(HEALTH_MAT_P2_TEXTURE_PATH);
		}

		m_contextBar->setUScale(BAR_X_SCALE);
		foreground->addComponent(m_contextBar);

		m_contextLerpController = static_cast<LerpController*>(compMan->createComponent("LerpController"));
		foreground->addComponent(m_contextLerpController);

		backgroundBar->setUScale(BACKGROUND_BAR_X_SCALE);
	}


	void CommanderHealthBar::updateBar()
	{
		int currentHealth = m_attachedCommander->m_attributes[UNIT_HP];
		int maxHealth = m_attachedCommander->m_attributes[UNIT_MAX_HP];

		float percentFull = (float)currentHealth / (float)maxHealth;

		if (percentFull != m_oldHealthPercent)
		{
			//scale foreground
			const glm::vec3& currentScale = getTransform().getScale();
			m_contextLerpController->scaleLerp(glm::vec3(BAR_X_SCALE*percentFull, BAR_Y_SCALE, currentScale.z), abs(m_oldHealthPercent - percentFull) * m_lerpTimeScalar);

			m_oldHealthPercent = percentFull;
		}
	}

	LerpController* CommanderHealthBar::getForegroundBarLerpController() const
	{
		return m_contextLerpController;
	}
}