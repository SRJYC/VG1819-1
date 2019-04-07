/*
@Rock 12.27
This class will be a component of tile object
It will contain a list of decoration game object to make this tile interesting
*/

#pragma once
#include "board/tile/landInfo/LandInformation.h"

#define MAX_DECO_NUM 3

class DecorationGenerator
{
public:
	static std::vector<kitten::K_GameObject*> generateDecoration(TileInfo* p_info);

private:
	static std::vector<kitten::K_GameObject*> generateGrassDecoration(int p_posX, int p_posY);
	static std::vector<kitten::K_GameObject*> generateForestDecoration();
	static std::vector<kitten::K_GameObject*> generateGardenDecoration();
	static std::vector<kitten::K_GameObject*> generateWaterDecoration(int p_posX, int p_posY);
	static std::vector<kitten::K_GameObject*> generateSandDecoration(int p_posX, int p_posY);
	static std::vector<kitten::K_GameObject*> generateSwampDecoration();
	static std::vector<kitten::K_GameObject*> generateTownDecoration();

	static LandInformation::TileType getTileType(int p_x, int p_y);
	static std::vector<bool> isSurroundTileType(int p_x, int p_y, const LandInformation::TileType& p_type);
	static const std::vector<std::string> getEdgeFile(int p_x, int p_y,
		const LandInformation::TileType& p_selfType, const LandInformation::TileType& p_targetType);
	static const std::string typeToString(const LandInformation::TileType& p_type);

	int m_tileX, m_tileY;
};