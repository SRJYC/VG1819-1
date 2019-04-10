#pragma once

#include "kitten\K_Component.h"
#include "kitten\K_Time.h"

#include "kitten\audio\AudioSource.h"

#include <vector>

class AmbientSystemController : public kitten::K_Component
{
public:
	struct AmbientEvent
	{
		const glm::vec3 place;
		const float minTimeBetweenEvents, const maxTimeBetweenEvents;
		float timeElapsed = 0.0f, timeToNextEvent = 0.0f;
		kitten::K_GameObject* gameObject;

		AmbientEvent(const glm::vec3& p_place, kitten::K_GameObject* p_go, float p_minTime, float p_maxtTime)
			: place(p_place), gameObject(p_go), minTimeBetweenEvents(p_minTime), maxTimeBetweenEvents(p_maxtTime) 
		{
			reset();
		}

		bool isReady() const { return timeElapsed >= timeToNextEvent; }
		void update(float p_deltaTime) { timeElapsed += p_deltaTime; }
		void reset()
		{
			timeElapsed = 0.0f;
			timeToNextEvent = LERP(((float)rand() / (float)RAND_MAX), minTimeBetweenEvents, maxTimeBetweenEvents);
		}
	};

private:

	struct PersistentSound
	{
		kitten::AudioSource* sound;
		const float volume;

		PersistentSound(kitten::AudioSource* p_source, float p_volume) :
			sound(p_source), volume(p_volume) {}
	};

	static AmbientSystemController* sm_instance;

	kitten::K_Time* m_kTime;

	std::vector<AmbientEvent> m_ambientEvents;
	std::vector<PersistentSound> m_persistentSounds;

	int m_persistentSoundDelay;
	float m_volume;


	virtual void start() override;
	virtual bool hasUpdate() const override { return true; };
	virtual void update() override;

public:
	AmbientSystemController(const std::vector<AmbientEvent>& p_ambientEvents, const std::vector<kitten::AudioSource*> p_persistentSounds);
	~AmbientSystemController();

	void registerAmbientEffect(const std::string& p_upfxFile, const glm::vec3& p_pos, float p_minTime, float p_maxTime);

	// Can be NULL !
	static AmbientSystemController* getInstance();

	void setVolume(float p_volume);
	float getVolume() const;
};
