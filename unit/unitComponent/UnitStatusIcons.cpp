#include "UnitStatusIcons.h"
#include "kitten\K_GameObjectManager.h"
#include "kitten\K_Time.h"

#define ICON_X_SCALE 0.20f
#define ICON_Y_SCALE 0.20f

#define BUFF_ICON_TEXTURE "textures/ui/icons/buff.png"
#define DEBUFF_ICON_TEXTURE "textures/ui/icons/debuff.png"
#define SHIELD_ICON_TEXTURE "textures/ui/icons/shield_icon.png"

namespace unit
{
	UnitStatusIcons::UnitStatusIcons(const glm::vec3& p_offset, float p_rotation)
		:
		m_offset(p_offset), m_rotation(p_rotation)
	{

	}

	UnitStatusIcons::~UnitStatusIcons()
	{

	}

	void UnitStatusIcons::start()
	{
		kitten::K_GameObject* statusIcon = kitten::K_GameObjectManager::getInstance()->createNewGameObject();
		statusIcon->getTransform().setIgnoreParent(false);
		statusIcon->getTransform().setParent(&getTransform());

		statusIcon->getTransform().scaleAbsolute(ICON_X_SCALE, ICON_Y_SCALE, 1.0f);
		statusIcon->getTransform().move(m_offset.x, m_offset.y + 0.1f, m_offset.z + -0.004f + 0.1f);
		statusIcon->getTransform().rotateAbsolute(glm::vec3(m_rotation, 0, 0));

		auto compMan = kitten::K_ComponentManager::getInstance();

		m_statusIcon = static_cast<kitten::QuadRenderableBB*>(compMan->createComponent("QuadRenderableBB"));
		m_statusIcon->setTexture(BUFF_ICON_TEXTURE);
		statusIcon->addComponent(m_statusIcon);

		m_unit = m_attachedObject->getComponent<Unit>();
		setEnabled(false);
	}

	void UnitStatusIcons::onEnabled()
	{
		m_statusIcon->setTexture(m_iconTexturePaths.back().c_str());
		m_statusIcon->setEnabled(true);
	}

	void UnitStatusIcons::onDisabled()
	{
		m_statusIcon->setEnabled(false);
	}

	void UnitStatusIcons::update()
	{
		m_timeElapsed -= kitten::K_Time::getInstance()->getDeltaTime();

		if (m_timeElapsed < 0.0f)
		{
			m_timeElapsed = ICON_SWAP_DELAY;

			if (m_iconTexturePaths.size() == 0) return;
			
			// Get the the texture at the front of the list
			std::string texturePath = m_iconTexturePaths.front();
			m_statusIcon->setTexture(texturePath.c_str());

			// Remove the texture at the front and add it to the back so we cycle through the list
			m_iconTexturePaths.pop_front();
			m_iconTexturePaths.push_back(texturePath);
		}
	}

	void UnitStatusIcons::addStatus(ability::Status* p_status)
	{
		if (!m_hasStarted)
		{
			return;
		}
		if (p_status->getStatusType() == ability::Status::StatusType::Stat_Buff)
		{
			if (m_numBuffs == 0)
			{
				m_iconTexturePaths.push_back(BUFF_ICON_TEXTURE);
			}
			++m_numBuffs;
		}
		else if (p_status->getStatusType() == ability::Status::StatusType::Stat_Debuff)
		{
			if (m_numDebuffs == 0)
			{
				m_iconTexturePaths.push_back(DEBUFF_ICON_TEXTURE);
			}
			++m_numDebuffs;
		}
		else if (p_status->getStatusType() == ability::Status::StatusType::Shield)
		{
			if (m_numShields == 0)
			{
				m_iconTexturePaths.push_back(SHIELD_ICON_TEXTURE);
			}
			++m_numShields;
		}

		// If Status LV < 1, then it should be a non-level up status that we want to display
		// in the Status Context
		if (p_status->getLV() < 1)
		{
			m_statusList.insert(p_status);
			kitten::Event* eventData = new kitten::Event(kitten::Event::Update_Card_Context_Status);
			eventData->putGameObj(UNIT_GO_KEY, &m_unit->getGameObject());
			kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Update_Card_Context_Status, eventData);
		}
		checkActiveStatuses();
	}

	void UnitStatusIcons::removeStatus(ability::Status* p_status)
	{
		if (!m_hasStarted)
		{
			return;
		}
		ability::Status::StatusType type = p_status->getStatusType();
		if (type == ability::Status::StatusType::Stat_Buff && m_numBuffs > 0)
		{
			--m_numBuffs;
		} 
		else if (type == ability::Status::StatusType::Stat_Debuff && m_numDebuffs > 0)
		{
			--m_numDebuffs;
		} 
		else if (type == ability::Status::StatusType::Shield && m_numShields > 0)
		{
			--m_numShields;
		}

		m_statusList.erase(p_status);
		kitten::Event* eventData = new kitten::Event(kitten::Event::Update_Card_Context_Status);
		eventData->putGameObj(UNIT_GO_KEY, &m_unit->getGameObject());
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Update_Card_Context_Status, eventData);
		checkActiveStatuses();
	}

	void UnitStatusIcons::checkActiveStatuses()
	{
		if (m_numBuffs == 0)
		{
			m_iconTexturePaths.remove(BUFF_ICON_TEXTURE);
		}

		if (m_numDebuffs == 0)
		{
			m_iconTexturePaths.remove(DEBUFF_ICON_TEXTURE);
		}

		if (m_numShields == 0)
		{
			m_iconTexturePaths.remove(SHIELD_ICON_TEXTURE);
		}

		// Add to update if there is more than one icon to cycle through
		if (m_iconTexturePaths.size() > 1)
		{
			setEnabled(true);
		}
		else if (m_iconTexturePaths.size() == 1)
		{
			// If only one texture, remove from update
			setEnabled(false);

			// But call onEnabled() to keep showing the single status icon
			onEnabled();
		}
		else
		{
			// If we're already disabled, call onDisabled() to ensure the icon is disabled
			if (!isEnabled())
			{
				onDisabled();
			}
			else
			{
				setEnabled(false);
			}
		}
	}
}