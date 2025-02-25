#include "board/tile/landInfo/LandInformation.h"
#include "ability/node/AbilityNodeManager.h"

void SwampLand::effectOnStay(unit::Unit* p_unit, TileInfo* p_tInfo)
{
	//damage unit by 1

	ability::AbilityNode* node1 = ability::AbilityNodeManager::getInstance()->findNode(ability::ChangeAttribute);

	//change hp
	node1->effect(p_unit, UNIT_HP, -1);
}