#include "CaptureItemController.h"
#include "kitten/K_Time.h"
CaptureItemController::CaptureItemController()
	:m_spawnArea(nullptr)
{
}

CaptureItemController::~CaptureItemController()
{
}

bool CaptureItemController::hasUpdate() const
{
	return true;
}

void CaptureItemController::update()
{
	if (m_isUnit)//hold by unit
	{//change position with unit if unit moves
		glm::vec3 pos = m_holder->getTransform().getTranslation();
		if (pos != m_lastUnitPos)
		{
			m_lastUnitPos = pos; 
			getTransform().place(pos.x + m_unitOffset.x, pos.y + m_unitOffset.y, pos.z + m_unitOffset.z);
		}
	}
	/*
	else//hold by tile
	{
		TileInfo* info = m_holder->getComponent<TileInfo>();
		if (info->hasUnit())//there's unit on the tile
		{
			//get unit
			unit::Unit* u = info->getUnit()->getComponent<unit::Unit>();

			//check if it has item
			if (!u->hasItem())
			{//no item

			}
		}
	}*/
}

void CaptureItemController::setTileOffset(const glm::vec3 & p_pos)
{
	m_tileOffset = p_pos;
}

void CaptureItemController::setUnitOffset(const glm::vec3 & p_pos)
{
	m_unitOffset = p_pos;
}

void CaptureItemController::setParent(unit::Unit * p_info)
{
	//change holder
	m_holder = &p_info->getGameObject();

	//add this to target
	p_info->addItem(&getGameObject());

	m_isUnit = true;
}

void CaptureItemController::setParent(TileInfo * p_info)
{
	//change holder
	m_holder = &p_info->getGameObject();

	//add this to target
	p_info->addItem(&getGameObject());

	//set pos
	glm::vec3 pos = m_holder->getTransform().getTranslation();
	getTransform().place(pos.x + m_tileOffset.x, pos.y + m_tileOffset.y, pos.z + m_tileOffset.z);

	m_isUnit = false;
}

void CaptureItemController::setSpawner(ItemSpawnArea * p_spawner)
{
	m_spawnArea = p_spawner;
}

void CaptureItemController::remove()
{
	if (m_spawnArea != nullptr)
	{
		m_spawnArea->dropItem(&this->getGameObject());
	}
}

