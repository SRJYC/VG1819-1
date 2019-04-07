#include "DeckAlterationComponent.h"
#include "kibble/databank/databank.hpp"

DeckAlterationComponent* instance = nullptr;


DeckAlterationComponent::DeckAlterationComponent() : m_source(nullptr)
{
	instance = this;

}

DeckAlterationComponent::~DeckAlterationComponent()
{
	instance = nullptr;
	if (m_saved == false) delete m_source;
}

DeckAlterationComponent * DeckAlterationComponent::getActiveInstance()
{
	return instance;
}

// Copies data, remember that this deletes old one. 
void DeckAlterationComponent::copyDeckData(const DeckData* p_source)
{
	if(m_saved == false) delete m_source;
	m_source = new DeckData(*p_source);

}

DeckData* DeckAlterationComponent::getDeckData()
{
	return m_source;
}

void DeckAlterationComponent::saveDeck()
{
	if (m_source == nullptr) assert(false); // there is no deck chosen to alter
	if(m_deckId >= 0) 
		kibble::replaceDeckData(m_deckId,m_source);
	else 
	{
		m_deckId = kibble::addNewDeckData(m_source);
		m_saved = true;
	}
}

void DeckAlterationComponent::discardChanges()
{
	if(m_saved == false) delete m_source;
	m_source = nullptr;
	m_deckId = -1;
	m_saved = false;
}

void DeckAlterationComponent::clearDeck()
{//remove all cards in deck
	m_source->cards.clear();
}

void DeckAlterationComponent::clearDeckForCommander()
{//remove all cards in deck that aren't neutral or same faction as commander

	//get commander id
	int commanderId = m_source->commanderID;
	//get commander faction
	std::string commanderFaction = kibble::getFactionTagFor(commanderId);

	//then check every card in deck
	auto it = m_source->cards.begin();
	while (it != m_source->cards.end())
	{
		//get unit id
		int unitId = it->first;
		//get faction tag
		std::string unitFaction = kibble::getFactionTagFor(unitId);

		if (unitFaction != commanderFaction && unitFaction != "Neutral")
		{
			it = m_source->cards.erase(it);//remove this card
		}
		else
		{
			it++;//check next card
		}
	}
}
