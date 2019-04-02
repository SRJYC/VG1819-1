#include "UnitDataType.hpp"
#include "kibble/databank/databank.hpp"
#include <string>

unit::UnitData* getUnitDataFrom(nlohmann::json& p_jsonfile) {
	unit::UnitData* ud = new unit::UnitData();
	ud->m_texPath = "textures/unit/Default.tga";

	ud->m_name = p_jsonfile["name"].get<std::string>();
	ud->m_HP = p_jsonfile["hp"];
	ud->m_MV = p_jsonfile["mv"];
	ud->m_IN = p_jsonfile["in"];
	ud->m_Cost = p_jsonfile["cost"];
	if (p_jsonfile.find("size") != p_jsonfile.end()) {
		if (p_jsonfile["size"] == "cube")
			ud->m_size = unit::UnitSize::cube;
		else
			ud->m_size = unit::UnitSize::point;
	}

	if (p_jsonfile.find("tags") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["tags"].begin(); it != p_jsonfile["tags"].end(); ++it) {
			ud->m_tags.push_back(*it);
		}
	}

	if (p_jsonfile.find("ad") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["ad"].begin(); it != p_jsonfile["ad"].end(); ++it) {
			ud->m_ad.push_back(getAbilityDescriptionFrom(*it));
		}
	}

	if (p_jsonfile.find("ability_description") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["ability_description"].begin(); it != p_jsonfile["ability_description"].end(); ++it) {
			ud->m_ad.push_back(getAbilityDescriptionFrom(*it));
		}
	}

	if (p_jsonfile.find("sd") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["sd"].begin(); it != p_jsonfile["sd"].end(); ++it) {
			ud->m_sd.push_back(getStatusDescriptionFrom(*it));
		}
	}

	if (p_jsonfile.find("status_description") != p_jsonfile.end()) {
		for (nlohmann::json::iterator it = p_jsonfile["status_description"].begin(); it != p_jsonfile["status_description"].end(); ++it) {
			ud->m_sd.push_back(getStatusDescriptionFrom(*it));
		}
	}


	if (p_jsonfile.find("texpath") != p_jsonfile.end()) {
		ud->m_texPath = p_jsonfile["texpath"].get<std::string>();
	}

	if (p_jsonfile.find("spritename") != p_jsonfile.end()) {
		ud->m_spriteName = p_jsonfile["spritename"].get<std::string>();
	}


	//TODO portrait path (for commander)

	return ud;
}

std::vector<unit::UnitData*> getMultipleUnitDataFrom(nlohmann::json& p_jsonfile) {
	std::vector<unit::UnitData*> units;
	for (nlohmann::json::iterator it = p_jsonfile["units"].begin(); it != p_jsonfile["units"].end(); ++it) {
		units.push_back(getUnitDataFrom(*it));
	}

	return units;
}

unit::AbilityDescription* getAbilityDescriptionFrom(nlohmann::json& p_jsonfile) {
	if (p_jsonfile.is_string()) {
		return kibble::getCopyAbilityFromName(p_jsonfile);
	}
	else if (p_jsonfile.is_object()) {
		unit::AbilityDescription* ad = new unit::AbilityDescription();

		for (nlohmann::json::iterator it = p_jsonfile.begin(); it != p_jsonfile.end(); ++it) {
			if (it.key() == "basename") {
				unit::AbilityDescription* target = kibble::getAbilityFromName(it.value());
				if (target == nullptr) {
					kibble::flagAbilityForLateLoad(ad);
					ad->m_stringValue["basename"] = it.value();
					ad->m_stringValue.emplace("name", it.value());
				}
				else {
					ad->m_intValue.insert(target->m_intValue.begin(), target->m_intValue.end());
					ad->m_stringValue.insert(target->m_stringValue.begin(), target->m_stringValue.end());
				}
			}
			else if (it.key() == "filename") {
				unit::AbilityDescription* target = getAbilityDescriptionFrom(jsonIn(it.value()));
				ad->m_intValue.insert(target->m_intValue.begin(), target->m_intValue.end());
				ad->m_stringValue.insert(target->m_stringValue.begin(), target->m_stringValue.end());
				delete target;
			}
			else if (it->is_string()) {
				ad->m_stringValue[it.key()] = it.value();
			}
			else if (it->is_number_integer()) {
				ad->m_intValue[it.key()] = it.value();
			}
		}
		return ad;
	}
}

unit::StatusDescription* getStatusDescriptionFrom(nlohmann::json& p_jsonfile) {
	unit::StatusDescription* sd = new unit::StatusDescription();
	for (nlohmann::json::iterator it = p_jsonfile.begin(); it != p_jsonfile.end(); ++it) {
		if (it->is_string()) {
			sd->m_stringValue[it.key()] = it.value();
		}
		else if (it->is_number_integer()) {
			sd->m_intValue[it.key()] = it.value();
		}
	}
	return sd;
}