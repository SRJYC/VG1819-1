// TileInfo
//
// Component attached to tile objects that holds information about the tile
//	like if it is highlighted, who highlighted it, the position, the owner
//
// @Ken

#include "TileInfo.h"
#include "kitten/QuadRenderable.h"
#include "kitten/K_GameObjectManager.h"
#include "board/tile/DecorationGenerator.h"
#include "board/BoardManager.h"
#include "gameMode/Capture/CaptureItemController.h"

TileInfo::TileInfo(int p_iPosX, int p_iPosY)
	:
	m_iPosX(p_iPosX),
	m_iPosY(p_iPosY),
	m_sOwnerId(-1),
	m_sHighlightedBy("NONE"),
	m_lastHighlightTexture(nullptr),
	m_unitGO(nullptr),
	m_landInfo(nullptr),
	m_itemGO(nullptr),
	m_toSetDecoration(true)
{
	//init highlight type
	for (int i = TileInfo::First; i < TileInfo::Count; ++i)
	{
		m_highlightType[static_cast<HighlightType>(i)] = false;
	}
}

TileInfo::~TileInfo()
{

}

void TileInfo::setType(LandInformation::TileType p_type)
{
	m_tileType = p_type;
	if (m_landInfo != nullptr)
		setLand();
}

void TileInfo::start()
{
	m_quadRenderable = m_attachedObject->getComponent<kitten::QuadRenderableRepeat>();
	setLand();

	m_edge = m_attachedObject->getComponent<kitten::QuadEdgeRenderable>();
	//m_edge->setEnabled(false);
}

void TileInfo::setLand()
{
	m_landInfo = LandInfoManager::getInstance()->getLand(m_tileType);

	if (m_tileType != LandInformation::TileType::Water_land)
	{
		m_quadRenderable->setTexture(m_landInfo->getTexturePath().c_str());
	}

	setDecoration();
}

const int TileInfo::getMVCost()
{
	if (m_landInfo == nullptr)
		return 1;

	return m_landInfo->getMVCost();
}

const std::string TileInfo::getDescription()
{
	return m_landInfo->getDescription();
}

void TileInfo::effect(ability::TimePointEvent::TPEventType p_tp, unit::Unit * p_u)
{
	if (m_landInfo == nullptr)
		return;

	switch (p_tp)
	{
	case ability::TimePointEvent::Turn_Start :
		m_landInfo->effectOnStart(p_u,this);
		break;
	case ability::TimePointEvent::Turn_End:
		m_landInfo->effectOnStay(p_u, this);
		break;
	case ability::TimePointEvent::New_Tile:
		m_landInfo->effectOnPass(p_u, this);
		break;
	case ability::TimePointEvent::Leave_Tile:
		m_landInfo->effectOnPass(p_u, this);
		break;
	}
}

void TileInfo::changeHighlightTexture(puppy::Texture* p_tex)
{
	if (m_lastHighlightTexture != p_tex)
	{
		if (m_tileType != LandInformation::TileType::Water_land)
		{
			if (p_tex != nullptr)
			{
				//Add new blending
				m_quadRenderable->addTexture(p_tex, 1.0f);
			}

			if (m_lastHighlightTexture != nullptr)
			{
				//Remove blending
				m_quadRenderable->removeTexture(m_lastHighlightTexture);
			}
		}
		
		m_lastHighlightTexture = p_tex;
	}
}

const bool TileInfo::isHighlighted(HighlightType p_type)
{
	return m_highlightType[p_type];
}

void TileInfo::setHighlighted(HighlightType p_type, bool p_bool)
{
	m_highlightType[p_type] = p_bool;
}

TileInfo::HighlightType TileInfo::getHighlightType()
{
	for (int i = TileInfo::First; i < TileInfo::Count; ++i)
	{
		HighlightType p = static_cast<HighlightType>(i);
		if(m_highlightType[p])
			return p;
	}
	return None;
}

const int TileInfo::getPosX()
{
	return m_iPosX;
}

void TileInfo::setPosX(int p_int)
{
	m_iPosX = p_int;
}

const int TileInfo::getPosY()
{
	return m_iPosY;
}

void TileInfo::setPosY(int p_int)
{
	m_iPosY = p_int;
}

std::pair<int, int> TileInfo::getPos()
{
	return std::pair<int, int>(m_iPosX,m_iPosY);
}

void TileInfo::setPos(int p_x, int p_z)
{
	m_iPosX = p_x;
	m_iPosY = p_z;
}

void TileInfo::setPos(std::pair<int, int> p_pos)
{
	m_iPosX = p_pos.first;
	m_iPosY = p_pos.second;
}

const bool TileInfo::hasUnit()
{
	return m_unitGO != nullptr;
}

void TileInfo::setUnit(kitten::K_GameObject * p_u)
{
	m_unitGO = p_u;
}

void TileInfo::removeUnit()
{
	m_unitGO = nullptr;
}

