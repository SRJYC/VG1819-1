#include "ChangeMapDetailOnClick.h"
#include "MapListController.h"
#include "kitten/K_GameObjectManager.h"

ChangeMapDetailOnClick::ChangeMapDetailOnClick(const std::string& p_textBoxData)
	:
	m_textBoxData(p_textBoxData),
	m_controller(nullptr),
	m_textBox(nullptr),
	m_mapId(-1),
	m_text("")
{
}

ChangeMapDetailOnClick::~ChangeMapDetailOnClick()
{
}

void ChangeMapDetailOnClick::start()
{
	ClickableUI::start();

	//create textbox object
	kitten::K_GameObject* go = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_textBoxData);
	
	//get text box
	m_textBox = go->getComponent<puppy::TextBox>();
	if (m_text != "")
		m_textBox->setText(m_text);

	//set object to be child
	go->getTransform().setIgnoreParent(false);
	go->getTransform().setParent(&getTransform());

	//move it to front
	go->getTransform().move(0, 0, 0.01);
}

void ChangeMapDetailOnClick::setController(MapListController * p_controller)
{
	m_controller = p_controller;
}

void ChangeMapDetailOnClick::setInfo(const std::string & p_text, int p_mapId)
{
	if (m_textBox == nullptr)//save text until get text box component
		m_text = p_text;
	else
		m_textBox->setText(p_text);

	m_mapId = p_mapId;
}

void ChangeMapDetailOnClick::onClick()
{
	assert(m_controller != nullptr);//didn't set controller

	m_controller->updateMapDetail(m_mapId);
}
