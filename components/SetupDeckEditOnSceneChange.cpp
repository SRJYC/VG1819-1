#include "SetupDeckEditOnSceneChange.h"
#include "kibble/kibble.hpp"
#include "kitten/K_Instance.h"
#include "DeckAlterationComponent.h"
#include "DeckInitializingComponent.h"

void SetupDeckEditOnSceneChange::onClick() {
	if (DeckInitializingComponent::getActiveInstance() == nullptr ||
		DeckInitializingComponent::getActiveInstance()->getDeckData() == nullptr ||
		DeckAlterationComponent::getActiveInstance() == nullptr
		)// There's no point of changing scenes if there isn't a selected deck. DeckAlteration component should have been initialized alongside the Initializing component
	{
		DeckInitializingComponent::getActiveInstance()->toggleMessage(true);
		return;
	}
	DeckAlterationComponent::getActiveInstance()->discardChanges();
	DeckAlterationComponent::getActiveInstance()->setDeckId(DeckInitializingComponent::getActiveInstance()->getDeckId());
	DeckAlterationComponent::getActiveInstance()->copyDeckData(DeckInitializingComponent::getActiveInstance()->getDeckData());

	kitten::K_Instance::changeScene(m_targetScene);
}

SetupDeckEditOnSceneChange::SetupDeckEditOnSceneChange(const std::string& p_sceneTarget) : m_targetScene(p_sceneTarget) {}
SetupDeckEditOnSceneChange::~SetupDeckEditOnSceneChange() {}
