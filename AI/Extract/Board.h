#pragma once 
#include "board/BoardManager.h"


namespace AI {
	namespace Extract {
		struct Board {
			std::vector<std::vector<TileInfo*>> board;

			void setupBoard() {
				int xSize = BoardManager::getInstance()->getDimension().first, 
					zSize = BoardManager::getInstance()->getDimension().second;

				board.resize(xSize);
				for (int x = 0; x < xSize; x++) {
					board[x].resize(zSize);
					for (int z = 0; z < zSize; z++) {
						board[x][z] = BoardManager::getInstance()->getTile(x, z)->getComponent<TileInfo>();
					}
				}
			}		
		};


		//TODO if free, add influence map as Influence Board here. 
	}
}