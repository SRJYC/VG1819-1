#pragma once
#include "DeckData.hpp"
#include <random>
#include <chrono> 
#include <algorithm>
#include <climits>

namespace AI {
	namespace Extract {
		struct Deck {
			std::default_random_engine RNGZUZ = std::default_random_engine(std::random_device{}());// our holy lord, determiner of fate

			const DeckData* m_deckSource;
			std::vector<int> m_cardPool;

			enum Status {
				Unknown,
				Bad,
				Did_Nothing,
				OK
			};

			struct InteractionState {
				Status status = OK;
				int card = -1;
				unsigned int deckSizeLeft = 0;

				InteractionState(unsigned int p_deckCardsLeft);
				InteractionState(Status p_status, unsigned int p_deckCardsLeft);
				InteractionState(Status p_status, int p_card, unsigned int p_deckCardsLeft);
			};

			void setDeckSource(const DeckData* p_source);

			InteractionState setupDeck();

			unsigned int getCardsLeftInDeck();

			InteractionState discardTop();

			InteractionState peekTop();

			InteractionState drawTop();

			InteractionState shuffle();

		};

	}
}