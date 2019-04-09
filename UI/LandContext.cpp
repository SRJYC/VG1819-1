#include "UI\LandContext.h"
#include "kitten\K_GameObjectManager.h"

#define CONTEXT_LERP_TIME 0.2f

LandContext::LandContext()
{

}

LandContext::~LandContext()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Update_Land_Context, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Land_Context_Set_Enabled, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Add_LandContext_Lerp_Callback, this);
}

void LandContext::start()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Update_Land_Context,
		this,
		std::bind(&LandContext::setTileInfoListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Land_Context_Set_Enabled,
		this,
		std::bind(&LandContext::setEnabledListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Add_LandContext_Lerp_Callback,
		this,
		std::bind(&LandContext::addLerpCallback, this, std::placeholders::_1, std::placeholders::_2));

	kitten::K_GameObjectManager* gom = kitten::K_GameObjectManager::getInstance();
	
	m_name = gom->createNewGameObject("UI/land_context/land_context_name_textbox.json")->getComponent<puppy::TextBox>();
	m_name->getTransform().setIgnoreParent(false);
	m_name->getTransform().setParent(&m_attachedObject->getTransform());
	m_name->setText("");

	m_description = gom->createNewGameObject("UI/land_context/land_context_textbox.json")->getComponent<puppy::TextBox>();
	m_description->getTransform().setIgnoreParent(false);
	m_description->getTransform().setParent(&m_attachedObject->getTransform());
	m_description->setText("");

	m_button = gom->createNewGameObject("UI/land_context/land_context_button.json");

	m_background = gom->createNewGameObject("UI/land_context/land_context_texture.json");
	m_background->getTransform().setIgnoreParent(false);
	m_background->getTransform().setParent(&m_attachedObject->getTransform());

	m_landPortrait = gom->createNewGameObject("UI/land_context/land_context_land_portrait.json")->getComponent<userinterface::UIObject>();
	glm::vec2 portraitScale = m_landPortrait->getTransform().getScale2D();
	m_landPortrait->getTransform().scale2D(portraitScale.x / getTransform().getScale2D().x, portraitScale.y / getTransform().getScale2D().y);
	m_landPortrait->getTransform().setIgnoreParent(false);
	m_landPortrait->getTransform().setParent(&m_attachedObject->getTransform());

	m_lerpController = m_attachedObject->getComponent<LerpController>();
	m_lerpController->addPositionLerpFinishedCallback(this);

	glm::vec3 pos = getTransform().getTranslation();
	m_enabledPos = glm::vec3(pos.x, pos.y, pos.z);
	m_disabledPos = glm::vec3(-getTransform().getScale2D().x, pos.y, pos.z);

	m_attachedObject->getTransform().place(m_disabledPos.x, m_disabledPos.y, m_disabledPos.z);
	m_attachedObject->setEnabled(false);
}

void LandContext::setTileInfo(TileInfo* p_tileInfo)
{
	if (m_tileInfo == nullptr)
	{
		m_tileInfo = p_tileInfo;
		if (isEnabled())
		{
			updateContext();
		}
	}
	else if (p_tileInfo->getType() != m_tileInfo->getType())
	{
		m_tileInfo = p_tileInfo;
		if (isEnabled())
		{
			updateContext();
		}
	}
}

void LandContext::setTileInfoListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	kitten::K_GameObject* tileGO = p_event->getGameObj(UPDATE_LAND_CONTEXT_KEY);
	if (tileGO != nullptr)
	{
		TileInfo* tileInfo = tileGO->getComponent<TileInfo>();
		setTileInfo(tileInfo);
	}
}

void LandContext::updateContext()
{
	std::string portraitPath = LandInfoManager::getInstance()->getLand(m_tileInfo->getType())->getTexturePath();
	m_landPortrait->setTexture(portraitPath.c_str());
	m_description->setText(m_tileInfo->getDescription());

	std::string name;
	switch (m_tileInfo->getType())
	{
		case LandInformation::Grass_land:
			name = "Grass Land";
			break;
		case LandInformation::Swamp_land:
			name = "Swamp Land";
			break;
		case LandInformation::Sand_land:
			name = "Sand Land";
			break;
		case LandInformation::Forest_land:
			name = "Mud Land";
			break;
		case LandInformation::Garden_land:
			name = "Garden Land";
			break;
		case LandInformation::Water_land:
			name = "Water Land";
			break;
		case LandInformation::Home_land:
			name = "Home Land";
			break;
		default:
			name = "Land";
			break;
	}
	m_name->setText(name);
}

void LandContext::setEnabledListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	if (m_attachedObject->isEnabled())
	{
		m_lerpController->positionLerp(glm::vec3(m_disabledPos.x, m_disabledPos.y, m_disabledPos.z), CONTEXT_LERP_TIME);
		m_enabling = false;
	}
	else
	{
		m_attachedObject->setEnabled(true);
		m_enabling = true;
		updateContext();
		m_lerpController->positionLerp(glm::vec3(m_enabledPos.x, m_enabledPos.y, m_enabledPos.z), CONTEXT_LERP_TIME);
	}
	
}

void LandContext::onPositionLerpFinished(kitten::K_GameObject* p_obj)
{
	if (!m_enabling)
	{
		m_attachedObject->setEnabled(false);
	}
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Add_LandContext_Lerp_Callback, nullptr);
}

void LandContext::addLerpCallback(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	m_lerpController->addPositionLerpFinishedCallback(this);
}