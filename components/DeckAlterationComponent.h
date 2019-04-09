#pragma once
#include "kitten/K_Component.h"
#include "DeckData.hpp"
#include "kitten\K_GameObjectManager.h"

class DeckAlterationComponent : public kitten::K_Component
{
private:
	DeckData* m_source;
	int m_deckId = -1; bool m_saved = false;
public:
	static DeckAlterationComponent* getActiveInstance();

	void copyDeckData(const DeckData* p_source);
	void setDeckId(int p_id) { m_deckId = p_id; }
	void saveDeck();
	void discardChanges();
	void clearDeck();
	void clearDeckForCommander();

	DeckData* getDeckData();
	const int getDeckId() const { return m_deckId; }

	DeckAlterationComponent();
	~DeckAlterationComponent();
};