#include "CounterGetterButton.h"
#include "CounterGetterController.h"

CounterGetterButton::CounterGetterButton(ButtonBehavior p_behavior)
	:m_behavior(p_behavior),
	m_countroller(nullptr)
{
}

CounterGetterButton::~CounterGetterButton()
{
}

void CounterGetterButton::onClick()
{
	assert(m_countroller != nullptr);

	switch (m_behavior)
	{
	case ButtonBehavior::plus:
		m_countroller->increase();
		break;
	case ButtonBehavior::minus:
		m_countroller->decrease();
		break;
	case ButtonBehavior::check:
		m_countroller->check();
		break;
	case ButtonBehavior::cancel:
		m_countroller->cancel();
		break;
	}
}

void CounterGetterButton::setController(CounterGetterController * p_controller)
{
	m_countroller = p_controller;
}
