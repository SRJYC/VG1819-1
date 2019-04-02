// BoardCreator
//
// Originally only created the tile objects that make up the board @Callum
// Multipurposed to handle highlighting and unhighlighting of tiles @Ken
// *Should rename component to something that better represents this.
// TODO: Either verify tiles to be highlighted are valid here or ensure it is
//	done before the tile list is sent here. E.g. A commander cannot highlight
//	tiles owned by the other commander

#include "BoardCreator.h"
#include "kitten\K_GameObjectManager.h"
#include "kitten\K_ComponentManager.h"

#include "kitten\QuadRenderable.h"
#include "_Project/UseAbilityWhenClicked.h"
//clickable
#include "board/clickable/ManipulateTileOnClick.h"
#include "board/clickable/PrintWhenClicked.h"
#include "board/clickable/SendSelfOnClick.h"
//tile
#include "board/tile/TileInfo.h"
//ambients
#include "_Project\AmbientSystemController.h"

#include "board/BoardManager.h"
#include "kibble/map/MapReader.h"
#include "board/tile/gameMode/GameModeManager.h"


BoardCreator::BoardCreator()
{

}

BoardCreator::~BoardCreator()
{

}

void BoardCreator::createBoard(int p_id)
{
	//tile info object
	if (m_enableTileInfoDisplay)
	{
		kitten::K_GameObject* tileText = kitten::K_GameObjectManager::getInstance()->createNewGameObject("debug_textbox.txt");
		m_tileInfoDisplay = static_cast<puppy::TextBox*>(tileText->getComponent<puppy::TextBox>());
		m_tileInfoDisplay->setText("Tile Info Debug");
		m_tileInfoDisplay->setEnabled(false);
		tileText->getTransform().move2D(800, 580);
	}

	//create place holders
	//create tile object list
	std::vector<kitten::K_GameObject*> list;

	//dimension and map id
	int dimX, dimZ;
	int mapId = p_id;

	//spawn point list
	kitten::Event::TileList m_spawnPointList(2);

	//get land list and dimension for random map
	//pair = LandInformation::TileType , GameModeComponent::TileType 
	std::vector<std::pair<int,int>> landList = kibble::MapReader::getInstance()->getMap(&dimX, &dimZ, &mapId);

	//board game object
	kitten::K_GameObject* borad = kitten::K_GameObjectManager::getInstance()->createNewGameObject();

	//create tile objects
	int i = 0;
	for (int x = 0; x < dimX; x++)
	{
		for (int z = 0; z < dimZ; z++)
		{
			kitten::K_GameObject* tileGO;

			LandInformation::TileType landtype;
			GameModeComponent::TileType modetype;

			//get type
			landtype = static_cast<LandInformation::TileType>(landList[i].first);
			modetype = static_cast<GameModeComponent::TileType>(landList[i].second);

			//create tile
			tileGO = createTile(x, z, landtype);

			//increase counter
			i++;

			//add to listo lis
			list.push_back(tileGO);

			//change parent
			kitten::Transform& transform = tileGO->getTransform();
			transform.setParent(&borad->getTransform());
			transform.setIgnoreParent(true);

			//register special type tile to game mode manager
			if (modetype != GameModeComponent::Unknow)//it's special
			{
				if (modetype == GameModeComponent::SpawnPoint0)//spawn point
					m_spawnPointList[0] = std::make_pair(x, z);
				else if (modetype == GameModeComponent::SpawnPoint1)//spawn point
					m_spawnPointList[1] = std::make_pair(x, z);
				else
					GameModeManager::getInstance()->registerTile(tileGO,modetype);
			}
		}
	}

	assert(m_spawnPointList.size() == 2);//number of spawn point must be 2

	BoardManager* bm = BoardManager::getInstance();

	// PowerTracker component attached to Board GO
	kitten::K_Component* powerTracker = kitten::K_ComponentManager::getInstance()->createComponent("PowerTracker");
	borad->addComponent(powerTracker);
	bm->setPowerTracker(static_cast<PowerTracker*>(powerTracker));

	//set board property
	bm->setSpawnPoint(m_spawnPointList);//spawn point
	bm->setTileList(list);//tile object
	bm->setDimension(dimX, dimZ);//dimension
	bm->setMapID(mapId);//map id
	bm->setBoardGameObject(borad);//board

	kitten::Event* e = new kitten::Event(kitten::Event::Board_Loaded);
	e->putInt(MAP_ID_KEY, mapId);
	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Board_Loaded, e);
}
/*
void BoardCreator::start()
{
	//create tile
	std::vector<kitten::K_GameObject*> list;

	int mapId = -1;

	//get land list and dimension for random map
	std::vector<int> landList = kibble::MapReader::getInstance()->getMap(&m_x, &m_z, &mapId);

	/*move to kibble::MapReader
	bool hasmap = false;
	std::ifstream file("data/map");
	if (file.is_open())
	{//read dimension
		file >> m_x;
		file >> m_z;
		hasmap = true;
		for (int i = 0; i < m_x * m_z; ++i)
		{
			int l;
			file >> l;
			landList.push_back(l);
		}
	}

	if (m_enableTileInfoDisplay)
	{
		kitten::K_GameObject* tileText = kitten::K_GameObjectManager::getInstance()->createNewGameObject("debug_textbox.txt");
		m_tileInfoDisplay = static_cast<puppy::TextBox*>(tileText->getComponent<puppy::TextBox>());
		m_tileInfoDisplay->setText("Tile Info Debug");
		m_tileInfoDisplay->setEnabled(false);
		tileText->getTransform().move2D(800, 580);
	}

	int i = 0;
	for (int x = 0; x < m_x; x++)
	{
		for (int z = 0; z < m_z; z++)
		{
			kitten::K_GameObject* tileGO;
			LandInformation::TileType landtype;
			if (landList[i] >= 0)//normal land
			{
				landtype = static_cast<LandInformation::TileType>(landList[i]);
			}
			else//spawn point
			{
				landtype = LandInformation::Grass_land;//commander spawn at normal land
				m_spawnPointList.push_back(std::make_pair(x, z));
			}
			tileGO = createTile(x, z, landtype);
			i++;

			list.push_back(tileGO);

			kitten::Transform& transform = tileGO->getTransform();
			transform.setParent(&m_attachedObject->getTransform());
			transform.setIgnoreParent(true);
		}
	}

	assert(m_spawnPointList.size() == 2);//number of spawn point must be 2
	//set spawn point
	BoardManager::getInstance()->setSpawnPoint(m_spawnPointList);

	//pass tile list and dimension to board manager
	BoardManager::getInstance()->setTileList(list);
	BoardManager::getInstance()->setDimension(m_x,m_z);

	// PowerTracker component attached to Board GO
	kitten::K_Component* powerTracker = kitten::K_ComponentManager::getInstance()->createComponent("PowerTracker");
	m_attachedObject->addComponent(powerTracker);
	BoardManager::getInstance()->setPowerTracker(static_cast<PowerTracker*>(powerTracker));

	kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Board_Loaded, nullptr);

	//delete this
	kitten::K_ComponentManager::getInstance()->destroyComponent(this);
}

void BoardCreator::setDimension(int x, int z)
{
	m_x = x;
	m_z = z;
}*/


