#include "SoundFader.h"
#include "util\MathUtil.h"

#include <iostream>

SoundFader::SoundFader() : m_sound(nullptr), m_kTime(nullptr), m_isFading(false)
{

}

SoundFader::~SoundFader()
{

}

void SoundFader::start()
{
	m_sound = m_attachedObject->getComponent<kitten::AudioSource>();
	assert(m_sound != nullptr);	

	m_kTime = kitten::K_Time::getInstance();
	assert(m_kTime != nullptr);
}

void SoundFader::fadeIn(float p_time, float p_endVolume, bool p_startAtOriginalVolume)
{
	m_isFading = true;
	m_isFadingOut = false;

	m_timeToFade = p_time;
	m_endVolume = p_endVolume;
	m_currentTime = 0;

	if (!p_startAtOriginalVolume)
	{
		m_sound->setVolume(0);
		m_originalVolume = 0;
	}
	else
	{
		m_originalVolume = m_sound->getVolume();
	}

	if (m_sound->isPaused() || m_sound->isFinished())
	{
		m_sound->play();
	}
	
	setEnabled(true);
}

void SoundFader::fadeOut(float p_time)
{
	m_isFading = true;
	m_isFadingOut = true;
	
	m_timeToFade = p_time;
	m_currentTime = 0;

	m_originalVolume = m_sound->getVolume();

	setEnabled(true);
}

bool SoundFader::isFading() const
{
	return m_isFading;
}

bool SoundFader::isFadingIn() const
{
	return isFading() && !m_isFadingOut;
}

bool SoundFader::isFadingOut() const
{
	return isFading() && m_isFadingOut;
}

float SoundFader::getProgress() const
{
	if (!isFading())
	{
		return 0;
	}
	//else

	return (m_currentTime / m_timeToFade);
}

float SoundFader::getFadeTime() const
{
	if (!isFading())
	{
		return 0;
	}

	return m_timeToFade;
}

void SoundFader::update()
{
	if (!m_isFading)
	{
		std::cerr << "SoundFader is updating but shouldn't be ... disabling" << std::endl;
		setEnabled(false);
		return;
	}

	float deltaTime = m_kTime->getDeltaTime();
	deltaTime = CLAMP(deltaTime, 0, 0.1f); // frame after a loading screen (where we call to switch BGM) has large deltatime -- just clamp to 1/10th of a second
	
	m_currentTime += deltaTime;
	m_currentTime = CLAMP(m_currentTime, 0, m_timeToFade);

	bool isLastFrame = (m_currentTime >= m_timeToFade);

	if (m_isFadingOut)
	{
		if (isLastFrame)
		{
			m_sound->stop();
		}
		else
		{
			float volume = LERP((m_currentTime / m_timeToFade), m_originalVolume, 0.0f);
			m_sound->setVolume(volume);
		}
	}
	else
	{
		float volume = LERP((m_currentTime / m_timeToFade), m_originalVolume, m_endVolume);
		m_sound->setVolume(volume);
	}

	if (isLastFrame)
	{
		setEnabled(false);
		m_isFading = false;
	}
}