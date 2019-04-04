/*@Rock
Apr 1
*/

#pragma once
#include "kitten/K_Common.h"
#include "UI/UIObject.h"
#include "puppy/Text/TextBox.h"
#include "kitten/event_system/EventManager.h"

class MapDetailController : public kitten::K_Component
{
public:
	MapDetailController();
	~MapDetailController();

	void start() override;

private:
	userinterface::UIObject* m_uiObject;
	puppy::TextBox* m_textBox;

	void registerEvent();
	void deregisterEvent();
	void listenEvent(kitten::Event::EventType p_type, kitten::Event* p_data);
};