#pragma once
#include "kitten/K_Common.h"
#include "board/tile/TileInfo.h"
#include "unit/Unit.h"
#include "ItemSpawnArea.h"

class CaptureItemController : public kitten::K_Component
{
public:
	CaptureItemController();
	~CaptureItemController();

	virtual bool hasUpdate() const override;
	virtual void update() override;

	void setTileOffset(const glm::vec3& p_pos);
	void setUnitOffset(const glm::vec3& p_pos);

	//set parent, may be tile or unit
	//they can hold item
	void setParent(unit::Unit* p_info);
	void setParent(TileInfo* p_info);

	void setSpawner(ItemSpawnArea* p_spawner);
	void remove();

private:
	//translation offset
	glm::vec3 m_tileOffset;
	glm::vec3 m_unitOffset;
	glm::vec3 m_lastUnitPos;

	//the obj which holds the item
	kitten::K_GameObject* m_holder;

	bool m_isUnit;

	//spawner
	ItemSpawnArea* m_spawnArea;
};
