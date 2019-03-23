#pragma once
#include "../GameModeManager.h"
#include "ItemSpawnArea.h"

class ItemDropArea : public GameModeComponent
{
public:
	ItemDropArea(int p_id, ItemSpawnArea* p_area);
	~ItemDropArea();

	virtual void check() override;
	virtual void setProperty(nlohmann::json* p_jsonfile) override;

private:
	const int m_clientId;

	//point gain for each item
	int m_pointPerItem;

	//spawner
	ItemSpawnArea* m_spawnArea;
};