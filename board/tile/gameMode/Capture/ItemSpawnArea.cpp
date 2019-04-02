#include "ItemSpawnArea.h"
#include "kitten/K_GameObjectManager.h"
#include "CaptureItemController.h"
ItemSpawnArea::ItemSpawnArea()
{
}

ItemSpawnArea::~ItemSpawnArea()
{
}

void ItemSpawnArea::check()
{
	if (m_delay > 0)//there's delay before first spawn
	{
		m_delay--;
		return;
	}

	m_turnPass++;
	if (m_turnPass < m_turnWait)//there's wait between each spawn
	{
		return;
	}
	m_turnPass = 0;//reset turn pass

	//spawn item
	//check empty tile
	std::vector<TileInfo*> emptyList;
	for (auto info : m_tileList)
	{
		if (!info->hasUnit() && !info->hasItem())//this tile is empty
		{
			emptyList.push_back(info);
		}
	}

	if (m_itemPath.length() == 0)
		return;

	//spawn item on random tile
	for (int i = 0; i < m_itemPerSpawn; i++)
	{
		if (emptyList.size() == 0)//no more empty tile
			break;

		//get random number
		int index = rand() % emptyList.size();

		//spawn item
		kitten::K_GameObject* item;
		if (m_inactiveItemList.size() == 0)//no inactive game object
		{
			//create one
			item = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_itemPath);

			//set spawner
			item->getComponent<CaptureItemController>()->setSpawner(this);
		}
		else
		{
			//get one item from inactive list
			item = m_inactiveItemList.back();

			//remove from list
			m_inactiveItemList.pop_back();

			//set enable
			item->setEnabled(true);
		}

		//put it into active item list
		m_activeItemList.push_back(item);

		//put it on tile
		item->getComponent<CaptureItemController>()->setParent(emptyList[index]);

		//that tile is no longer empty, remove from list
		emptyList.erase(emptyList.begin()+index);
	}

}

void ItemSpawnArea::setProperty(nlohmann::json * p_jsonfile)
{
	m_itemPerSpawn = p_jsonfile->operator[]("item_per_spawn");

	m_turnWait = p_jsonfile->operator[]("turn_for_next_spawn");
	m_turnPass = m_turnWait;//the first spawn time using "delay" not "turn wait"

	m_delay = p_jsonfile->operator[]("delay");

	m_itemPath = p_jsonfile->operator[]("item_path");

	m_filePath = p_jsonfile->operator[]("tile");
}

void ItemSpawnArea::dropItem(kitten::K_GameObject * p_item)
{
	//get item from active list
	auto end = m_activeItemList.end();
	for (auto it = m_activeItemList.begin(); it != end; it++)
	{
		if (*it == p_item)
		{
			//remove it from active list
			m_activeItemList.erase(it);

			//disable it
			p_item->setEnabled(false);

			//add to inactive list
			m_inactiveItemList.push_back(p_item);

			//done
			break;
		}
	}
}
