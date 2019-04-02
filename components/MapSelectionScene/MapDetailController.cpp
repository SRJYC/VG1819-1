#include "MapDetailController.h"

MapDetailController::MapDetailController()
	:m_uiObject(nullptr),
	m_textBox(nullptr)
{
}

MapDetailController::~MapDetailController()
{
	deregisterEvent();
}

void MapDetailController::start()
{
	//get texture or text box
	m_uiObject = m_attachedObject->getComponent<userinterface::UIObject>();
	m_textBox = m_attachedObject->getComponent<puppy::TextBox>();


	registerEvent();
}

void MapDetailController::registerEvent()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Update_Map_Detail,
		this,
		std::bind(&MapDetailController::listenEvent, this, std::placeholders::_1, std::placeholders::_2));
}

void MapDetailController::deregisterEvent()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::Update_Map_Detail, this);
}

void MapDetailController::listenEvent(kitten::Event::EventType p_type, kitten::Event* p_data)
{
	if (p_type == kitten::Event::EventType::Update_Map_Detail)
	{
		//update texture if it exists
		if (m_uiObject != nullptr)
		{
			std::string img = p_data->getString("Image");
			m_uiObject->setTexture(img.c_str());
		}

		//update description if it exists
		if (m_textBox != nullptr)
		{
			std::string text = p_data->getString("Text");
			m_textBox->setText(text);
		}
	}
}
