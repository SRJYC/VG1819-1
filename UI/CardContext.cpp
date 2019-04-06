#include "UI\CardContext.h"
#include "kitten\K_GameObjectManager.h"
#include "UI\UIObject.h"
#include "unit\unitComponent\UnitStatusIcons.h"

// Only for testing, using a static unit
#include "kibble\databank\databank.hpp"
#include "kitten\InputManager.h"

#define ABILITIES_X 14.0f
#define ABILITIES_Y 138.0f

#define LINE_HEIGHT 12.0f
#define LINE_MAX_CHAR_LENGTH 36.0f

#define PORTRAIT_X 52.0f
#define PORTRAIT_Y 180.0f

#define DEFAULT_MAX_NAME_LENGTH 16
#define NAME_FONT_DEFAULT "../fonts/nsimsun_18pt.fnt"
#define NAME_FONT_SMALL "../fonts/nsimsun_14pt.fnt"

#define P0_COLOR_R 255.0f / 255.0f
#define P0_COLOR_G 216.0f / 255.0f
#define P0_COLOR_B 0.0f / 255.0f
#define P1_COLOR_R 0.0f / 255.0f
#define P1_COLOR_G 230.0f / 255.0f
#define P1_COLOR_B 255.0f / 255.0f

CardContext::CardContext(char p_statusContextKey) : m_statusContextKey(p_statusContextKey)
{

}

CardContext::~CardContext()
{
	removeListeners();
}

void CardContext::start()
{
	registerListeners();

	kitten::K_GameObjectManager* gom = kitten::K_GameObjectManager::getInstance();
	// Cannot set these as children as they won't display
	m_cardTexture = gom->createNewGameObject("UI/card_context/card_context_texture.json");
	m_cardTexture->getTransform().setIgnoreParent(false);
	m_cardTexture->getTransform().setParent(&m_attachedObject->getTransform());
	m_unitPortrait = gom->createNewGameObject("UI/card_context/card_context_portrait.json")->getComponent<userinterface::UIObject>();
	m_unitPortrait->getTransform().setIgnoreParent(false);
	m_unitPortrait->getTransform().setParent(&m_attachedObject->getTransform());

	m_nameBox = gom->createNewGameObject("UI/card_context/card_context_name_textbox.json")->getComponent<puppy::TextBox>();
	m_nameBox->getGameObject().getTransform().setIgnoreParent(false);
	m_nameBox->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	m_tagsBox = gom->createNewGameObject("UI/card_context/card_context_tags_textbox.json")->getComponent<puppy::TextBox>();
	m_tagsBox->getGameObject().getTransform().setIgnoreParent(false);
	m_tagsBox->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	m_hpBox = gom->createNewGameObject("UI/card_context/card_context_hp_textbox.json")->getComponent<puppy::TextBox>();
	m_hpBox->getGameObject().getTransform().setIgnoreParent(false);
	m_hpBox->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	m_mvBox = gom->createNewGameObject("UI/card_context/card_context_mv_textbox.json")->getComponent<puppy::TextBox>();
	m_mvBox->getGameObject().getTransform().setIgnoreParent(false);
	m_mvBox->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	m_initiativeBox = gom->createNewGameObject("UI/card_context/card_context_in_textbox.json")->getComponent<puppy::TextBox>();
	m_initiativeBox->getGameObject().getTransform().setIgnoreParent(false);
	m_initiativeBox->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	m_costBox = gom->createNewGameObject("UI/card_context/card_context_cost_textbox.json")->getComponent<puppy::TextBox>();
	m_costBox->getGameObject().getTransform().setIgnoreParent(false);
	m_costBox->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	for (int i = 0; i < 3; ++i)
	{
		// Creating Ability Attribute TextBoxes
		puppy::TextBox* abilityText = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UI/card_context/card_context_ability_attribute_textbox.json")->getComponent<puppy::TextBox>();
		abilityText->getTransform().setIgnoreParent(false);
		abilityText->getTransform().setParent(&m_attachedObject->getTransform());
		m_abilityAttributes[i] = abilityText;

		// Creating Ability Description TextBoxes
		abilityText = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UI/card_context/card_context_ability_description_textbox.json")->getComponent<puppy::TextBox>();
		abilityText->getTransform().setIgnoreParent(false);
		abilityText->getTransform().setParent(&m_attachedObject->getTransform());
		m_abilityDescriptions[i] = abilityText;
	}

	m_statusList = gom->createNewGameObject("UI/card_context/card_context_status_textbox.json")->getComponent<puppy::TextBox>();
	m_statusList->getGameObject().getTransform().setIgnoreParent(false);
	m_statusList->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	// Saving the fonts used for the Name TextBox as they may change
	m_defaultNameFont = puppy::FontTable::getInstance()->getFont(NAME_FONT_DEFAULT);
	m_smallNameFont = puppy::FontTable::getInstance()->getFont(NAME_FONT_SMALL);
	m_currentNameFont = m_defaultNameFont;

	m_statusContext = gom->createNewGameObject("UI/status_context/status_context.json")->getComponent<StatusContext>();
	m_statusContext->getGameObject().getTransform().setIgnoreParent(false);
	m_statusContext->getGameObject().getTransform().setParent(&m_attachedObject->getTransform());

	// Testing
	setUnit(kibble::getUnitFromId(13));

	m_attachedObject->setEnabled(false);
}

