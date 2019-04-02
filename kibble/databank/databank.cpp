#include "databank.hpp"
#include "unit/UnitSpawn.h"
#include <map>
#include <unordered_set>
#include <kibble/kibble.hpp>
#include <kitten/K_GameObjectManager.h>
#include <ctime>

std::vector<kibble::UnitFileStruct> unitDataVector;
std::vector<int> addableToDeckUnitVect;
std::map<std::string, unit::AbilityDescription*> abilityDataMap;
std::map<std::string, std::vector<int>> abilityToUnitMap, tagToUnitMap;
std::unordered_set<unit::AbilityDescription*> lateLoadAbility;
std::vector<DeckData*> deckDataVector;

std::vector<std::vector<kitten::K_Component*>> unitSpecificComponentVector;

#define DECK_LIST "data/gamedecklist.txt"
#define UNIT_LIST "data/gameunitlist.txt"
#define COMMANDER "Commander"

// Basically the deconstructor
void kibble::destroyDatabank() {
	for (kibble::UnitFileStruct& unit : unitDataVector) {
		for (auto behavior : unit.data->m_AbilityBehavior) {
			delete behavior.second;
		}
		delete unit.data;
	}
	for (auto data : deckDataVector) {
		delete data;
	}
}

#include "kibble/kibble.hpp"
#include <fstream>
void kibble::setupDatabank() {
	unitDataVector.reserve(50);
	kibble::UnitDataParser* unitDataParser = kibble::getUnitDataParserInstance();

	std::ifstream input(UNIT_LIST);
	if (input.is_open()) {
		std::string unitFilename;
		while (input >> unitFilename) {
			UnitFileStruct target = unitDataParser->getUnit(unitFilename);

			if (std::find(target.data->m_tags.begin(), target.data->m_tags.end(), "Dummy") == target.data->m_tags.end()) {
				for (auto ability : target.data->m_ADMap) { // Set up Abilities
					std::string name = ability.first;
					abilityToUnitMap[name].push_back(unitDataVector.size()); // add unit to list of units that use the ability
					if (abilityDataMap.find(name) == abilityDataMap.end()) { // check to see if a previous ability with the same name doesn't exist
						// if it finds nothing, it checks if it's being late loaded
						if (lateLoadAbility.find(ability.second) != lateLoadAbility.end())
							abilityDataMap[name] = ability.second; // if its not being loaded late set it
					}
				}

				for (std::string tag : target.data->m_tags) { // Set up Tags
					tagToUnitMap[tag].push_back(unitDataVector.size());
				}

				if (!target.data->isCommander() &&
					std::find(target.data->m_tags.begin(), target.data->m_tags.end(), "token") == target.data->m_tags.end()
					)
					addableToDeckUnitVect.push_back(unitDataVector.size());
			}

			target.data->m_kibbleID = unitDataVector.size();
			// At the end push the unit into vector. 
			//This is left to the end so that previous calls to size return the actual index of this unit
			unitDataVector.push_back(target);
		}
		input.close();
	}
	else {
		throw std::exception("Something went wrong, can't open the Unit List file."); // check if filename is correct.
		return;
	}

	for (auto ability : lateLoadAbility) {// if an error is thrown back, most likely than not it's because it returned a nullptr. 
		unit::AbilityDescription* target = getAbilityFromName(ability->m_stringValue["basename"]);
		
		ability->m_intValue.insert(target->m_intValue.begin(), target->m_intValue.end());
		ability->m_stringValue.insert(target->m_stringValue.begin(), target->m_stringValue.end());
	}
	lateLoadAbility.clear();

	std::ifstream deckListFile(DECK_LIST);
	if (deckListFile.is_open()) {
		std::string deckFilename;
		while (deckListFile >> deckFilename) {
			deckDataVector.push_back(kibble::getDeckDataParserInstance()->getDeckData(deckFilename));
		}
		deckListFile.close();
	}
	else {
		throw std::exception("Something went wrong, can't open the deck List file."); // check if filename is correct.
		return;
	}
}


unit::Unit* kibble::getUnitFromId(const int& p_identifier) {
	if (p_identifier < 0) return unitDataVector[unit::UnitSpawn::King].data;
	return unitDataVector[p_identifier].data;
}

std::vector<unit::Unit*> kibble::getUnitsFromListOfIds(const std::vector<int>& p_identifiers) {
	std::vector<unit::Unit*> unitList; 
	for (int index : p_identifiers) {
		unitList.push_back(unitDataVector[index].data);
	}
	return unitList;
}


//returns null if none is found matching string
unit::AbilityDescription* kibble::getAbilityFromName(const std::string& p_name) {
	if (abilityDataMap.find(p_name) == abilityDataMap.end())
		return nullptr;
	else
		return abilityDataMap[p_name];
}

// returns an empty ability if none is found matching string it also internally adds it to late load list 
// otherwise it returns a copy of the ability to do whatever with. 
unit::AbilityDescription* kibble::getCopyAbilityFromName(const std::string& p_name) {
	if (abilityDataMap.find(p_name) == abilityDataMap.end()) {
		unit::AbilityDescription* ability = new unit::AbilityDescription();
		ability->m_stringValue["basename"] = p_name;
		ability->m_stringValue["name"] = p_name;
		lateLoadAbility.insert(ability);
		return ability;
	}
	else
		return new unit::AbilityDescription(*abilityDataMap[p_name]);
}
void kibble::flagAbilityForLateLoad(unit::AbilityDescription* p_ability) {
	lateLoadAbility.insert(p_ability);
}

