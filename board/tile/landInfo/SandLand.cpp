#include "board/tile/landInfo/LandInformation.h"
#include "ability/StatusManager.h"

void SandLand::effectOnStay(unit::Unit* p_unit, TileInfo* p_tInfo)
{
	//Unit will temporarily -1 IN

	ability::Status* status = ability::StatusManager::getInstance()->findStatus(STATUS_TEMP_CHANGE);
	status->changeName(LAND_STATUS_NAME);
	status->changeDescription("IN -1");
	status->addAttributeChange(UNIT_IN, -1);
	status->addCounter(UNIT_DURATION, 1);
	status->endEffectAt(ability::TimePointEvent::Turn_Start);

	status->attach(p_unit);
}