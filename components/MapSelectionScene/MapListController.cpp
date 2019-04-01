#include "MapListController.h"
#include "kitten/InputManager.h"
#include "kitten/event_system/EventManager.h"

MapListController::MapListController()
{
}

MapListController::~MapListController()
{
}

void MapListController::update()
{
	input::InputManager* in = input::InputManager::getInstance();
	if (in->keyDown('X') && !in->keyDownLast('X'))
	{
		kitten::Event* ev = new kitten::Event(kitten::Event::Update_Map_Detail);
		ev->putString("Image", m_mapList[1].imgPath);
		ev->putString("Text", m_mapList[1].description);
		kitten::EventManager::getInstance()->queueEvent(kitten::Event::Update_Map_Detail,ev);
	}
}

void MapListController::start()
{
	m_mapList = kibble::MapReader::getInstance()->getMapList();
}
