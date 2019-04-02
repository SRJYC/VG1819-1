#include "AreaPattern.h"
#include "board/BoardManager.h"

kitten::Event::TileList PathPattern::getTileList(AreaInfo * p_info)
{
	getInfo(p_info);

	//using range;


	/*
	PathFind p;

	TileInfo* end = p_info->m_pivot->getComponent<TileInfo>();
	TileInfo* start = p_info->m_origin->getComponent<TileInfo>();

	kitten::Event::TileList list = p.getPath(start->getPos(), end->getPos(), m_len);
	*/
	//delete r;

	return BoardManager::getInstance()->getRangeComponent()->getPath(p_info->m_pivot, p_info->m_origin);
}