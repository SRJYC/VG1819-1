/*
@Rock
This class handles the action that the button will perform
basically same as SelectAbility
*/

#pragma once

#include "kitten/mouse picking/ClickableUI.h"
#include "puppy\Text\TextBox.h"
#include "unit/Unit.h"

namespace unit
{
	class ActionButtonStore;
	class ActionSelect : public kitten::ClickableUI
	{
		friend class ActionMap;
	private:
		const float m_textZ = 0.1;
		const std::pair<int, int> m_txtOffset;
		const std::pair<int,int> m_cdOffset;

		bool m_actionShow;//flag for display action list

		bool m_active;
		int m_cd;
		std::string m_action;

		Unit* m_unit;

		kitten::K_GameObject* m_text;
		kitten::K_GameObject* m_cdtext;

		ActionButtonStore* m_storage;
		ActionMap* m_map;

	public:
		ActionSelect(const std::pair<int,int> p_to, const std::pair<int, int> p_co);
		~ActionSelect();

		void start() override;

		void setStorage(ActionButtonStore* p_s) {m_storage = p_s;};
		void setActionMap(ActionMap* p_m) { m_map = p_m; };
		void setUnit(Unit* p_u);
		void setAction(const std::string& p_a, int p_cd = 0);
		void setActive(bool p_a) { m_active = p_a; };
		const std::string getAction() { return m_action; };

		//void act();

		void onClick() override;
		void onDisabled() override;
	};
}
