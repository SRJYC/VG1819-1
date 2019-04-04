#include "ChangeModeDetailOnClick.h"
#include "ModeDetailController.h"

ChangeModeDetailOnClick::ChangeModeDetailOnClick()
	:m_controller(nullptr),
	m_ui(nullptr)
{
}

ChangeModeDetailOnClick::~ChangeModeDetailOnClick()
{
}

void ChangeModeDetailOnClick::start()
{
	ClickableUI::start();

	//change texture
	m_ui = m_attachedObject->getComponent<userinterface::UIObject>();
	m_ui->setTexture(m_img.c_str());
}

void ChangeModeDetailOnClick::setController(ModeDetailController * p_controller)
{
	m_controller = p_controller;
}

void ChangeModeDetailOnClick::setInfo(const std::string & p_img, int p_id)
{
	if (m_ui == nullptr)
		m_img = p_img;
	else
		m_ui->setTexture(p_img.c_str());

	m_id = p_id;
}

void ChangeModeDetailOnClick::onClick()
{
	m_controller->changeDescription(m_id);
}
