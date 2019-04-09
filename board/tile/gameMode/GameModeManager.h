#pragma once
#include "kitten/K_Common.h"
#include <assert.h>
#include "kitten/event_system/EventManager.h"
#include "GameModeComponent.h"

#define GAME_MODE_DATA "data/map/GameMode.json"
class GameModeManager
{
public:
	static void createInstance() { assert(sm_instance == nullptr); sm_instance = new GameModeManager(); };
	static void destroyInstance() { assert(sm_instance != nullptr); delete(sm_instance); sm_instance = nullptr; };
	static GameModeManager * getInstance() { return sm_instance; };


	void registerTile(kitten::K_GameObject* p_tileGO, GameModeComponent::TileType p_type);
	void listenEvent(kitten::Event::EventType p_type, kitten::Event* p_data);

	void gainPoint(int p_clientId, int p_points);

	void initComponents();
	void removeModeComponent(GameModeComponent* p_comp);
private:
	static GameModeManager* sm_instance;
	GameModeManager();
	~GameModeManager();

	void init();

	void registerEvent();
	void deregisterEvent();

	void checkPoints();
private:
	std::unordered_map<GameModeComponent::TileType, GameModeComponent*> m_modeComponentMap;

	std::vector<int> m_points; //the current point that player has

	int m_maxPoint;//when player reach max point, he wins

};