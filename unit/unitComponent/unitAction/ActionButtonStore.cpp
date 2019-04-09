#include "ActionButtonStore.h"
#include "kitten/K_GameObjectManager.h"
#include "kitten/InputManager.h"
#include "UI/ClickableButton.h"
#include "networking/ClientGame.h"
#include "ActionMap.h"
#include "AI/controller.h"

unit::ActionButtonStore::ActionButtonStore()
{
	m_show = false;
	m_actionShow = false;
	m_buttonScaleX = 0;
	m_buttonScaleY = 0;
	m_index = 0;
	m_actionStartIndex = 0;
	m_actionEndIndex = 0;

	m_map = new ActionMap();

	registerEvent();
}

unit::ActionButtonStore::~ActionButtonStore()
{
	delete m_map;

	deregisterEvent();
}

void unit::ActionButtonStore::display(Unit * p_u)
{
	//will not refresh when click unit after buttons are displayed
	if (m_show)
		return;

	//set unit
	m_unit = p_u;

	//get start position
	input::InputManager* im = input::InputManager::getInstance();
	m_lastX = im->getMouseXPos();
	m_lastY = im->getMouseYOpenGLPos();

	//get window size
	m_winX = im->getWindowWidth();
	m_winY = im->getWindowHeight();
	m_centerX = m_winX / 2;
	m_centerY = m_winY / 2;

	//get button scale
	if (m_buttonScaleX <= 0 || m_buttonScaleY <= 0)
	{
		getButtonScale();
	}
	assert(m_buttonScaleX > 0 && m_buttonScaleY > 0);

	//change x position
	m_xChange = false;
	if (m_lastX > m_centerX)
	{
		m_lastX = m_lastX - m_buttonScaleX;
		m_xChange = true;
	}


	//start of list
	m_index = 0;

	//move
	if (m_unit->m_attributes["base_mv"] > 0)
		setButton("Move", m_unit->canMove());

	//action
	setButton("Action", m_unit->canAct());

	/*
	//normal ability
	setAbility();

	//join
	//commander action
	if (p_u->isCommander())
	{
		setButton("ManipulateTile", m_unit->canAct());
		//setButton("Summon", true);
	}
	

	bool canJoin = !p_u->isCommander() && !p_u->isStructure();//not structure or commander
	if(canJoin && p_u->m_attributes[UNIT_LV] < 3)//not level 3
		setButton("Join", m_unit->canAct());
	*/

	setButton("Turn End", true);

	//for test
	if (!networking::ClientGame::isNetworkValid() && !(AI::controller::getAIControllerSize() == 1))
	{
		bool canJoin = !m_unit->isCommander() && !m_unit->checkTag(STRUCTURE);//not structure or commander
		canJoin = canJoin && m_unit->m_attributes[UNIT_LV] < 3;//not level 3
		if(canJoin)
			setButton("For test: Level Up", true);

		setButton("For test: Destroy", true);

		setButton("For test: Set DP", true);
	}

	m_show = true;

	//change y position
	if (im->getMouseYOpenGLPos() < m_centerY)
	{
		int delta = m_index * m_buttonScaleY;
		for (int i = 0; i < m_index; i++)
		{
			m_buttonList[i]->getTransform().move2D(0,delta);
		}
	}
}

void unit::ActionButtonStore::displayAction(kitten::K_GameObject * p_buttonGO)
{
	//already displayed
	if (m_actionShow)
	{
		for (int i = m_actionStartIndex; i < m_actionEndIndex; i++)
		{
			m_buttonList[i]->setEnabled(true);
		}
		return;
	}

	m_actionShow = true;

	//get button scale
	assert(m_buttonScaleX > 0 && m_buttonScaleY > 0);

	//get start position
	auto pos = p_buttonGO->getTransform().getTranslation();
	m_lastX = pos.x + m_buttonScaleX;
	m_lastY = pos.y + m_buttonScaleY;
	//change x position
	if (!m_xChange)//if the button will out of window
		m_lastX = m_lastX - m_buttonScaleX * 2;//move it to left

	//get action list start index
	m_actionStartIndex = m_index;

	//set actions
	//normal ability
	setAbility();

	/*
	//commander action
	if (m_unit->isCommander())
	{
		setButton("ManipulateTile", m_unit->canAct());
	}*/

	//join
	bool canJoin = !m_unit->isCommander() && !m_unit->checkTag(STRUCTURE);//not structure or commander
	canJoin = canJoin && m_unit->m_attributes[UNIT_LV] < 3;//not level 3
	if (canJoin)//not level 3
		setButton("Join", m_unit->canAct());

	//get end index
	m_actionEndIndex = m_index;

	/*
	//change y position
	if (m_yChange)
	{
		int delta = (m_actionEndIndex - m_actionStartIndex) * m_buttonScaleY;
		for (int i = m_actionStartIndex; i < m_index; i++)
		{
			m_buttonList[i]->getTransform().move2D(0, delta);
		}
	}*/
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
		m_actionShow = false;
	}
}

void unit::ActionButtonStore::hideAction()
{
	if (m_actionShow)
	{
		for (int i = m_actionStartIndex; i < m_actionEndIndex; i++)
		{
			m_buttonList[i]->setEnabled(false);
		}
	}
}

bool unit::ActionButtonStore::isDisplay()
{
	return m_show;
}

void unit::ActionButtonStore::listenEvent(kitten::Event::EventType p_type, kitten::Event * p_event)
{
	//right click can cancel button display
	if (p_type == kitten::Event::Right_Clicked)
	{
		hide();
	}
}

void unit::ActionButtonStore::createNewButton()
{
	kitten::K_GameObject* ab = kitten::K_GameObjectManager::getInstance()->createNewGameObject("unit_action_button.json");
	ab->getComponent<ActionSelect>()->setStorage(this); 
	ab->getComponent<ActionSelect>()->setActionMap(m_map);
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

	userinterface::ClickableButton* cb = go->getComponent<userinterface::ClickableButton>();
	if (p_cd > 0 || !p_a)
	{		
		cb->setActive(false);
	}
	else
	{
		cb->setActive(true);
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
		if (it->m_intValue[UNIT_LV] <= lv)//check level
		{
			if (!m_unit->canAct())//check can act
			{
				setButton(it->m_stringValue[ABILITY_NAME], false);
			}
			else
			{
				int cd = m_unit->checkCD(it->m_stringValue[ABILITY_NAME]);//check cd
				if(cd > 0)
					setButton(it->m_stringValue[ABILITY_NAME], false, cd);
				else
				{
					//check disable
					int disable = it->m_intValue[ABILITY_DISABLE];
					if (disable)
						setButton(it->m_stringValue[ABILITY_NAME], false);
					else
						setButton(it->m_stringValue[ABILITY_NAME], true);
				}

			}
		}
	}
}

void unit::ActionButtonStore::registerEvent()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Right_Clicked,
		this,
		std::bind(&ActionButtonStore::listenEvent, this, std::placeholders::_1, std::placeholders::_2));

}

void unit::ActionButtonStore::deregisterEvent()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::Right_Clicked, this);
}
