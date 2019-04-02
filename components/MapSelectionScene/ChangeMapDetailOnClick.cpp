#include "ChangeMapDetailOnClick.h"
#include "MapListController.h"
ChangeMapDetailOnClick::ChangeMapDetailOnClick()
	:m_controller(nullptr),
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
	m_textBox = m_attachedObject->getComponent<puppy::TextBox>();
	if (m_text != "")
		m_textBox->setText(m_text);
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
