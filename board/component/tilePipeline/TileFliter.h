/*Rock 10.26

The filter that will remove tiles from list.

*/

#pragma once
#include "board/BoardManager.h"

class TilePipeline;
class TileFilter
{
public:
	TileFilter(TilePipeline* p_p);
	~TileFilter();

	virtual void filter(kitten::Event::TileList* p_list);
	void enable();
	void disable();
protected:
	TilePipeline* m_pipe;
	bool m_enabled = false;

	virtual bool shouldRemove(kitten::K_GameObject* p_tileGO) = 0;
};

class UnitFilter : public TileFilter
{//remove tiles that has unit on it
public:
	UnitFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);
protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class OwnedTileFilter : public TileFilter
{//remove tiles that has owner
public:
	OwnedTileFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);
protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class UnownedTileFilter : public TileFilter
{//remove tiles that owner isn't player
public:
	UnownedTileFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class NoUnitFilter : public TileFilter
{//remove tiles that doesn't have unit
public:
	NoUnitFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class NoCommanderFilter : public TileFilter
{//remove tiles that doesn't have commander
public:
	NoCommanderFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class EnemyFilter : public TileFilter
{//remove tiles that has enemy
public:
	EnemyFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class AllyFilter : public TileFilter
{//remove tiles that has ally
public:
	AllyFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class NotDPFilter : public TileFilter
{//remove tiles that's not demonic presence
public:
	NotDPFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class IsDPFilter : public TileFilter
{//remove tiles that is demonic presence
public:
	IsDPFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class SelfFilter : public TileFilter
{//remove tiles that is source
public:
	SelfFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class StructureFilter : public TileFilter
{//remove units that is structure
public:
	StructureFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class NonStructureFilter : public TileFilter
{//remove units that is not structure
public:
	NonStructureFilter(TilePipeline* p_p) :TileFilter(p_p) {};
	//void filter(kitten::Event::TileList* p_list);

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

class WaterFilter : public TileFilter
{//remove water tiles
public:
	WaterFilter(TilePipeline* p_p) :TileFilter(p_p) {};

protected:
	bool shouldRemove(kitten::K_GameObject* p_tileGO);
};