void CardContext::onEnabled()
{
	m_attachedObject->setEnabled(true);
	m_cardTexture->setEnabled(true);
	m_unitPortrait->setEnabled(true);
	if (m_nonLevelStatus > 0)
	{
		m_statusContext->getGameObject().setEnabled(true);
	}
}

void CardContext::onDisabled()
{
	m_attachedObject->setEnabled(false);
	m_cardTexture->setEnabled(false);
	m_unitPortrait->setEnabled(false);
	if (m_nonLevelStatus > 0)
	{
		m_statusContext->getGameObject().setEnabled(false);
	}
}

// For testing only, changes the unit on the hovered card by pressing the B key
void CardContext::update()
{
	input::InputManager* inputMan = input::InputManager::getInstance();
	// Test feature to cycle through units
	if (inputMan->keyDown('B') && !inputMan->keyDownLast('B'))
	{
		setUnit(kibble::getUnitFromId(m_unitId));

		m_unitId = (m_unitId + 1) % 15;
		if (m_unitId == 0 || m_unitId == 6)
			m_unitId++;
	}

	if (inputMan->keyDown(m_statusContextKey) && !inputMan->keyDownLast(m_statusContextKey) && m_nonLevelStatus > 0)
	{
		m_statusContext->lerpContext();
	}
}

void CardContext::setUnit(unit::Unit* p_unit)
{
	if (p_unit != m_unitData && p_unit != nullptr)
	{
		m_unitData = p_unit;
		updateUnitData();
	}
}

