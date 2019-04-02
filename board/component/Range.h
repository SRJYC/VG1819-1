
//@Rock
//This is a component of the board manager. 
//It keeps tile's coordinate.
//It will be used when unit use range ability, like move


#pragma once
#include "kitten/K_Component.h"
#include "board/component/BoardCreator.h"
#include "unit/Unit.h"
#include "kitten/event_system/EventManager.h"

class Range
{
public:
	Range();
	~Range();

	void setDimension(int p_x, int p_z);
	void clear();

	kitten::Event::TileList getTilesInRange(kitten::Event * p_data);

	kitten::Event::TileList getTilesInRange(kitten::K_GameObject* p_tileAtOrigin,int p_minRange, int p_maxRange);

	kitten::Event::TileList getPath(kitten::K_GameObject * p_target, kitten::K_GameObject * p_origin);

private:
	//board dimension
	int m_x;
	int m_z;

	//range
	int m_min;
	int m_max;

	kitten::K_GameObject* m_origin;
	std::pair<int, int> m_originPos;

	kitten::Event::TileList getList() const;
	void addTile(kitten::Event::TileList* p_list, int p_tileX, int p_tileZ) const;

	//path node
	struct node
	{
		std::pair<int, int> tile;
		int cost;
		node* parent = nullptr;
	};

	std::map<std::pair<int, int>, node*> m_nodeMap;

	std::map<std::pair<int, int>, node*> getPathRange() const;

	void checkTile
	(std::map<std::pair<int, int>, node*>* p_nodeMap,
		node* p_parentNode,
		std::pair<int, int> p_currentTile) const;
};