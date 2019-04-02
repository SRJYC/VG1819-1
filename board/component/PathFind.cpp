#include "PathFind.h"
#include "board/BoardManager.h"
#include <algorithm>
PathFind::PathFind()
{
	std::pair<int, int> d = BoardManager::getInstance()->getDimension();
	m_maxX = d.first;
	m_maxZ = d.second;
}

PathFind::~PathFind()
{
	if (!m_isClear) 
	{ 
		clear(); 
	}
}

kitten::Event::TileList PathFind::getPath(std::pair<int, int> p_start, std::pair<int, int> p_end, int p_mv)
{
	m_isClear = false;

	m_mv = p_mv;
	m_target = p_end;

	m_start = new node();
	m_start->tile = p_start;
	m_start->cost = 0;
	getNeighbour(m_start);

	while (!m_openList.empty())
	{
		node* last = getNodeInOpen();

		getNeighbour(last);
		m_closedList.push_back(last);

		if (last->tile == m_target)
		{
			kitten::Event::TileList list = getList(last);

			clear();

			return list;
		}
	}

	return kitten::Event::TileList();
}

PathFind::node* PathFind::getNodeInOpen()
{
	int leastWeight = INT_MAX;
	int index;

	for(int i=0;i<m_openList.size();i++)
	{
		if (m_openList[i]->weight < leastWeight)
		{
			leastWeight = m_openList[i]->weight;
			index = i;
		}
	}

	node* n = m_openList[index];
	m_openList.erase(m_openList.begin() + index);//remove from open

	return n;
}

void PathFind::getNeighbour(node* p_node)
{
	std::pair<int, int> tile = p_node->tile;
	int cost = p_node->cost;

	std::pair<int,int> p = std::make_pair(tile.first+1, tile.second);
	if (checkTile(p, cost))
		addNodeToOpen(p, cost, p_node);

	p = std::make_pair(tile.first-1, tile.second);
	if (checkTile(p, cost))
		addNodeToOpen(p, cost, p_node);

	p = std::make_pair(tile.first, tile.second+1);
	if (checkTile(p, cost))
		addNodeToOpen(p, cost, p_node);

	p = std::make_pair(tile.first, tile.second-1);
	if (checkTile(p, cost))
		addNodeToOpen(p, cost, p_node);
}

bool PathFind::checkTile(std::pair<int, int> p_tile, int p_cost)
{
	//out of board
	if (p_tile.first < 0 || p_tile.second < 0)
		return false;
	if (p_tile.first >= m_maxX || p_tile.second >= m_maxZ)
		return false;

	kitten::K_GameObject* tileGO = BoardManager::getInstance()->getTile(p_tile.first, p_tile.second);
	TileInfo* info = tileGO->getComponent<TileInfo>();
	//has unit
	if (info->hasUnit())
		return false;
	//no more mv
	int mvCost = info->getMVCost();
	if (m_mv < mvCost + p_cost)
		return false;

	return true;
}

void PathFind::addNodeToOpen(std::pair<int, int> p_tile, int p_cost, node* p_parent)
{
	m_isClear = false;

	node* n = new node();
	n->tile = p_tile;

	n->estimate = abs(m_target.first - p_tile.first) + abs(m_target.second - p_tile.second);

	kitten::K_GameObject* tileGO = BoardManager::getInstance()->getTile(p_tile.first, p_tile.second);
	TileInfo* info = tileGO->getComponent<TileInfo>();
	n->cost = info->getMVCost() + p_cost;

	n->weight = n->cost + n->estimate;

	for (int i = 0; i < m_openList.size(); i++)
	{
		if (m_openList[i]->tile == n->tile)//already in open list
		{
			//check weight
			if (m_openList[i]->weight < n->weight)
			{
				delete n;
				return;
			}
		}
	}

	for (int i = 0; i < m_closedList.size(); i++)
	{
		if (m_closedList[i]->tile == n->tile)//already in closed list
		{
			//check weight
			if (m_closedList[i]->weight < n->weight)
			{
				delete n;
				return;
			}
		}
	}

	n->parent = p_parent;
	m_openList.push_back(n);
}

kitten::Event::TileList PathFind::getList(node* p_n)
{
	kitten::Event::TileList list;
	node* n = p_n;
	while (n != m_start)
	{
		list.push_back(n->tile);
		n = n->parent;
	}
	std::reverse(list.begin(), list.end());

	return list;
}

void PathFind::clear()
{
	for (int i = 0; i < m_openList.size(); i++)
	{
		delete m_openList[i];
	}

	for (int i = 0; i < m_closedList.size(); i++)
	{
		delete m_closedList[i];
	}

	m_openList.clear();
	m_closedList.clear();

	delete m_start;

	m_isClear = true;
}
