#include "SetCommanderOnClick.h"
#include "components/DeckAlterationComponent.h"
#include "components/DisplayFrame/CommanderDisplayFrame.h"

SetCommanderOnClick::SetCommanderOnClick() { }

SetCommanderOnClick::~SetCommanderOnClick() {	

}

void SetCommanderOnClick::onClick()
{
	// This Component assumes that the object it's going to be attached to is going to be part of the display Frame object. Specifically the commander version.
	DeckAlterationComponent::getActiveInstance()->getDeckData()->commanderID =
		CommanderDisplayFrame::getActiveInstance()->getCommanderTiedTo(&this->m_attachedObject->getTransform().getParent()->getAttachedGameObject()
		);

	//clear deck, remove other factions unit
	DeckAlterationComponent::getActiveInstance()->clearDeckForCommander();

	CommanderDisplayFrame::getActiveInstance()->refreshCommander(); // do this first, 
	CommanderDisplayFrame::getActiveInstance()->updateDisplay();
}