#include "AudioListener.h"
#include "AudioEngineWrapper.h"
#include "kitten\InputManager.h"
namespace kitten
{
	AudioListener::AudioListener()
	{

	}

	AudioListener::~AudioListener()
	{
		m_cachedTransform->removePositionListener(this);
	}

	void AudioListener::start()
	{
		m_cachedTransform = &getTransform();
		m_cachedTransform->addPositionListener(this);
		onPosChanged(m_cachedTransform->getTranslation());
	}

	void AudioListener::onPosChanged(const glm::vec3& p_newPos)
	{
		irrklang::vec3df pos(p_newPos.x,p_newPos.y,p_newPos.z);
		irrklang::vec3df vel(0, 0, 0); // Only used for doppler effect

		const glm::vec3& forward = m_cachedTransform->getForward();
		irrklang::vec3df look(0, forward.y, -1);

		AudioEngineWrapper::sm_instance->m_audioEngine->setListenerPosition(pos,look);
	}
}
