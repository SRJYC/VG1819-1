#pragma once

#include "kitten\audio\AudioSource.h"
#include "kitten\K_Time.h"

class SoundFader : public kitten::K_Component
{
private:
	kitten::AudioSource* m_sound;
	kitten::K_Time* m_kTime;

	bool m_isFadingOut, m_isFading;
	float m_timeToFade, m_currentTime;
	float m_originalVolume, m_endVolume;

	virtual void start();
	virtual bool hasUpdate() const override { return true; }
	virtual void update() override;

public:
	SoundFader();
	~SoundFader();

	void fadeOut(float p_time);
	void fadeIn(float p_time, float p_endVolume, bool p_startAtOriginalVolume = false);

	bool isFading() const;
	bool isFadingIn() const;
	bool isFadingOut() const;

	float getProgress() const;
	float getFadeTime() const;
};