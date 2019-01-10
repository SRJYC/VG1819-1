// CardContext
//
// Displays the stats of the hovered card
// Uses TextBoxes aligned to the card_context.tga texture to display 
// the stats
// 
//
// @Ken

#pragma once
#include "kitten\K_Component.h"
#include "unit\Unit.h"
#include "puppy\Text\TextBox.h"
#include "UI\UIObject.h"
#include <vector>
#include "kitten\event_system\EventManager.h"

class CardContext : public kitten::K_Component
{
private:
	puppy::TextBox* m_nameBox;
	puppy::TextBox* m_tagsBox;
	puppy::TextBox* m_hpBox;
	puppy::TextBox* m_mvBox;
	puppy::TextBox* m_initiativeBox;
	puppy::TextBox* m_costBox;

	std::vector<puppy::TextBox*> m_abilityList;
	std::vector<puppy::TextBox*> m_statusList;

	kitten::K_GameObject* m_cardTexture;
	userinterface::UIObject* m_unitPortrait;

	unit::Unit* m_unitData;

	// Testing
	int m_unitId = 1;

public:
	CardContext();
	~CardContext();

	virtual void start() override;

	// For testing only, changes the unit on the hovered card on B key pressed
	virtual bool hasUpdate() const override { return true; };
	virtual void update() override;

	void setUnit(unit::Unit* p_unit);
	void setUnitListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	void updateUnitData();

	virtual void onEnabled() override;
	virtual void onDisabled() override;

	void setEnabledListener(kitten::Event::EventType p_type, kitten::Event* p_event);

	virtual void onUnitDataChanged() { updateUnitData(); }
};