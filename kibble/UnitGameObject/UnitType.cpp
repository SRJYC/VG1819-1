#include "UnitType.h"
#include "ability/StatusManager.h"
#include "AI/Extract/Behavior.h"

unit::Unit * getUnitFrom(nlohmann::json & p_jsonfile)
{
	unit::Unit* unit = new unit::Unit();

	//TO DO: an ID system instead of random words
	unit->m_ID = "testUnit01";//hard coded for now

	unit->m_name = p_jsonfile["name"].get<std::string>();
	int hp = p_jsonfile["hp"];
	int in = p_jsonfile["in"];
	int mv = p_jsonfile["mv"];
	int cost = p_jsonfile["cost"];

	if (p_jsonfile.find("size") != p_jsonfile.end()) {
		if (p_jsonfile["size"] == "cube")
			unit->m_size = unit::UnitSize::cube;
		else
			unit->m_size = unit::UnitSize::point;
	}
	else { unit->m_size = unit::UnitSize::point; }

	unit->m_attributes[UNIT_HP] = hp;
	unit->m_attributes[UNIT_MAX_HP] = hp;
	unit->m_attributes[UNIT_IN] = in;
	unit->m_attributes[UNIT_BASE_IN] = in;
	unit->m_attributes[UNIT_MV] = mv;
	unit->m_attributes[UNIT_BASE_MV] = mv;
	unit->m_attributes[UNIT_COST] = cost;
	unit->m_attributes[UNIT_BASE_COST] = cost;
	//set lv to 1
	unit->m_attributes[UNIT_LV] = 1;

	//commander
	if (p_jsonfile.find("commander") != p_jsonfile.end()) {
		//change lv to 0 since it doesn't apply to commander
		unit->m_attributes[UNIT_LV] = 0;

		unit->m_ID = "testCommander01";//should be change

		unit->addCommander(getCommanderFrom(p_jsonfile["commander"]));
	}

	//size
	if (p_jsonfile.find("size") != p_jsonfile.end()) {
		if (p_jsonfile["size"] == "cube")
			unit->m_size = unit::UnitSize::cube;
		else
			unit->m_size = unit::UnitSize::point;
	}

	//tags
	if (p_jsonfile.find("tags") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["tags"].begin(); it != p_jsonfile["tags"].end(); ++it) {
			unit->m_tags.push_back(*it);
		}
	}

	//ability descrption
	if (p_jsonfile.find("ability_description") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["ability_description"].begin(); it != p_jsonfile["ability_description"].end(); ++it) {
			unit::AbilityDescription * ad = getADFrom(*it);
			unit->m_ADList.push_back(ad);
			unit->m_ADMap[ad->m_stringValue[ABILITY_NAME]] = ad;
		}
	}

	//status
	if (p_jsonfile.find("status") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["status"].begin(); it != p_jsonfile["status"].end(); ++it) {
			ability::Status * ad = getStatusFrom(*it);
			ad->attach(unit, false);
		}
	}

	// Portrait Texture
	if (p_jsonfile.find("portrait_texture") != p_jsonfile.end()) {
		std::string texturePath = p_jsonfile["portrait_texture"].get<std::string>();
		unit->setPortraitTexturePath(texturePath);
	}

	// AI Behavior
	if (p_jsonfile.find("AI") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["AI"].begin(); it != p_jsonfile["AI"].end(); ++it) {
			unit->m_AbilityBehavior[it->at("ability")] = generateBehavior(it->at("behavior"));
		}
	}

	//client id for net working
	//doesn't belong to any client
	unit->m_clientId = -1;

	return unit;
}




unit::Commander * getCommanderFrom(nlohmann::json & p_jsonfile)
{
	unit::Commander * c = new unit::Commander();

	if (p_jsonfile.find("portrait_path") != p_jsonfile.end())
	{
		c->m_porPath = p_jsonfile["portrait_path"].get<std::string>();
	}

	return c;
}

unit::AbilityDescription * getADFrom(nlohmann::json & p_jsonfile)
{
	unit::AbilityDescription * ad = new unit::AbilityDescription();
	for (nlohmann::json::iterator it = p_jsonfile.begin(); it != p_jsonfile.end(); ++it) {
		if (it->is_string()) {
			ad->m_stringValue[it.key()] = it.value().get<std::string>();
		}
		else if (it->is_number_integer()) {
			ad->m_intValue[it.key()] = it.value();
		}
	}
	return ad;
}

ability::Status * getStatusFrom(nlohmann::json & p_jsonfile)
{
	std::string name = p_jsonfile["name"];

	//find a empty copy of the status
	ability::Status* s = ability::StatusManager::getInstance()->findStatus(name);

	for (nlohmann::json::iterator it = p_jsonfile.begin(); it != p_jsonfile.end(); ++it) {
		if (it->is_string()) {
			s->m_stringValue[it.key()] = it.value().get<std::string>();
		}
		else if (it->is_number_integer()) {
			s->m_intValue[it.key()] = it.value();
		}
	}
	return s;


	/*
	//get description
	if (p_jsonfile.find("description") != p_jsonfile.end())
	{
		s->changeDescription(p_jsonfile["description"].get<std::string>());
	}

	time point is fixed in each status
	//get trigger time point 
	if (p_jsonfile.find("time_point") != p_jsonfile.end())
	{
		int i = p_jsonfile["time_point"];
		s->addTimePoint(static_cast<ability::TimePointEvent::TPEventType>(i));
	}*/

	/*
	//effected thing
	if (p_jsonfile.find("effected") != p_jsonfile.end())
	{
		s->setEffectedAD(p_jsonfile["effected"].get<std::string>());
	}*/
	/*
	for (nlohmann::json::iterator it = p_jsonfile.begin(); it != p_jsonfile.end(); ++it) 
	{
		if (it->is_number_integer())
		{
			//for lv status
			if (it.key() == UNIT_LV)
			{
				s->changeName(LEVEL_UP);
				s->changeLV(p_jsonfile[UNIT_LV]);
			}
			else if (it.key() == UNIT_HP)//hp
			{
				int hp = it.value();
				s->addAttributeChange(UNIT_MAX_HP, hp);
				s->addAttributeChange(UNIT_HP, hp);
			}
			else if (it.key() == UNIT_IN)//in
			{
				int in = it.value();
				s->addAttributeChange(UNIT_IN, in);
				s->addAttributeChange(UNIT_BASE_IN, in);
			}
			else if (it.key() == UNIT_MV)//mv
			{
				int mv = it.value();
				s->addAttributeChange(UNIT_MV, mv);
				s->addAttributeChange(UNIT_BASE_MV, mv);
			}
			else//other int value, put in counter
			{
				s->addCounter(it.key(), it.value());
			}
		}
	}
	return s;*/
}