void CardContext::updateUnitData()
{
	m_unitPortrait->setTexture(m_unitData->getPortraitTexturePath().c_str());

	std::string name = m_unitData->m_name;
	if (m_unitData->m_attributes[UNIT_LV] > 0)
	{
		name += "(" + std::to_string(m_unitData->m_attributes[UNIT_LV]) + ")";
	}
	if (name.length() > DEFAULT_MAX_NAME_LENGTH && m_currentNameFont != m_smallNameFont)
	{
		m_nameBox->setFont(m_smallNameFont);
		m_nameBox->getTransform().move2D(0.0f, -2.0f);
		m_currentNameFont = m_smallNameFont;
	}
	else if (m_currentNameFont != m_defaultNameFont)
	{
		m_nameBox->setFont(m_defaultNameFont);
		m_nameBox->getTransform().move2D(0.0f, 2.0f);
		m_currentNameFont = m_defaultNameFont;
	}
	m_nameBox->setText(name);
	switch (m_unitData->m_clientId)
	{
		case 0:
			m_nameBox->setColor(P0_COLOR_R, P0_COLOR_G, P0_COLOR_B);
			break;
		case 1:
			m_nameBox->setColor(P1_COLOR_R, P1_COLOR_G, P1_COLOR_B);
			break;
		default:
			m_nameBox->setColor(1.0f, 1.0f, 1.0f);
			break;
	}

	std::string tags = "";
	auto it = m_unitData->m_tags.begin();
	for (; it != m_unitData->m_tags.end(); ++it)
	{
		tags += *it + ", ";
	}
	m_tagsBox->setText(tags.substr(0, tags.length() - 2));
	updateUnitAttributes();
	//return;
	// Loop through the ability list
	// Trying to use a max of 3 lines for each ability + description,
	// otherwise we won't be able to fit all the details on the card

	m_textBoxesIndex = 0;
	for (auto it : m_unitData->m_ADList)
	{
		if (m_textBoxesIndex < 3) // Hard Limit of displaying only 3 abilities
		{
			std::string abilityDesc = "";
			unit::AbilityDescription* ad = it;

			int lv = ad->m_intValue[UNIT_LV];
			if (lv > 0)
				abilityDesc += "LV:" + std::to_string(lv) + " ";

			std::string abilityName = ad->m_stringValue[ABILITY_NAME];
			abilityDesc += abilityName + ":";

			int power = ad->m_intValue[UNIT_POWER];
			if (power > 0)
				abilityDesc += " POW:" + std::to_string(power);

			int cd = ad->m_intValue[UNIT_CD];
			if (cd > 0)
				abilityDesc += " CD:" + std::to_string(cd);

			int ct = ad->m_intValue[UNIT_CT];
			if (ct > 0)
				abilityDesc += " CT:" + std::to_string(ct);

			if (ad->m_intValue.find(MIN_RANGE) != ad->m_intValue.end())
			{
				int minRange = ad->m_intValue[MIN_RANGE];
				int maxRange = ad->m_intValue[MAX_RANGE];
				if (minRange == maxRange && minRange != 0)
					abilityDesc += " RNG:" + std::to_string(minRange);
				else if (minRange < maxRange)
					abilityDesc += " RNG:" + std::to_string(minRange) + "-" + std::to_string(maxRange);
			}

			m_abilityAttributes[m_textBoxesIndex]->setText(abilityDesc);

			// Creating a new TextBox for the description
			std::string description = ad->m_stringValue[UNIT_ABILITY_DESCRIPTION];
			if (description.length() > 0)
			{
				m_abilityDescriptions[m_textBoxesIndex]->setText(description);
			}
			++m_textBoxesIndex;
		}
	}

	// Clear any unused textboxes
	for (int leftovers = m_textBoxesIndex; leftovers < 3; ++leftovers)
	{
		m_abilityAttributes[leftovers]->setText("");
		m_abilityDescriptions[leftovers]->setText("");
	}

	updateUnitStatus();
	

	arrangeTextBoxes();
}

void CardContext::updateUnitAttributes()
{
	if (m_unitData->m_attributes[UNIT_HP] < 0)
	{
		setEnabled(false);
	}
	m_hpBox->setText(std::to_string(m_unitData->m_attributes[UNIT_HP]));
	m_mvBox->setText(std::to_string(m_unitData->m_attributes[UNIT_MV]));
	m_initiativeBox->setText(std::to_string(m_unitData->m_attributes[UNIT_IN]));
	m_costBox->setText(std::to_string(m_unitData->m_attributes[UNIT_COST]));

	setAttribTextColor(m_hpBox, UNIT_HP, UNIT_MAX_HP);
	setAttribTextColor(m_mvBox, UNIT_MV, UNIT_BASE_MV);
	setAttribTextColor(m_initiativeBox, UNIT_IN, UNIT_BASE_IN);
	setAttribTextColor(m_costBox, UNIT_COST, UNIT_BASE_COST);
}

void CardContext::updateUnitStatus()
{
	// Status Info TextBoxes
	// Clear the previous text then set the new statuses
	m_statusList->setText("");
	unit::StatusContainer* statusContainer = m_unitData->getStatusContainer();
	std::string statusDesc = "";

	m_nonLevelStatus = 0;
	for (auto it : statusContainer->m_statusList)
	{
		if (it != statusContainer->m_statusList.at(0))
		{
			statusDesc += ", ";
		}

		std::unordered_map<std::string, int> attributes = it->getAttributeChanges();

		int lv = it->getLV();
		if (lv > 0)
			statusDesc += "LV:" + std::to_string(lv);
		else
		{
			++m_nonLevelStatus;
			continue;
		}

		int hp = attributes[UNIT_HP];
		if (hp > 0)
			statusDesc += " HP+" + std::to_string(hp);

		int in = attributes[UNIT_IN];
		if (in > 0)
			statusDesc += " IN+" + std::to_string(in);

		int mv = attributes[UNIT_MV];
		if (mv > 0)
			statusDesc += " MV+" + std::to_string(mv);


		std::string description = it->getDescription();
		if (description.length() > 0)
		{
			statusDesc += " " + description;
		}

		m_statusList->setText(statusDesc);
	}

	if (m_updatedByGO && m_nonLevelStatus > 0)
	{
		unit::UnitStatusIcons* unitStatusIcons = m_unitData->getGameObject().getComponent<unit::UnitStatusIcons>();
		if (unitStatusIcons != nullptr)
		{
			m_statusContext->updateContext(unitStatusIcons->m_statusList);
		}
	}
}

