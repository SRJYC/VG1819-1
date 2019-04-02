/*Rock 10.26

Remove tiles that meets some conditions from tile list.

*/

#pragma once
#include "board/component/tilePipeline/TileFliter.h"
#include <unordered_map>

#define FILTER_NO_UNIT "no_unit"
#define FILTER_UNIT "unit"
#define FILTER_OWNED_TILE "owned_tile"
#define FILTER_UNOWNED_TILE "unowned_tile"
#define FILTER_NO_COMMANDER "non_commander"
#define FILTER_ENEMY "enemy"
#define FILTER_ALLY "ally"
#define FILTER_SELF "self"
#define FILTER_STRUCTURE "structure_machine"
#define FILTER_NON_STRUCTURE "non_structure_machine"
#define FILTER_WATER "water"

#define FILTER_NOT_DP "not_demonic_presence"
#define FILTER_IS_DP "is_demonic_presence"

#define FILTER "filter"
#define AREA_FILTER "area_filter"

class TileFilter;
class TilePipeline
{
public:
	unit::Unit* m_source;

	TilePipeline();
	~TilePipeline();

	void filterList(kitten::Event::TileList* p_list);

	void useFilter(const std::string& p_filter);
	void setSource(unit::Unit* p_u);

	void resetFilter();
private:
	std::unordered_map<std::string, TileFilter*> m_filterList;
};