#include "UnitDisplayFrame.h"
#include "kitten/K_GameObjectManager.h"
#include "kibble/databank/databank.hpp"
#include "puppy/Text/TextBox.h"
#include <math.h>
#include "components/DeckAlterationComponent.h"
#include "components/clickables/AddUnitOnClick.h"
#include <string>
#include "UI/UIFrame.h"
#include "unit/Unit.h"

UnitDisplayFrame* instance;
UnitDisplayFrame* UnitDisplayFrame::getActiveInstance() { return instance; }

UnitDisplayFrame::UnitDisplayFrame(int p_marginX, int p_marginY) : m_unitVector(kibble::getNonCommanderIds()), DisplayFrame(p_marginX, p_marginY,
	std::string("Deck/unit-display_frame_object.json"),
	std::string("Deck/deck_display-left_button.json"), std::string("Deck/deck_display-right_button.json"),
	std::string("Deck/unit_display-highlight.json"), std::string("Deck/general-display_empty.json"))// TODO add an empty object
{
	instance = this;
}

UnitDisplayFrame::~UnitDisplayFrame()
{
	instance = nullptr;

}

void UnitDisplayFrame::start()
{
	m_currentPick = -1;
	DisplayFrame::start();

	//get commander id
	int commanderId = DeckAlterationComponent::getActiveInstance()->getDeckData()->commanderID;

	kitten::Event* e = new kitten::Event(kitten::Event::Card_Context_Set_Enabled);
	e->putInt(CARD_CONTEXT_SET_ENABLED_KEY, TRUE);
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Card_Context_Set_Enabled, e);

	kitten::Event* updateContextEvent = new kitten::Event(kitten::Event::Update_Card_Context_By_ID);
	updateContextEvent->putInt(UPDATE_CARD_CONTEXT_KEY, commanderId);
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Update_Card_Context_By_ID, updateContextEvent);

	//change unit vector
	m_unitVector = kibble::getAvaliableUnitIdsForCommander(commanderId);

	//reupdate display
	updateDisplay();
}

int UnitDisplayFrame::getTargetAvailable() {
	return m_unitVector.size();
}

void UnitDisplayFrame::updateIndividualDisplayObject(int p_activeObjectIndex)
{
	//+ m_currentSet * m_objectsToDisplay.size() To get the deck id 
	const unit::Unit* unitData = kibble::getUnitFromId(m_unitVector[p_activeObjectIndex + m_currentSet * m_objectsToDisplay.size()]);

	// Set commander name. The components are in the order loaded in the file
	m_objectsToDisplay[p_activeObjectIndex]->getTransform().getChildren()[2]->getAttachedGameObject() // Third is the TextBox
		.getComponent<puppy::TextBox>()->setText(unitData->m_name);

	// Add picture 
	m_objectsToDisplay[p_activeObjectIndex]->getTransform().getChildren()[0]->getAttachedGameObject() // first is the ui frame
		.getComponent<userinterface::UIFrame>()->setTexture(unitData->getPortraitTexturePath().c_str());
}

void UnitDisplayFrame::onObjectClicked(int p_clickedDataSetIndex)
{
	kitten::Event* updateContextEvent = new kitten::Event(kitten::Event::Update_Card_Context_By_ID);
	updateContextEvent->putInt(UPDATE_CARD_CONTEXT_KEY, m_unitVector[p_clickedDataSetIndex]);
	kitten::EventManager::getInstance()->queueEvent(kitten::Event::Update_Card_Context_By_ID, updateContextEvent);
}
