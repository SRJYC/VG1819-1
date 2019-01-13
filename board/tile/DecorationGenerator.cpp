#include "DecorationGenerator.h"
#include "kitten/K_GameObjectManager.h"
#include "board/BoardManager.h"
#include "TileInfo.h"

std::vector<kitten::K_GameObject*> DecorationGenerator::generateDecoration(TileInfo* p_info)
{
	LandInformation::TileType type = p_info->getType();

	int x = p_info->getPosX();
	int y = p_info->getPosY();

	switch (type)
	{
	case LandInformation::Grass_land:
		return generateGrassDecoration();
	case LandInformation::Forest_land:
		return generateForestDecoration();
	case LandInformation::Garden_land:
		return generateGardenDecoration();
	case LandInformation::Swamp_land:
		return generateSwampDecoration();
	case LandInformation::Water_land:
		return generateWaterDecoration(x, y);
	case LandInformation::Sand_land:
		return generateSandDecoration(x, y);
	}
	return std::vector<kitten::K_GameObject*>();
}


std::vector<kitten::K_GameObject*> DecorationGenerator::generateGrassDecoration()
{
	std::vector<kitten::K_GameObject*> list;
	kitten::K_GameObject* k;
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();

	/*
	float chance = (float)rand() / RAND_MAX;
	if (chance < 0.4)
	{
		k = gm->createNewGameObject("TileDecoration/grass/grass.json");
		list.push_back(k);
	}*/
	/*
	int n = rand() % MAX_DECO_NUM;//number of decoration

	for (int i = 0; i < n; i++)
	{
		kitten::K_GameObject* k;

		float chance = (float) rand() / RAND_MAX;
		if (chance < 0.7)//70% chance it's grass
		{
			k = kitten::K_GameObjectManager::getInstance()->createNewGameObject("Decoration_grass.json");
		}
		else//30% chance it's stone
		{
			k = kitten::K_GameObjectManager::getInstance()->createNewGameObject("Decoration_stone.json");
		}
		
		list.push_back(k);
	}*/
	
	return list;
}

std::vector<kitten::K_GameObject*> DecorationGenerator::generateForestDecoration()
{
	std::vector<kitten::K_GameObject*> list;
	kitten::K_GameObject* k;
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();


	//tree
	k = gm->createNewGameObject("TileDecoration/forest/tree.json");
	list.push_back(k);


	return list;
}

std::vector<kitten::K_GameObject*> DecorationGenerator::generateGardenDecoration()
{
	std::vector<kitten::K_GameObject*> list;
	kitten::K_GameObject* k;
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();

	//fence
	k = gm->createNewGameObject("TileDecoration/garden/fence.json");
	list.push_back(k);

	//flower
	k = gm->createNewGameObject("TileDecoration/garden/flower.json");
	list.push_back(k);

	return list;
}

std::vector<kitten::K_GameObject*> DecorationGenerator::generateWaterDecoration(int p_posX, int p_posY)
{
	std::vector<kitten::K_GameObject*> list;
	kitten::K_GameObject* k;
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();

	//grass
	const std::vector<std::string> filelist = getEdgeFile(p_posX, p_posY, LandInformation::Water_land, LandInformation::Grass_land);
	for (int i = 0; i < filelist.size(); i++)
	{
		k = gm->createNewGameObject(filelist[i]);
		list.push_back(k);
	}

	//sand
	const std::vector<std::string> filelist2 = getEdgeFile(p_posX, p_posY, LandInformation::Water_land, LandInformation::Sand_land);
	for (int i = 0; i < filelist2.size(); i++)
	{
		k = gm->createNewGameObject(filelist2[i]);
		list.push_back(k);
	}

	return list;
}

std::vector<kitten::K_GameObject*> DecorationGenerator::generateSandDecoration(int p_posX, int p_posY)
{
	std::vector<kitten::K_GameObject*> list;
	kitten::K_GameObject* k;
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();

	const std::vector<std::string> filelist2 = getEdgeFile(p_posX, p_posY, LandInformation::Sand_land, LandInformation::Grass_land);
	for (int i = 0; i < filelist2.size(); i++)
	{
		k = gm->createNewGameObject(filelist2[i]);
		list.push_back(k);
	}

	return list;
}

std::vector<kitten::K_GameObject*> DecorationGenerator::generateSwampDecoration()
{
	std::vector<kitten::K_GameObject*> list;
	kitten::K_GameObject* k;
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();

	//k = gm->createNewGameObject("TileDecoration/swamp/swamp_tree.json");
	//list.push_back(k);

	k = gm->createNewGameObject("TileDecoration/swamp/skull.json");
	list.push_back(k);

	return list;
}

LandInformation::TileType DecorationGenerator::getTileType(int p_x, int p_y)
{
	std::pair<int, int> d = BoardManager::getInstance()->getDimension();

	if (p_x >= 0 && p_x < d.first && p_y >= 0 && p_y < d.second)//on the board
	{
		kitten::K_GameObject* t = BoardManager::getInstance()->getTile(p_x, p_y);
		return t->getComponent<TileInfo>()->getType();
	}

	return LandInformation::Unknown;
}