const std::vector<int>&  kibble::getUnitIdsThatHaveAbilityOfName(const std::string& p_name) {
	return abilityToUnitMap[p_name];
}
const std::vector<int>&  kibble::getUnitIdsThatHaveTag(const std::string& p_tag) {
	return tagToUnitMap[p_tag];
}
const std::vector<int>&  kibble::getCommanderIds() {
	return tagToUnitMap[COMMANDER];
}
const std::vector<int>&  kibble::getNonCommanderIds() {
	return addableToDeckUnitVect;
}

int kibble::getDeckDataListCount() {
	return deckDataVector.size();
}
int kibble::getCommanderUnitCount() {
	return tagToUnitMap[COMMANDER].size();
}
int kibble::getNonCommanderUnitCount() {
	return addableToDeckUnitVect.size();
}

DeckData* kibble::getDeckDataFromId(const int& p_identifier) {
	return deckDataVector[p_identifier];
}

int kibble::addNewDeckData(DeckData* p_data) {
	p_data->filename = "Deck" + std::to_string(time(NULL));

	if (p_data->name == "") p_data->name = "Unnamed Deck " + std::to_string(time(NULL));
	kibble::getDeckDataParserInstance()->saveDeckData(p_data, p_data->filename);
	std::ofstream deckList(DECK_LIST, std::ofstream::app | std::ofstream::out);
	if (deckList.is_open()) {
		deckList << std::endl << p_data->filename ;
	}
	deckDataVector.push_back(p_data);
	return deckDataVector.size() - 1;
}
#include <cstdio>
#include <fstream>
void kibble::eraseDeckData(int p_deckId) {
	std::remove(("data/saved/" + deckDataVector[p_deckId]->filename).c_str());
	std::ifstream deckList(DECK_LIST);
	std::ofstream temp("temp.txt");

	if (deckList.is_open()) {
		std::string line;
		while (std::getline(deckList, line))
			if (line != deckDataVector[p_deckId]->filename)
				temp << line << std::endl;
	}
		
	temp.close();
	deckList.close();
	std::remove(DECK_LIST);
	std::rename("temp.txt", DECK_LIST);
	delete deckDataVector[p_deckId];
	deckDataVector.erase(deckDataVector.begin()+p_deckId);
}

void kibble::replaceDeckData(int p_deckIdSource, DeckData* p_deckTarget)
{
	(*deckDataVector[p_deckIdSource]) = (*p_deckTarget);
	kibble::getDeckDataParserInstance()->saveDeckData(p_deckTarget, p_deckTarget->filename);
}

#include "kitten/K_ComponentManager.h"
kitten::K_GameObject* kibble::attachCustomComponentsToGameObject(const int& p_identifier, kitten::K_GameObject* p_targetGameObject) {
	kibble::UnitFileStruct& targetUnit = unitDataVector[p_identifier];
	kitten::K_ComponentManager* componentManager = kitten::K_ComponentManager::getInstance();

	unit::Unit* unit = componentManager->createUnitComponent(targetUnit.unitJson);
	unit->m_kibbleID = p_identifier;
	p_targetGameObject->addComponent(unit);
	for (nlohmann::json component : targetUnit.components) {
		p_targetGameObject->addComponent(componentManager->createComponent(&component));
	}

	p_targetGameObject->getTransform().rotateAbsolute(glm::vec3(targetUnit.rotate[0], targetUnit.rotate[1], targetUnit.rotate[2]));
	p_targetGameObject->getTransform().scaleAbsolute(targetUnit.scale[0], targetUnit.scale[1], targetUnit.scale[2]);

	return p_targetGameObject;
}

kitten::K_GameObject* kibble::attachCustomComponentsToGameObject(const unit::Unit* p_unit, kitten::K_GameObject* p_targetGameObject) {
	kibble::UnitFileStruct& targetUnit = unitDataVector[p_unit->m_kibbleID];
	kitten::K_ComponentManager* componentManager = kitten::K_ComponentManager::getInstance();

	for (nlohmann::json component : targetUnit.components) {
		p_targetGameObject->addComponent(componentManager->createComponent(&component));
	}

	p_targetGameObject->getTransform().rotateAbsolute(glm::vec3(targetUnit.rotate[0], targetUnit.rotate[1], targetUnit.rotate[2]));
	p_targetGameObject->getTransform().scaleAbsolute(targetUnit.scale[0], targetUnit.scale[1], targetUnit.scale[2]);

	return p_targetGameObject;
}


bool kibble::checkIfComponentDriven(const int& p_identifier) {
	return !unitDataVector[p_identifier].components.empty();
}

unit::Unit* kibble::getUnitInstanceFromId(const int& p_identifier) {
	unit::Unit* unit = kitten::K_ComponentManager::getInstance()->createUnitComponent(unitDataVector[p_identifier].unitJson);
	unit->m_kibbleID = p_identifier;
	return unit;
}
