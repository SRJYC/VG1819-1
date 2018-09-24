#include "GameObjectDataType.hpp"
#include "kibble/json/JSONComponentDataParser.hpp"
#include "kitten/K_GameObjectManager.h"
#include "kitten/K_ComponentManager.h"

kitten::K_GameObject* getGameObjectBy(nlohmann::json& p_jsonfile) {

	// going through both managers is important! 
	kitten::K_GameObjectManager* gameobjectmanager = kitten::K_GameObjectManager::getInstance();
	kitten::K_ComponentManager* componentmanager = kitten::K_ComponentManager::getInstance();
	kitten::K_GameObject* gameobject = gameobjectmanager->createNewGameObject();

	// add all attributes related to game object only here!
	if (p_jsonfile.find("translate") != p_jsonfile.end()) {
		gameobject->getTransform().place(p_jsonfile["translate"][0], p_jsonfile["translate"][1], p_jsonfile["translate"][2]);
	}
	
	if (p_jsonfile.find("rotate") != p_jsonfile.end()) {
		gameobject->getTransform().place(p_jsonfile["rotate"][0], p_jsonfile["rotate"][1], p_jsonfile["rotate"][2]);
	}
	
	if (p_jsonfile.find("scale") != p_jsonfile.end()) {
		gameobject->getTransform().place(p_jsonfile["scale"][0], p_jsonfile["scale"][1], p_jsonfile["scale"][2]);
	}

	if (p_jsonfile.find("components") != p_jsonfile.end()) {
		//assert(p_jsonfile["components"].is_array());
		for (nlohmann::json::iterator it = p_jsonfile["components"].begin(); it != p_jsonfile["components"].end(); ++it) {
			kibble::JSONComponentDataParser data = kibble::JSONComponentDataParser(&(*it));
			gameobject->addComponent(componentmanager->createComponent(&data));
		}
	}

	return gameobject;
}