kitten::K_GameObject * TileInfo::getUnit()
{
	return m_unitGO;
}

const int TileInfo::getOwnerId()
{
	return m_sOwnerId;
}

void TileInfo::setOwnerId(const int p_sId)
{
	m_sOwnerId = p_sId;

	if (m_sOwnerId)
	{
		m_edge->setTexture(P1_EDGE_TEXTURE);
	}
	else
	{
		m_edge->setTexture(P2_EDGE_TEXTURE);
	}

	//m_edge->setEnabled(true);
}

const std::string& TileInfo::getHighlightedBy()
{
	return m_sHighlightedBy;
}

void TileInfo::setHighlightedBy(const std::string& p_sId)
{
	m_sHighlightedBy = p_sId;
}

const bool TileInfo::isDemonicPresence()
{
	return (m_DemonicPresence || m_SecondaryDP);//this is DP or adjcent to DP
}

void TileInfo::setDemonicPresence(bool p_dp)
{
	if (m_DemonicPresence != p_dp)
	{
		m_DemonicPresence = p_dp;

		//dp changed, trigger new tile event
		triggerNewTileEvent();

		//ask adjcent tiles to check if this change matters
		if (m_adjTileList.size() == 0)//no tile in the list
			getAdjTile();

		for (auto it : m_adjTileList)
		{
			it->getComponent<TileInfo>()->checkSecondaryDP();
		}
	}
}

bool TileInfo::getDemonicPresence() const
{
	return m_DemonicPresence;
}

void TileInfo::checkSecondaryDP()
{
	if (m_adjTileList.size() == 0)//no tile in the list
		getAdjTile();

	bool dp = false;
	for (auto it : m_adjTileList)
	{
		TileInfo* info = it->getComponent<TileInfo>();
		if (info->getDemonicPresence())//adjcent tile is DP
		{
			dp = true;
			break;
		}
	}

	//change secondary dp
	if (dp != m_SecondaryDP)
	{
		m_SecondaryDP = dp;

		if (!m_DemonicPresence)
		{
			//this isn't DP, so secondary dp change matters
			//trigger new tile event
			triggerNewTileEvent();
		}
	}
}


void TileInfo::triggerNewTileEvent()
{
	if (!hasUnit())//do nothing if no unit on the tile
		return;

	ability::TimePointEvent* e = new ability::TimePointEvent(ability::TimePointEvent::New_Tile);
	e->putGameObject("tile", &getGameObject());//put this tile obj
	unit::Unit* u = m_unitGO->getComponent<unit::Unit>();
	u->triggerTP(ability::TimePointEvent::New_Tile, e);

}

bool TileInfo::hasItem() const
{
	return m_itemGO != nullptr;
}

void TileInfo::addItem(kitten::K_GameObject * p_item)
{
	//set reference
	m_itemGO = p_item;
}

void TileInfo::removeItem()
{
	m_itemGO = nullptr;
}

kitten::K_GameObject * TileInfo::getItem() const
{
	return m_itemGO;
}

void TileInfo::changeDecoration(const std::vector<kitten::K_GameObject*>& p_list)
{
	if (m_decorationList.size() > 0)
		removeDecoration();

	m_decorationList = p_list;

	kitten::Transform* tr = &m_attachedObject->getTransform();
	for (auto it : m_decorationList)
	{
		it->getTransform().setIgnoreParent(false);
		it->getTransform().setParent(tr);
	}

	m_toSetDecoration = false;
}

void TileInfo::setDecoration()
{
	if (!m_toSetDecoration)
		return;

	if (m_decorationList.size() > 0)
		removeDecoration();

	m_decorationList = DecorationGenerator::generateDecoration(this);

	kitten::Transform* tr = &m_attachedObject->getTransform();
	for (auto it : m_decorationList)
	{
		it->getTransform().setIgnoreParent(false);
		it->getTransform().setParent(tr);
	}
}

void TileInfo::removeDecoration()
{
	for (auto it : m_decorationList)
	{
		kitten::K_GameObjectManager::getInstance()->destroyGameObject(it);
	}
	m_decorationList.clear();
}

void TileInfo::getAdjTile()
{
	BoardManager* bm = BoardManager::getInstance();

	//check adjcent tiles
	kitten::K_GameObject* tile = bm->getTile(m_iPosX + 1, m_iPosY);
	if (tile != nullptr)
		m_adjTileList.push_back(tile);

	tile = bm->getTile(m_iPosX - 1, m_iPosY);
	if (tile != nullptr)
		m_adjTileList.push_back(tile);

	tile = bm->getTile(m_iPosX, m_iPosY + 1);
	if (tile != nullptr)
		m_adjTileList.push_back(tile);

	tile = bm->getTile(m_iPosX, m_iPosY - 1);
	if (tile != nullptr)
		m_adjTileList.push_back(tile);
}
