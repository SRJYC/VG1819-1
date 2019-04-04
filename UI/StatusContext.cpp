#include "UI\StatusContext.h"
#include "kitten\K_GameObjectManager.h"

#define CONTEXT_LERP_TIME 0.2f

StatusContext::StatusContext()
{

}

StatusContext::~StatusContext()
{

}

void StatusContext::start()
{
	kitten::K_GameObjectManager* gom = kitten::K_GameObjectManager::getInstance();

	m_background = gom->createNewGameObject("UI/status_context/status_context_texture.json");
	m_background->getTransform().setIgnoreParent(false);
	m_background->getTransform().setParent(&m_attachedObject->getTransform());

	m_buffsTextbox = gom->createNewGameObject("UI/status_context/status_context_textbox.json")->getComponent<puppy::TextBox>();
	m_buffsTextbox->getTransform().setIgnoreParent(false);
	m_buffsTextbox->getTransform().setParent(&m_attachedObject->getTransform());
	m_buffsTextbox->setText("");

	int textboxHeight = m_buffsTextbox->getBoxHeight();

	m_debuffsTextbox = gom->createNewGameObject("UI/status_context/status_context_textbox.json")->getComponent<puppy::TextBox>();
	m_debuffsTextbox->getTransform().setIgnoreParent(false);
	m_debuffsTextbox->getTransform().setParent(&m_attachedObject->getTransform());
	m_debuffsTextbox->getTransform().move2D(0.0f, -((float)textboxHeight + 28));
	m_debuffsTextbox->setText("");

	m_shieldsTextbox = gom->createNewGameObject("UI/status_context/status_context_textbox.json")->getComponent<puppy::TextBox>();
	m_shieldsTextbox->getTransform().setIgnoreParent(false);
	m_shieldsTextbox->getTransform().setParent(&m_attachedObject->getTransform());
	m_shieldsTextbox->getTransform().move2D(0.0f, -((float)textboxHeight + 28) * 2.0f);
	m_shieldsTextbox->setText("");	

	m_buffsIcon = gom->createNewGameObject("UI/status_context/status_context_buff_icon.json")->getComponent<userinterface::UIObject>();
	glm::vec2 portraitScale = m_buffsIcon->getTransform().getScale2D();
	m_buffsIcon->getTransform().scale2D(portraitScale.x / getTransform().getScale2D().x, portraitScale.y / getTransform().getScale2D().y);
	m_buffsIcon->getTransform().setIgnoreParent(false);
	m_buffsIcon->getTransform().setParent(&m_attachedObject->getTransform());

	m_debuffsIcon = gom->createNewGameObject("UI/status_context/status_context_debuff_icon.json")->getComponent<userinterface::UIObject>();
	m_debuffsIcon->getTransform().scale2D(portraitScale.x / getTransform().getScale2D().x, portraitScale.y / getTransform().getScale2D().y);
	m_debuffsIcon->getTransform().setIgnoreParent(false);
	m_debuffsIcon->getTransform().setParent(&m_attachedObject->getTransform());

	m_shieldsIcon = gom->createNewGameObject("UI/status_context/status_context_shield_icon.json")->getComponent<userinterface::UIObject>();
	m_shieldsIcon->getTransform().scale2D(portraitScale.x / getTransform().getScale2D().x, portraitScale.y / getTransform().getScale2D().y);
	m_shieldsIcon->getTransform().setIgnoreParent(false);
	m_shieldsIcon->getTransform().setParent(&m_attachedObject->getTransform());

	m_lerpController = m_attachedObject->getComponent<LerpController>();

	//glm::vec3 pos = m_attachedObject->getTransform().getTranslation();
	glm::vec3 pos = glm::vec3(978.0f, 390.0f, -0.3f);
	m_disabledPos = pos;
	m_enabledPos = glm::vec3(pos.x - 194.0f, pos.y, pos.z);

	getTransform().place(m_disabledPos.x, m_disabledPos.y, m_disabledPos.z);
	m_attachedObject->setEnabled(false);
}

void StatusContext::onEnabled()
{
	setText();
	m_lerpController->setEnabled(false);
}

void StatusContext::onDisabled()
{
	if (m_displayed)
	{
		getTransform().place(m_disabledPos.x, m_disabledPos.y, m_disabledPos.z);
		m_displayed = false;
	}
}

void StatusContext::updateContext(const std::unordered_set<ability::Status*>& p_statusList)
{
	m_buffs = "";
	m_debuffs = "";
	m_shields = "";
	for (ability::Status* status : p_statusList)
	{
		ability::Status::StatusType type = status->getStatusType();
		if (type == ability::Status::StatusType::Stat_Buff)
		{
			m_buffs += status->getName() + ": " + status->getDescription() + " ";
		}
		else if (type == ability::Status::StatusType::Stat_Debuff)
		{
			m_debuffs += status->getName() + ": " + status->getDescription() + " ";
		}
		else if (type == ability::Status::StatusType::Shield)
		{
			m_shields += status->getName() + ": " + status->getDescription() + " ";
		}
	}
	m_textboxesSet = false;

	if (isEnabled())
	{
		setText();
	}
}

void StatusContext::setText()
{
	if (!m_textboxesSet)
	{
		m_buffsTextbox->setText(m_buffs);
		m_debuffsTextbox->setText(m_debuffs);
		m_shieldsTextbox->setText(m_shields);
		m_textboxesSet = true;
	}
}

void StatusContext::lerpContext()
{
	if (!m_displayed)
	{
		m_lerpController->positionLerp(glm::vec3(m_enabledPos.x, m_enabledPos.y, m_enabledPos.z), CONTEXT_LERP_TIME);
		m_displayed = !m_displayed;
	}
	else
	{
		m_lerpController->positionLerp(glm::vec3(m_disabledPos.x, m_disabledPos.y, m_disabledPos.z), CONTEXT_LERP_TIME);
		m_displayed = !m_displayed;
	}
}