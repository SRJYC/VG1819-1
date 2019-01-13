#include "components\PowerTracker.h"
#include "kitten\K_ComponentManager.h"
#include "kitten\K_GameObjectManager.h"
#include "puppy\Text\FontTable.h"

PowerTracker::PowerTracker()
	:
	m_iMaxPower(4),
	m_iCurrentPower(4)
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Manipulate_Tile,
		this,
		std::bind(&PowerTracker::increaseMaxPowerEvent, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Reset_Power,
		this,
		std::bind(&PowerTracker::resetEvent, this, std::placeholders::_1, std::placeholders::_2));

}

PowerTracker::~PowerTracker()
{
	//kitten::K_ComponentManager::getInstance()->destroyComponentImmediate(m_textBox);
}

void PowerTracker::start()
{
	kitten::K_GameObject* textBox = kitten::K_GameObjectManager::getInstance()->createNewGameObject();

	m_textBox = static_cast<puppy::TextBox*>(kitten::K_ComponentManager::getInstance()->createComponent("TextBox"));
	textBox->addComponent(m_textBox);
	textBox->getTransform().place2D(410, 130);

	m_textBox->setFont(puppy::FontTable::getInstance()->getFont("../fonts/nsimsun_34pt.fnt"));
	m_textBox->setColor(1.0, 1.0, 1.0);
	m_textBox->setBoxBounds(1000, 400);
}

void PowerTracker::update()
{
	m_textBox->setText(std::to_string(getCurrentPower()) + " / " + std::to_string(m_iMaxPower));
}

void PowerTracker::increaseMaxPower(int p_iAmount)
{
	m_iMaxPower += p_iAmount;
}

void PowerTracker::increaseMaxPowerEvent(kitten::Event::EventType p_type, kitten::Event* p_data)
{
	int power = p_data->getInt(MANIPULATE_TILE_KEY);
	m_iMaxPower += power;
	m_iCurrentPower += power;
	//resetCurrent();
}

bool PowerTracker::summonUnitCost(int p_iCost)
{
	if ((m_iCurrentPower - p_iCost) >= 0)
	{
		m_iCurrentPower -= p_iCost;
		return true;
	}

	return false;
}

void PowerTracker::resetCurrent()
{
	m_iCurrentPower = m_iMaxPower;
}

void PowerTracker::resetEvent(kitten::Event::EventType p_type, kitten::Event * p_data)
{
	resetCurrent();
}

int PowerTracker::getMaxPower()
{
	return m_iMaxPower;
}

int PowerTracker::getCurrentPower()
{
	return m_iCurrentPower;
}


bool PowerTracker::changeCurrentPower(int p_iAmount)
{
	if ((m_iCurrentPower + p_iAmount) >= 0)
	{
		m_iCurrentPower += p_iAmount;
		return true;
	}

	return false;
}