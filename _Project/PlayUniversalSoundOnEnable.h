#pragma once

#include "kitten\K_Component.h"
#include "kitten\audio\AudioSource.h"

#include <string>

class PlayUniversalSoundOnEnable : public kitten::K_Component
{
private:
	const std::string m_soundName;
	const bool m_is3D;

	virtual void onEnabled() override;

public:
	PlayUniversalSoundOnEnable(const std::string& p_soundName, bool p_is3D);
	~PlayUniversalSoundOnEnable();
};