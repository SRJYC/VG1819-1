#pragma once

#include "kitten\K_Component.h"
#include "kitten\K_ParticleSystem.h"
#include "kitten\InputManager.h"

#include <string>
#include <unordered_map>
#include <queue>
#include <list>
#include <map>

#define UNIT_DEATH_EFFECT_NAME "UnitDeath"

class UniversalPfx : public kitten::K_Component
{
private:
	static UniversalPfx* sm_instance;

	std::unordered_map<std::string, std::queue<kitten::K_GameObject*>> m_effects;
	std::vector<kitten::K_ParticleSystem*> m_particleSystems;
	std::unordered_map<std::string, glm::vec3> m_groupedEffects;

	bool m_isDebug;
	char m_debugRefreshKey;

	input::InputManager* m_inputMan;

	virtual void start() override;

	virtual bool hasUpdate() const override
	{
		return true;
	}

	virtual void update() override;

public:
						//		              name,		pathToEffect, num to pool
	UniversalPfx(const std::list<std::tuple<std::string, std::string, int>>& p_effects, bool p_isDebug, char p_debugKey = 'R');
	~UniversalPfx();

	static UniversalPfx* getInstance() { assert(sm_instance != nullptr);  return sm_instance; }

	void addEffectToGroup(const std::string& p_effectName, const glm::vec3& p_position);
	void playEffect(const std::string& p_effectName, const glm::vec3& p_position);
	void playGroupedEffects();
};