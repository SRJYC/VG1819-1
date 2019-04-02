#pragma once
#include "unit/Unit.h"

namespace AI {
	namespace Extract {
		struct Unit {
			int m_kibbleID;
			std::unordered_map<std::string, int> m_attributes;
			std::vector<unit::AbilityDescription*> m_ADList;
			bool m_isCommander;
			unit::Unit* m_originalData;

			Unit(unit::Unit* p_data): m_attributes(p_data->m_attributes), m_ADList(p_data->m_ADList), m_isCommander(p_data->isCommander()),
				m_originalData(p_data)
			{

			}
		};
	}

}