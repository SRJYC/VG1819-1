#include "ItemDropArea.h"
#include "CaptureItemController.h"

ItemDropArea::ItemDropArea(int p_id)
	:m_clientId(p_id)
{
}

ItemDropArea::~ItemDropArea()
{
}

void ItemDropArea::check()
{
	for (auto info : m_tileList)
	{
		if (info->hasUnit())//this tile has unit on it
		{
			//get unit
			kitten::K_GameObject* ugo = info->getUnit();
			unit::Unit* u = ugo->getComponent<unit::Unit>();

			//check if unit is ally
			if (m_clientId != u->m_clientId)
				continue;

			//check if unit has item
			if (!u->hasItem())
				continue;

			//get item
			kitten::K_GameObject* itemGO = u->getItem();

			//remove item
			u->removeItem();

			//drop item
			itemGO->getComponent<CaptureItemController>()->remove();

			//increase points
			GameModeManager::getInstance()->gainPoint(m_clientId, m_pointPerItem);
		}
	}
}

void ItemDropArea::setProperty(nlohmann::json * p_jsonfile)
{
	m_pointPerItem = p_jsonfile->operator[]("point_per_item");

	m_filePath = p_jsonfile->operator[]("tile");
}
