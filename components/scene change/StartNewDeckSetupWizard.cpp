#include "StartNewDeckSetupWizard.h"
#include "components/DeckAlterationComponent.h"
#include "components/DeckInitializingComponent.h"
#include "kitten/InputManager.h"
#include "kibble/databank/databank.hpp"
#include "puppy/Text/TextBox.h"

#include "kitten\K_Instance.h"

StartNewDeckSetupWizard::StartNewDeckSetupWizard(bool p_switchSceneOnEnterPressed, const std::string& p_scene) : m_switchSceneOnEnterPressed(p_switchSceneOnEnterPressed),
	m_sceneToSwitch(p_scene)
{

}

StartNewDeckSetupWizard::~StartNewDeckSetupWizard()
{

	std::string deckName = m_stringInputDisplay->getString();
	if(deckName == "") deckName = m_attachedObject->getComponent<puppy::TextBox>()->getText();

	DeckData* deckData = new DeckData();
	deckData->name = deckName;

	DeckAlterationComponent::getActiveInstance()->discardChanges();
	DeckAlterationComponent::getActiveInstance()->copyDeckData(deckData);

	input::InputManager::getInstance()->setPollMode(true);

	if (m_switchSceneOnEnterPressed)
	{
		input::InputManager::getInstance()->removeStringListener(this);
	}

	delete deckData;
}

void StartNewDeckSetupWizard::onStringFinished(const std::string& p_string)
{
	kitten::K_Instance::changeScene(m_sceneToSwitch);
}

void StartNewDeckSetupWizard::onStringChanged(const std::string& p_string)
{

}

void StartNewDeckSetupWizard::start()
{
	input::InputManager::getInstance()->setPollMode(false);
	m_stringInputDisplay = m_attachedObject->getComponent<StringInputDisplay>();
	assert(m_stringInputDisplay != nullptr);

	if (m_switchSceneOnEnterPressed)
	{
		input::InputManager::getInstance()->addStringListener(this);
	}
}