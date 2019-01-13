#include "CounterGetterController.h"
#include "kitten/Transform.h"
#include "CounterGetterButton.h"
#include "kitten/K_GameObjectManager.h"
#include "CounterGetter.h"

CounterGetterController::CounterGetterController()
	:m_counterGO(nullptr),
	m_currentGO(nullptr),
	m_maxGO(nullptr),
	m_minGO(nullptr),
	m_sliderButtonGO(nullptr),
	m_getter(nullptr)
{
}

CounterGetterController::~CounterGetterController()
{
}

void CounterGetterController::setGetter(CounterGetter * p_g)
{
	m_getter = p_g;
}

void CounterGetterController::start()
{
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();
	//create text box
	m_counterGO = gm->createNewGameObject("counter_getter/name_text.json");
	m_minGO = gm->createNewGameObject("counter_getter/min_text.json");
	m_maxGO = gm->createNewGameObject("counter_getter/max_text.json");
	m_currentGO = gm->createNewGameObject("counter_getter/current_text.json");

	//create slider button
	m_sliderButtonGO = gm->createNewGameObject("counter_getter/slider_button.json");

	//create buttons
	m_ButtonGO.push_back(gm->createNewGameObject("counter_getter/cancel_button.json"));
	m_ButtonGO.push_back(gm->createNewGameObject("counter_getter/check_button.json"));
	m_ButtonGO.push_back(gm->createNewGameObject("counter_getter/plus_button.json"));
	m_ButtonGO.push_back(gm->createNewGameObject("counter_getter/minus_button.json"));

	//set
	addThisToButton();

	m_maxGO->getComponent<puppy::TextBox>()->setText(std::to_string(m_max));
	m_minGO->getComponent<puppy::TextBox>()->setText(std::to_string(m_min));
	m_counterGO->getComponent<puppy::TextBox>()->setText(m_counter);

	setCurrentNumber();
}

void CounterGetterController::set(const std::string & p_name, int p_min, int p_max)
{
	m_counter = p_name;
	m_min = p_min;
	m_max = p_max;
	m_current = m_min;

	if (m_maxGO != nullptr)
	{
		m_maxGO->getComponent<puppy::TextBox>()->setText(std::to_string(m_max));
		m_minGO->getComponent<puppy::TextBox>()->setText(std::to_string(m_min));
		m_counterGO->getComponent<puppy::TextBox>()->setText(m_counter);

		setCurrentNumber();
	}
}

void CounterGetterController::increase()
{
	m_current++;
	if (m_current > m_max)
		m_current = m_min;
	setCurrentNumber();
}

void CounterGetterController::decrease()
{
	m_current--;
	if (m_current < m_min)
		m_current = m_max;
	setCurrentNumber();
}

void CounterGetterController::check()
{
	assert(m_getter != nullptr);
	m_getter->getPlayerChoice(m_current);
}

void CounterGetterController::cancel()
{
	assert(m_getter != nullptr);
	m_getter->cancel();
}

void CounterGetterController::onEnabled()
{
	m_counterGO->setEnabled(true);
	m_minGO->setEnabled(true);
	m_maxGO->setEnabled(true);
	m_currentGO->setEnabled(true);
	m_sliderButtonGO->setEnabled(true);
	for (int i = 0; i < m_ButtonGO.size(); i++)
	{
		m_ButtonGO[i]->setEnabled(true);
	}
}

void CounterGetterController::onDisabled()
{
	m_max = -1;
	m_min = -1;
	m_current = -1;
	m_counter = "Missing";

	m_counterGO->setEnabled(false);
	m_minGO->setEnabled(false);
	m_maxGO->setEnabled(false);
	m_currentGO->setEnabled(false);
	m_sliderButtonGO->setEnabled(false);
	for (int i = 0; i < m_ButtonGO.size(); i++)
	{
		m_ButtonGO[i]->setEnabled(false);
	}
}

void CounterGetterController::addThisToButton()
{
	for (int i = 0; i < m_ButtonGO.size(); i++)
	{
		m_ButtonGO[i]->getComponent<CounterGetterButton>()->setController(this);
	}
}

void CounterGetterController::setCurrentNumber()
{
	assert(m_currentGO != nullptr);
	assert(m_sliderButtonGO != nullptr);

	m_currentGO->getComponent<puppy::TextBox>()->setText(std::to_string(m_current));

	int range = m_max - m_min;
	float percent = (float)(m_current - m_min) / (float)range;
	int x = percent * SLIDER_RANGE + SLIDER_START;
	int y = m_sliderButtonGO->getTransform().getTranslation().y;
	m_sliderButtonGO->getTransform().place2D(x,y);
}
