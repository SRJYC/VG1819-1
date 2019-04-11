#pragma once
#include "ContextMenu.h"
#include "kitten\event_system\Event.h"
#include "kitten\event_system\EventManager.h"
#include "puppy\Text\TextBox.h"
#include "unit\Unit.h"

namespace userinterface
{
	class CommanderContext : public ContextMenu
	{
	public:
		CommanderContext();
		~CommanderContext();
		void start() override;
	private:
		unit::Unit* m_attachedCommander;
		void attachCommander(unit::Unit* p_commanderToAttach);
		void commanderLoadListener(kitten::Event::EventType p_type, kitten::Event* p_event);
		void receiveCommanderDamage(kitten::Event::EventType p_type, kitten::Event* p_event);

		puppy::TextBox* m_HP;
		int m_currentHP;
		
	};
}
