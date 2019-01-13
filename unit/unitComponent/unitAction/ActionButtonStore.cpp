#include "ActionButtonStore.h"
#include "kitten/K_GameObjectManager.h"
#include "kitten/InputManager.h"
#include "UI/ClickableButton.h"

unit::ActionButtonStore::ActionButtonStore()
{
	m_show = false;
	m_buttonScaleX = 0;
	m_buttonScaleY = 0;
}

unit::ActionButtonStore::~ActionButtonStore()
{
}

void unit::ActionButtonStore::display(Unit * p_u)
{
	//set unit
	m_unit = p_u;

	//get start position
	input::InputManager* im = input::InputManager::getInstance();
	m_lastX = im->getMouseXPos();
	m_lastY = im->getMouseYOpenGLPos();
	//get window size
	int winX = im->getWindowWidth();
	int winY = im->getWindowHeight();
	int centerX = winX / 2;
	int centerY = winY / 2;
	//get button scale
	if (m_buttonScaleX <= 0 || m_buttonScaleY <= 0)
	{
		getButtonScale();
	}
	assert(m_buttonScaleX > 0 && m_buttonScaleY > 0);
	//change x position
	if (m_lastX > centerX)
		m_lastX = m_lastX - m_buttonScaleX;


	//start of list
	m_index = 0;

	//move
	if (m_unit->m_attributes["base_mv"] > 0)
		setButton("Move", m_unit->canMove());

	//normal ability
	setAbility();

	//join
	//commander action
	if (p_u->isCommander())
	{
		setButton("ManipulateTile", m_unit->canAct());
		setButton("Summon", true);
	}
	else
	{
		bool canJoin = true;
		for (std::string it : m_unit->m_tags)//strucutre can't join to another unit
		{
			if (it == STRUCTURE)
			{
				canJoin = false;
				break;
			}
		}
		if (canJoin)
		{
			setButton("Join", m_unit->canAct());

			//for test
			setButton("For test: Level Up", true);
		}
	}
	
	setButton("Turn End", true);

	setButton("For test: Destroy", true);

	m_show = true;

	//change y position
	if (im->getMouseYOpenGLPos() < centerY)
	{
		int delta = m_index * m_buttonScaleY;
		for (int i = 0; i < m_index; i++)
		{
			m_buttonList[i]->getTransform().move2D(0,delta);
		}
	}
}

void unit::ActionButtonStore::hide()
{
	if (m_show)
	{
		for (int i = 0; i < m_buttonList.size(); i++)
		{
			m_buttonList[i]->setEnabled(false);
		}
		m_show = false;
	}
	
}

void unit::ActionButtonStore::createNewButton()
{
	kitten::K_GameObject* ab = kitten::K_GameObjectManager::getInstance()->createNewGameObject("unit_action_button.json");
	ab->getComponent<ActionSelect>()->setStorage(this);
	//ab->setEnabled(false);
	m_buttonList.push_back(ab);
}

void unit::ActionButtonStore::setButton(const std::string & p_msg, bool p_a, int p_cd)
{
	kitten::K_GameObject* go;
	if (m_index >= m_buttonList.size())//create new button if not enough
		createNewButton();
	go = m_buttonList[m_index];

	m_lastY = m_lastY - go->getTransform().getScale2D().y;

	go->getTransform().place2D(m_lastX, m_lastY);

	ActionSelect* a = go->getComponent<ActionSelect>();
	a->setAction(p_msg, p_cd);
	a->setUnit(m_unit);
	a->setActive(p_a);

	if (p_cd > 0 || !p_a)
	{
		userinterface::ClickableButton* cb = go->getComponent<userinterface::ClickableButton>();
		cb->setActive(false);
	}

	go->setEnabled(true);

	m_index++;
}

void unit::ActionButtonStore::getButtonScale()
{
	if (m_buttonList.size() == 0)//no button exist
	{
		createNewButton();
	}

	kitten::K_GameObject* ab = m_buttonList[0];
	auto scale = ab->getTransform().getScale2D();

	m_buttonScaleX = scale.x;
	m_buttonScaleY = scale.y;
}

void unit::ActionButtonStore::setAbility()
{
	int lv = m_unit->m_attributes[UNIT_LV];
	for (auto it : m_unit->m_ADList)
	{
		if (it.second->m_intValue[UNIT_LV] <= lv)//check level
		{
			if (!m_unit->canAct())//check can act
			{
				setButton(it.first, false);
			}
			else
			{
				int cd = m_unit->checkCD(it.first);//check cd
				if(cd > 0)
					setButton(it.first, false, cd);
				else
					setButton(it.first, true);

			}
		}
	}
}
