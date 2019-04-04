#pragma once
#include "kitten\K_Component.h"
#include "kitten\InputManager.h"

#include "puppy\P_Common.h"

#include <json.hpp>

#include <tuple>
#include <string>

#define PLAYER_PREFS_FILE_PATH "data/playerprefs.json"

class PlayerPrefs : public kitten::K_Component
{

private:
	static PlayerPrefs* sm_instance;

	int m_sfxVolume, m_bgmVolume, m_ambientVolume;
	bool m_fullscreen;
	std::pair<int, int> m_resolution;
	std::string m_playerName;

	bool m_hasUnsavedChanges;

	std::string toJsonString();

	virtual void start() override;

	void privateSetSFXVolume(int p_volume);
	void privateSetBGMVolume(int p_volume);
	void privateSetAmbientVolume(int p_volume);
	void privateSetPlayerName(const std::string& p_name);

	void privateSetFullscreen(bool p_isFullscreen);
	void privateSetResolution(int p_winX, int p_winY);

	void privateSaveAllSettings();
	void privateAsyncSaveAllSettings();
public:
	PlayerPrefs();
	~PlayerPrefs();

	static void setSFXVolume(int p_volume);
	static int getSFXVolume();

	static void setBGMVolume(int p_volume);
	static int getBGMVolume();

	static void setAmbientVolume(int p_volume);
	static int getAmbientVolume();

	static void setPlayerName(const std::string& p_name);
	const static std::string& getPlayerName();

	//This doesn't do anything, glfw version does not support
	static void setFullscreen(bool p_isFullscreen);
	//always false for now
	static bool getFullscreen();

	static void setResolution(int p_winX, int p_winY);
	static std::pair<int, int> getResolution();

	static void saveAllSettings();

	//Non-functional until jobs fixed
	static void asyncSaveAllSettings();
};