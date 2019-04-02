#pragma once 

namespace AI {
	namespace Extract {
		struct PowerTracker {
			int m_iMaxPower = 0;
			int m_iCurrentPower = 0;
			
			void increaseMaxPower(int p_iAmount = 1) { m_iMaxPower += p_iAmount; }
			void useCurrentPower(int p_iAmount = 1) { m_iCurrentPower -= p_iAmount; }
			void resetCurrent() { m_iCurrentPower = m_iMaxPower; }

			bool checkPowerAmountUsable(int p_iAmount) { return m_iCurrentPower > p_iAmount; }
		};
	}
}