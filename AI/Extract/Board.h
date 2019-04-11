#pragma once 
#include "board/BoardManager.h"


namespace AI {
	namespace Extract {
		struct Board {
			std::vector<std::vector<TileInfo*>> tile;
			std::vector<std::vector<unit::Unit*>> unit;

			void setupBoard() {
				int xSize = BoardManager::getInstance()->getDimension().first, 
					zSize = BoardManager::getInstance()->getDimension().second;

				tile.resize(xSize);
				unit.resize(xSize);
				for (int x = 0; x < xSize; x++) {
					tile[x].resize(zSize);
					unit[x].resize(zSize);
					for (int z = 0; z < zSize; z++) {
						tile[x][z] = BoardManager::getInstance()->getTile(x, z)->getComponent<TileInfo>();
						if (tile[x][z]->hasUnit())
							unit[x][z] = tile[x][z]->getUnit()->getComponent<unit::Unit>();
						else
							unit[x][z] = nullptr;
					}
				}
			}		

			void refreshBoards() {

				for (int x = 0; x < unit.size(); x++) {
					for (int z = 0; z < unit[x].size(); z++) {
						if (tile[x][z]->hasUnit())
							unit[x][z] = tile[x][z]->getUnit()->getComponent<unit::Unit>();
						else
							unit[x][z] = nullptr;
					}
				}
			}
		};


		//TODO if free, add influence map as Influence Board here. 
	}
}