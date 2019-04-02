#include "Deck.h"

namespace AI {
	namespace Extract {
		void Deck::setDeckSource(const DeckData * p_source) { m_deckSource = p_source; }
		Deck::InteractionState AI::Extract::Deck::setupDeck() {

			// Set up Pool
			this->m_cardPool.reserve(m_deckSource->totalCardCount);
			for (std::pair<int, int> card : m_deckSource->cards) {
				this->m_cardPool.resize(m_cardPool.size() + card.second, card.first); // expand it by repetition, and value set by index of card
			}

			// Shuffle the pool
			std::shuffle(m_cardPool.begin(), m_cardPool.end(), RNGZUZ);

			return InteractionState(m_cardPool.size());
		}

		unsigned int Deck::getCardsLeftInDeck() {
			return m_cardPool.size();
		}

		Deck::InteractionState Deck::discardTop() {
			if (m_cardPool.size() == 0) return InteractionState(Bad, m_cardPool.size());
			int cardID = m_cardPool.back();
			m_cardPool.pop_back();
			return InteractionState(OK, cardID, m_cardPool.size());
		}

		Deck::InteractionState Deck::peekTop() {
			if (m_cardPool.size() == 0) return InteractionState(Bad, m_cardPool.size());
			return InteractionState(OK, m_cardPool.back(), m_cardPool.size());
		}

		Deck::InteractionState Deck::drawTop() {
			if (m_cardPool.size() == 0) return InteractionState(Bad, m_cardPool.size());
			int cardID = m_cardPool.back();
			m_cardPool.pop_back();
			return InteractionState(OK, cardID, m_cardPool.size());
		}

		Deck::InteractionState AI::Extract::Deck::shuffle() {
			if (m_cardPool.size() == 0) return InteractionState(Did_Nothing, m_cardPool.size());
			std::shuffle(m_cardPool.begin(), m_cardPool.end(), RNGZUZ);
			return InteractionState(OK, m_cardPool.size());
		}
		Deck::InteractionState::InteractionState(unsigned int p_deckCardsLeft) { deckSizeLeft = p_deckCardsLeft; }
		Deck::InteractionState::InteractionState(Status p_status, unsigned int p_deckCardsLeft) { status = p_status; deckSizeLeft = p_deckCardsLeft; }
		Deck::InteractionState::InteractionState(Status p_status, int p_card, unsigned int p_deckCardsLeft) { status = p_status; card = p_card; deckSizeLeft = p_deckCardsLeft; }
	}
}