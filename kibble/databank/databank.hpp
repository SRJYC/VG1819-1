#pragma once
#include "unit/UnitCommon.h"
#include "DeckData.hpp"
#include <vector>
#include "kitten/K_GameObject.h"

namespace kibble {
	unit::Unit* getUnitFromId(const int& p_identifier);
	std::vector<unit::Unit*> getUnitsFromListOfIds(const std::vector<int>& p_identifier);

	//returns null if none is found matching string
	unit::AbilityDescription* getAbilityFromName(const std::string& p_name);

	const std::vector<int>& getUnitIdsThatHaveAbilityOfName(const std::string& p_name);
	const std::vector<int>& getUnitIdsThatHaveTag(const std::string& p_tag);
	const std::vector<int>& getCommanderIds();
	const std::vector<int>& getNonCommanderIds();

	DeckData* getDeckDataFromId(const int& p_identifier);
	int getDeckDataListCount();

	DeckData* getAIDeckDataFromId(const int& p_identifier);
	int getAIDeckDataListCount();

	int getCommanderUnitCount();
	int getNonCommanderUnitCount();

	int addNewDeckData(DeckData*);
	void eraseDeckData(int p_deckId);
	void replaceDeckData(int p_deckIdSource, DeckData* p_deckTarget);

	kitten::K_GameObject* attachCustomComponentsToGameObject(const int& p_identifier, kitten::K_GameObject* p_targetGameObject);
	kitten::K_GameObject* attachCustomComponentsToGameObject(const unit::Unit* p_unit, kitten::K_GameObject* p_targetGameObject);
	unit::Unit* getUnitInstanceFromId(const int& p_identifier);
	bool checkIfComponentDriven(const int& p_identifier);



	// ===----- For internal Use Only after this comment ---------------------------------------------------

	void setupDatabank();
	void destroyDatabank();
	// returns an empty ability if none is found matching string, adds it internally to a late load list. 
	// To be used only within kibble
	unit::AbilityDescription* getCopyAbilityFromName(const std::string& p_name);
	void flagAbilityForLateLoad(unit::AbilityDescription*);
}