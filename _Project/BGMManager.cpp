#include "BGMManager.h"

#include "SoundFader.h"

#include "kitten\K_GameObjectManager.h"


BGMManager* BGMManager::sm_instance = nullptr;

BGMManager::BGMManager(const std::list<std::pair<std::string, std::string>>& p_soundsToCreate, float p_crossFadeTime) : 
	m_soundsToCreate(p_soundsToCreate), m_playingSource(nullptr), m_crossFadeTime(p_crossFadeTime)
{
	assert(sm_instance == nullptr);
	sm_instance = this;
}

BGMManager::~BGMManager()
{
	assert(sm_instance == this);
	sm_instance = nullptr;
}

void BGMManager::start()
{
	kitten::K_GameObjectManager::getInstance()->flagGameObjectToSurvive(m_attachedObject);

	auto end = m_soundsToCreate.cend();
	for (auto it = m_soundsToCreate.cbegin(); it != end; ++it)
	{
		auto soundName = (*it).first;
		auto soundPath = (*it).second;

		auto createdSound = kitten::K_GameObjectManager::getInstance()->createNewGameObject(soundPath);
		kitten::AudioSource* createdAudioSource = createdSound->getComponent<kitten::AudioSource>();

		BGMEntry entry(createdAudioSource, createdAudioSource->getVolume());

		m_tracks.insert(std::make_pair(soundName, entry));

		createdSound->getTransform().setParent(&(getTransform()));
	}

	m_soundsToCreate.clear();

	privateSetVolume(m_volume);
}

void BGMManager::playBGM(const std::string& p_name)
{
	sm_instance->privatePlayBGM(p_name);
}

void BGMManager::stopBGM()
{
	sm_instance->privateStopBGM();
}

void BGMManager::privatePlayBGM(const std::string& p_name)
{
	if (m_playingName != p_name)
	{
		auto found = m_tracks.find(p_name);
		assert(found != m_tracks.end());

		if (m_playingSource != nullptr)
		{
			SoundFader* playingFader = m_playingSource->getGameObject().getComponent<SoundFader>();
			playingFader->fadeOut(m_crossFadeTime);
		}

		auto source = (*found).second.source;
		SoundFader* fader = source->getGameObject().getComponent<SoundFader>();
		fader->fadeIn(m_crossFadeTime, (*found).second.originalVolume * m_volume);

		m_playingSource = source;
		m_playingName = p_name;
	}
}

void BGMManager::privateStopBGM()
{
	if (m_playingSource != nullptr)
	{
		m_playingSource->stop();
		m_playingSource = nullptr;
	}
}

void BGMManager::setVolume(float p_volume)
{
	sm_instance->privateSetVolume(p_volume);
}

void BGMManager::privateSetVolume(float p_volume)
{
	auto end = m_tracks.cend();
	for (auto it = m_tracks.cbegin(); it != end; ++it)
	{
		const auto& entry = (*it).second;
		
		SoundFader* fader = entry.source->getGameObject().getComponent<SoundFader>();
		
		if (!fader->isFading())
		{
			entry.source->setVolume(p_volume * entry.originalVolume);
		}
		else
		{
			float progress = fader->getProgress();
			float fadeTime = fader->getFadeTime();
			float timeLeft = fadeTime - (progress * fadeTime);
			
			if (fader->isFadingOut())
			{
				fader->fadeOut(timeLeft);
			}
			else // Fading in
			{
				float volume = entry.originalVolume * p_volume;

				fader->fadeIn(timeLeft, volume, true);
			}
		}
	}

	m_volume = p_volume;
}