void CardContext::setAttribTextColor(puppy::TextBox* p_textBox, const std::string& p_currAttrib, const std::string& p_baseAttrib)
{
	if (m_unitData->m_attributes[p_currAttrib] < m_unitData->m_attributes[p_baseAttrib])
	{
		p_textBox->setColor(1.0f, 0.0f, 0.0f);
	} 
	else if (m_unitData->m_attributes[p_currAttrib] > m_unitData->m_attributes[p_baseAttrib])
	{
		p_textBox->setColor(0.0f, 0.5f, 1.0f);
	} 
	else
	{
		p_textBox->setColor(1.0f, 1.0f, 1.0f);
	}
}

void CardContext::arrangeTextBoxes()
{
	int row = 0, lastRow = row;
	float padding = 0.0f;
	glm::vec3 contextPos = getTransform().getTranslation();

	// Abilities
	for (int i = 0; i < m_textBoxesIndex; ++i)
	{
		m_abilityAttributes[i]->getTransform().place(ABILITIES_X, ABILITIES_Y - (row * LINE_HEIGHT) - padding, 0.01f);
		row += std::ceil(m_abilityAttributes[i]->getText().length() / LINE_MAX_CHAR_LENGTH); // Determines how many rows have been used by the text that's been set

		m_abilityDescriptions[i]->getTransform().place(ABILITIES_X, ABILITIES_Y - (row * LINE_HEIGHT) - padding, 0.01f);
		row += std::ceil(m_abilityDescriptions[i]->getText().length() / LINE_MAX_CHAR_LENGTH);

		padding += (row - lastRow> 0 ? 4.0f : 0.0f);
		lastRow = row;
	}

	// Status
	m_statusList->getTransform().place(ABILITIES_X, ABILITIES_Y - (row * LINE_HEIGHT) - padding, 0.01f);
}

void CardContext::setEnabledListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	m_attachedObject->setEnabled(p_event->getInt(CARD_CONTEXT_SET_ENABLED_KEY));
}

void CardContext::setUnitListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	if (p_type == kitten::Event::Update_Card_Context_By_GO)
	{
		kitten::K_GameObject* unitGO = p_event->getGameObj(UPDATE_CARD_CONTEXT_KEY);
		if (unitGO != nullptr)
		{
			m_updatedByGO = true;
			unit::Unit* unit = unitGO->getComponent<unit::Unit>();
			setUnit(unit);			
		}
	} else if (p_type == kitten::Event::Update_Card_Context_By_ID)
	{
		m_updatedByGO = false;
		setUnit(kibble::getUnitFromId(p_event->getInt(UPDATE_CARD_CONTEXT_KEY)));
	}
}

void CardContext::updateUnitAttributesListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	if (p_event->getGameObj(UNIT_GO_KEY)->getComponent<unit::Unit>() == m_unitData)
	{
		updateUnitData();
	}
}

void CardContext::updateUnitStatusListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	if (p_event->getGameObj(UNIT_GO_KEY)->getComponent<unit::Unit>() == m_unitData)
	{
		updateUnitStatus();
	}
}

void CardContext::registerListeners()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Update_Card_Context_By_ID,
		this,
		std::bind(&CardContext::setUnitListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Update_Card_Context_By_GO,
		this,
		std::bind(&CardContext::setUnitListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Card_Context_Set_Enabled,
		this,
		std::bind(&CardContext::setEnabledListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Update_Card_Context_Attrib,
		this,
		std::bind(&CardContext::updateUnitAttributesListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Update_Card_Context_Status,
		this,
		std::bind(&CardContext::updateUnitStatusListener, this, std::placeholders::_1, std::placeholders::_2));
}

void CardContext::removeListeners()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Update_Card_Context_By_ID, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Update_Card_Context_By_GO, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Card_Context_Set_Enabled, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Update_Card_Context_Attrib, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Update_Card_Context_Status, this);
}