kitten::K_GameObject * BoardCreator::createTile(int x, int z, LandInformation::TileType p_type)
{
	kitten::K_GameObjectManager* gameObjMan = kitten::K_GameObjectManager::getInstance();
	kitten::K_ComponentManager* compMan = kitten::K_ComponentManager::getInstance();


	kitten::K_GameObject* tileGO;

	if (p_type != LandInformation::TileType::Water_land)
	{
		tileGO = gameObjMan->createNewGameObject("tileobj.txt");
	}
	else
	{
		tileGO = gameObjMan->createNewGameObject("waterTileObj.txt");
	}

	/*this is test of clickable, not useful anymore
	PrintWhenClicked* printWhenClick = static_cast<PrintWhenClicked*>(compMan->createComponent("PrintWhenClicked"));
	printWhenClick->setMessage("grassy tile: " + std::to_string(x) + ", " + std::to_string(z));
	tileGO->addComponent(printWhenClick);*/

	//ManipulateTileOnClick* manipTileOnClick = static_cast<ManipulateTileOnClick*>(compMan->createComponent("ManipulateTileOnClick"));
	//tileGO->addComponent(manipTileOnClick);

	SendSelfOnClick* sendSelfOnClick = static_cast<SendSelfOnClick*>(compMan->createComponent("SendSelfOnClick"));
	sendSelfOnClick->setTileInfoDisplay(m_tileInfoDisplay);
	tileGO->addComponent(sendSelfOnClick);

	TileInfo* tileInfo = static_cast<TileInfo*>(compMan->createComponent("TileInfo"));
	tileInfo->setPos(x, z);//set position
	tileInfo->setType(p_type);
	tileGO->addComponent(tileInfo);

	kitten::K_Component* clickBox = compMan->createComponent("ClickableBox");
	tileGO->addComponent(clickBox);

	kitten::Transform& transform = tileGO->getTransform();
	transform.move(x, -1, z);

	//ambient particles
	switch (p_type)
	{
	case LandInformation::TileType::Swamp_land:
	{
		AmbientSystemController::getInstance()->registerAmbientEffect("Universal Pfx/SwampGasUPFX.json", tileGO->getTransform().getTranslation(), 10, 15);
		break;
	}
	case LandInformation::TileType::Sand_land:
	{
		AmbientSystemController::getInstance()->registerAmbientEffect("Universal Pfx/SandDustUPFX.json", tileGO->getTransform().getTranslation(), 30, 60);
		break;
	}
	case LandInformation::TileType::Water_land:
	{
		AmbientSystemController::getInstance()->registerAmbientEffect("Universal Pfx/WaterSparkleUPFX.json", tileGO->getTransform().getTranslation(), 1, 120);
		break;
	}
	}

	return tileGO;

}
