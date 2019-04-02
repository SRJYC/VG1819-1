#pragma once 
#include "unit/Unit.h"

namespace AI {
	namespace Extract {
		struct Hand {
			std::vector<unit::Unit*> m_cards;

			Hand() {
				m_cards.reserve(5);
			}

			bool canAddCard() { return m_cards.size() < 5; }

			bool addCard(unit::Unit* p_unit) {
				if (m_cards.size() >= 5) return false;
				m_cards.push_back(p_unit);
				return true;
			}

			unit::Unit* takeOutCard(unsigned int p_index) {
				unit::Unit* card = m_cards[p_index];
				m_cards.erase(m_cards.begin() + p_index);
				return card;
			}

			unit::Unit* checkOutCard(unsigned int p_index) { return m_cards[p_index]; }
		};
	}
}