std::vector<bool> DecorationGenerator::isSurroundTileType(int p_x, int p_y, const LandInformation::TileType & p_type)
{
	//get all tile type
	std::vector<LandInformation::TileType> typelist;
	typelist.push_back(getTileType(p_x + 1, p_y + 1));
	typelist.push_back(getTileType(p_x, p_y + 1));
	typelist.push_back(getTileType(p_x - 1, p_y + 1));
	typelist.push_back(getTileType(p_x + 1, p_y));
	typelist.push_back(getTileType(p_x - 1, p_y));
	typelist.push_back(getTileType(p_x + 1, p_y - 1));
	typelist.push_back(getTileType(p_x, p_y - 1));
	typelist.push_back(getTileType(p_x - 1, p_y - 1));

	std::vector<bool> list;
	if (p_type == LandInformation::Grass_land)
	{
		for (int i = 0; i < 8; i++)
		{
			bool b = typelist[i] == LandInformation::Grass_land ||
				typelist[i] == LandInformation::Garden_land ||
				typelist[i] == LandInformation::Forest_land;
			list.push_back(b);
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			bool b = typelist[i] == p_type;
			list.push_back(b);
		}
	}

	return list;
}

const std::vector<std::string> DecorationGenerator::getEdgeFile(int p_x, int p_y, const LandInformation::TileType & p_selfType, const LandInformation::TileType & p_targetType)
{
	std::vector<bool> sameTypeTile = isSurroundTileType(p_x, p_y, p_selfType);
	std::vector<bool> targetTypeTile = isSurroundTileType(p_x, p_y, p_targetType);

	std::string selfType = typeToString(p_selfType);
	std::string targetType = typeToString(p_targetType);
	std::string edge = targetType + "_" + selfType + "_edge";
	std::string path = "TileDecoration/" + edge + "/" + edge + "_";
	std::string end = ".json";

	std::vector<std::string> fileList;
	std::string file;
	//left
	if (targetTypeTile[3])
	{
		file = path + "4" + end;
		fileList.push_back(file);
	}

	//right
	if (targetTypeTile[4])
	{
		file = path + "5" + end;
		fileList.push_back(file);
	}

	//top
	if (targetTypeTile[1])
	{
		file = path + "2" + end;
		fileList.push_back(file);

		//left
		if (targetTypeTile[3])
		{
			//left top corner is normal
			file = path + "1" + end;
			fileList.push_back(file);
		}
		else
		{
			//left top corner is extends from top
			file = path + "1_ext2" + end;
			fileList.push_back(file);
		}

		//right
		if (targetTypeTile[4])
		{
			//right top corner is normal
			file = path + "3" + end;
			fileList.push_back(file);
		}
		else
		{
			//right top corner is extends from top
			file = path + "3_ext2" + end;
			fileList.push_back(file);
		}
	}
	else
	{
		//left
		if (targetTypeTile[3])
		{
			//left top corner is extends from left
			file = path + "1_ext4" + end;
			fileList.push_back(file);
		}
		else if (targetTypeTile[0] && sameTypeTile[1] && sameTypeTile[3])
		{
			//left top corner is a turn
			file = path + "1_turn" + end;
			fileList.push_back(file);
		}

		//right
		if (targetTypeTile[4])
		{
			//right top corner is extends from right
			file = path + "3_ext5" + end;
			fileList.push_back(file);
		}
		else if (targetTypeTile[2] && sameTypeTile[1] && sameTypeTile[4])
		{
			//right top corner is a turn
			file = path + "3_turn" + end;
			fileList.push_back(file);
		}
	}

	
	//bot
	if (targetTypeTile[6])
	{
		file = path + "7" + end;
		fileList.push_back(file);

		//left
		if (targetTypeTile[3])
		{
			//left bot corner is normal
			file = path + "6" + end;
			fileList.push_back(file);
		}
		else
		{
			//left bot corner is extends from bot
			file = path + "6_ext7" + end;
			fileList.push_back(file);
		}

		//right
		if (targetTypeTile[4])
		{
			//right bot corner is normal
			file = path + "8" + end;
			fileList.push_back(file);
		}
		else
		{
			//right bot corner is extends from bot
			file = path + "8_ext7" + end;
			fileList.push_back(file);
		}
	}
	else
	{
		//left
		if (targetTypeTile[3])
		{
			//left bot corner is extends from left
			file = path + "6_ext4" + end;
			fileList.push_back(file);
		}
		else if (targetTypeTile[5] && sameTypeTile[6] && sameTypeTile[3])
		{
			//left bot corner is a turn
			file = path + "6_turn" + end;
			fileList.push_back(file);
		}

		//right
		if (targetTypeTile[4])
		{
			//right bot corner is extends from right
			file = path + "8_ext5" + end;
			fileList.push_back(file);
		}
		else if (targetTypeTile[7] && sameTypeTile[6] && sameTypeTile[4])
		{
			//right bot corner is a turn
			file = path + "8_turn" + end;
			fileList.push_back(file);
		}
	}

	return fileList;
}

const std::string DecorationGenerator::typeToString(const LandInformation::TileType & p_type)
{
	switch (p_type)
	{
	case LandInformation::Grass_land:
	case LandInformation::Forest_land:
	case LandInformation::Garden_land:
		return "grass";
	case LandInformation::Swamp_land:
		return "swamp";
	case LandInformation::Water_land:
		return "water";
	case LandInformation::Sand_land:
		return "sand";
	default:
		break;
	}
	return "none";
}
