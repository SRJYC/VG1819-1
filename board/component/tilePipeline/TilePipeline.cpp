#include "TilePipeline.h"

TilePipeline::TilePipeline()
{
	//init all filter
	TileFilter* filter;

	filter = new UnitFilter(this);
	m_filterList[FILTER_UNIT] = filter;

	filter = new NoUnitFilter(this);
	m_filterList[FILTER_NO_UNIT] = filter;

	filter = new OwnedTileFilter(this);
	m_filterList[FILTER_OWNED_TILE] = filter;

	filter = new UnownedTileFilter(this);
	m_filterList[FILTER_UNOWNED_TILE] = filter;

	filter = new NoCommanderFilter(this);
	m_filterList[FILTER_NO_COMMANDER] = filter;

	filter = new EnemyFilter(this);
	m_filterList[FILTER_ENEMY] = filter;

	filter = new AllyFilter(this);
	m_filterList[FILTER_ALLY] = filter;

	filter = new IsDPFilter(this);
	m_filterList[FILTER_IS_DP] = filter;

	filter = new NotDPFilter(this);
	m_filterList[FILTER_NOT_DP] = filter;

	filter = new SelfFilter(this);
	m_filterList[FILTER_SELF] = filter;

	filter = new StructureFilter(this);
	m_filterList[FILTER_STRUCTURE] = filter;

	filter = new NonStructureFilter(this);
	m_filterList[FILTER_NON_STRUCTURE] = filter;

	filter = new WaterFilter(this);
	m_filterList[FILTER_WATER] = filter;
}

TilePipeline::~TilePipeline()
{
	for (auto it = m_filterList.begin(); it != m_filterList.end(); it++)
	{
		delete it->second;
	}
}

void TilePipeline::filterList(kitten::Event::TileList * p_list)
{
	for (auto it = m_filterList.begin(); it != m_filterList.end(); it++)
	{
		it->second->filter(p_list);
	}

	//resetFilter();
}

void TilePipeline::useFilter(const std::string & p_filter)
{
	m_filterList[p_filter]->enable();
}

void TilePipeline::setSource(unit::Unit * p_u)
{
	m_source = p_u;
}

void TilePipeline::resetFilter()
{
	m_source = nullptr;
	for (auto it = m_filterList.begin(); it != m_filterList.end(); it++)
	{
		it->second->disable();
	}
}
