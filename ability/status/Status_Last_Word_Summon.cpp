#include "ability/status/Status.h"
#include "ability/AbilityMacro.h"
#include "unit/UnitCommon.h"
#include "unit/UnitSpawn.h"
#include "unit/unitComponent/UnitMove.h"
#include "networking/ClientGame.h"

ability::Status_Last_Word_Summon::Status_Last_Word_Summon()
{
	m_Id = STATUS_LAST_WORD_SUMMON;
	addTimePoint(TimePointEvent::Unit_Destroy);
}

int ability::Status_Last_Word_Summon::effect(const TimePointEvent::TPEventType& p_type, ability::TimePointEvent* p_event)
{
	if (p_type == TimePointEvent::Unit_Destroy)
	{
		//get id
		int id = m_intValue[KIBBLE_ID];

		//get lv
		int lv = m_intValue[UNIT_LV];


		//spawn unit
		kitten::K_GameObject* u = unit::UnitSpawn::getInstance()->spawnUnitObject(id, m_intValue[CLIENT_ID]);
		unit::Unit* unit = u->getComponent<unit::Unit>();

		//set tile
		kitten::K_GameObject* tile = m_unit->getTile();
		u->getComponent<unit::UnitMove>()->setTile(tile);

		//lv up
		for (int i = 1; i < lv; i++)
		{
			unit->levelup();
		}
	}
	return 0;
}

void ability::Status_Last_Word_Summon::setUnitToSummon(int p_id, int p_lv)
{
	m_intValue[KIBBLE_ID] = p_id;
	m_intValue[UNIT_LV] = p_lv;
}
