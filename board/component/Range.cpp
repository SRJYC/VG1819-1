#include "Range.h"
#include "board/tile/TileInfo.h"
#include "board/BoardManager.h"
#include <iostream>

Range::Range()
{
}

Range::~Range()
{
	clear();
}

void Range::setDimension(int p_x, int p_z)
{
	m_x = p_x;
	m_z = p_z;
}

void Range::clear()
{
	m_min = 0;
	m_max = 0;
	m_origin = nullptr;

	for (auto node : m_nodeMap)
	{
		delete node.second;
	}
	m_nodeMap.clear();
}

kitten::Event::TileList Range::getTilesInRange(kitten::Event * p_data)
{
	kitten::K_GameObject * tileAtOrigin = p_data->getGameObj("tileAtOrigin");
	m_min = p_data->getInt("min_range");
	m_max = p_data->getInt("max_range");
	m_origin = tileAtOrigin;
	m_originPos = m_origin->getComponent<TileInfo>()->getPos();

	if (p_data->getInt("path") == TRUE)
	{
		m_nodeMap = getPathRange();

		kitten::Event::TileList list;
		for (auto node : m_nodeMap)
		{
			int cost = node.second->cost;
			if (cost >= m_min && cost <= m_max)
			{
				list.push_back(node.second->tile);
			}
		}
		return list;
	}

	return getList();
}

kitten::Event::TileList Range::getTilesInRange(kitten::K_GameObject * p_tileAtOrigin, int p_minRange, int p_maxRange)
{
	m_min = p_minRange;
	m_max = p_maxRange;
	m_origin = p_tileAtOrigin;
	m_originPos = m_origin->getComponent<TileInfo>()->getPos();

	return getList();
}

kitten::Event::TileList Range::getPath(kitten::K_GameObject * p_target, kitten::K_GameObject * p_origin)
{
	if (m_origin == p_origin)
	{
		std::pair<int, int> targetPos = p_target->getComponent<TileInfo>()->getPos();
		
		auto found = m_nodeMap.find(targetPos);
		if (found != m_nodeMap.end())
		{
			//get node
			node* n = found->second;

			kitten::Event::TileList list;
			
			while (n->tile != m_originPos)
			{
				list.push_back(n->tile);
				n = n->parent;
			}
			std::reverse(list.begin(), list.end());
			return list;
		}
	}
	return kitten::Event::TileList();
}

kitten::Event::TileList Range::getList() const
{
	kitten::Event::TileList list;

	for (int xStep = m_max; xStep >= -m_max; xStep--)
	{
		int alter = m_max - xStep;
		if (xStep < 0)
			alter = xStep + m_max;

		for (int zStep = -alter; zStep <= alter; zStep++)
		{
			int x = m_originPos.first + xStep;
			int z = m_originPos.second + zStep;
			std::cout << "tile:" << x << " , " << z << std::endl;
			addTile(&list, x, z);
		}
	}

	return list;
}

void Range::addTile(kitten::Event::TileList * p_list, int p_tileX, int p_tileZ) const
{
	//within dimension
	if (p_tileX >= 0 && p_tileX < m_x && p_tileZ >= 0 && p_tileZ < m_z)
	{
		//within range
		int distance = 0;
		distance += std::abs(p_tileX - m_originPos.first);
		distance += std::abs(p_tileZ - m_originPos.second);
		if (distance >= m_min && distance <= m_max)
		{
			p_list->push_back(std::make_pair(p_tileX,p_tileZ));
		}
	}
}

std::map<std::pair<int, int>, Range::node*> Range::getPathRange() const
{
	std::map<std::pair<int, int>, node*> nodeMap;

	int x = m_originPos.first;
	int z = m_originPos.second;

	//create first node
	node* originNode = new node();
	originNode->tile = m_originPos;
	originNode->cost = 0;

	//insert to map
	nodeMap.insert(std::make_pair(m_originPos, originNode));

	checkTile(&nodeMap, originNode,	std::pair<int, int>(x - 1, z));
	checkTile(&nodeMap, originNode, std::pair<int, int>(x + 1, z));
	checkTile(&nodeMap, originNode, std::pair<int, int>(x, z - 1));
	checkTile(&nodeMap, originNode, std::pair<int, int>(x, z + 1));

	return nodeMap;
}

void Range::checkTile(std::map<std::pair<int, int>, node*>* p_nodeMap, node * p_parentNode, std::pair<int, int> p_currentTile) const
{
	//out of board
	if (p_currentTile.first >= m_x || p_currentTile.first < 0 || p_currentTile.second >= m_z || p_currentTile.second < 0)
		return;

	//get tile info
	TileInfo* info = BoardManager::getInstance()->getTile(p_currentTile.first, p_currentTile.second)->getComponent<TileInfo>();

	//dont add tile has unit exclude origin tile
	if (info->hasUnit())
		return;

	//get mv cost
	int cost = info->getMVCost();

	//distance is preivouse tile cost,add it
	cost += p_parentNode->cost;

	//if the distance is excess the max range, we dont go further
	if (cost > m_max)
		return;

	node* n = nullptr;

	//check if the tile is already in the list
	auto found = p_nodeMap->find(p_currentTile);
	if (found != p_nodeMap->end())
	{
		n = found->second;
		//it's in the list, then compare the distance
		if (cost < n->cost)
		{
			n->cost = cost;
			n->parent = p_parentNode;
			//always keep the least distance
		}
		else//avoid repeat check
			return;
	}
	else
	{
		n = new node();
		n->tile = p_currentTile;
		n->cost = cost;
		n->parent = p_parentNode;
			
		//not in the list, add it
		p_nodeMap->insert(std::make_pair(p_currentTile, n));
	}

	//then check neighbour
	int x = p_currentTile.first;
	int z = p_currentTile.second;

	checkTile(p_nodeMap, n,	std::pair<int, int>(x - 1, z));
	checkTile(p_nodeMap, n,	std::pair<int, int>(x + 1, z));
	checkTile(p_nodeMap, n,	std::pair<int, int>(x, z - 1));
	checkTile(p_nodeMap, n,	std::pair<int, int>(x, z + 1));
}

