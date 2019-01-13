#include "ComponentDataType.hpp"
#include <string>

#define JSONHAS(str) p_jsonFile->find(str) != p_jsonFile->end()
#define LOOKUP(str) p_jsonFile->operator[](str)
#define SET(varName,jsonName) varName = p_jsonFile->operator[](jsonName)
#define SETOPT(varName,jsonName) if(JSONHAS(jsonName)) SET(varName,jsonName)
#define SETOPTDEF(varName,jsonName,defaultVal) if(JSONHAS(jsonName)) SET(varName,jsonName); else varName = defaultVal

#include "_Project/MoveByMouseRightClickDrag.h"
kitten::K_Component* getMoveByMouseRightClickDrag(nlohmann::json* p_jsonFile) {
	float speed;
	glm::vec2 minClamp, maxClamp;

	SETOPTDEF(speed, "speed", 0.005f);
	
	if (JSONHAS("minClamp")) {
		minClamp = glm::vec2(LOOKUP("minClamp")[0], LOOKUP("minClamp")[1]);
	}

	if (JSONHAS("maxClamp")) {
		maxClamp = glm::vec2(LOOKUP("maxClamp")[0], LOOKUP("maxClamp")[1]);
	}

	return new MoveByMouseRightClickDrag(speed, minClamp, maxClamp);
}

#include "_Project/ZoomByMouseWheel.h"
kitten::K_Component* getZoomByMouseWheel(nlohmann::json* p_jsonFile) {
	float speed;
	int minFOV, maxFOV;

	SETOPTDEF(speed, "speed", 1.0f);
	SETOPTDEF(minFOV, "minfov", 1);
	SETOPTDEF(maxFOV, "maxfov", 70);

	return new ZoomByMouseWheel(speed, minFOV, maxFOV); 
}

#include "kitten/Camera.h"
kitten::K_Component* getCamera(nlohmann::json* p_jsonFile) {
	kitten::Camera* component = new kitten::Camera();
	if (p_jsonFile->find("fov") != p_jsonFile->end()) {
		component->setFOV(p_jsonFile->operator[]("fov"));
	}

	if (p_jsonFile->find("near") != p_jsonFile->end()) {
		component->setNearClip(p_jsonFile->operator[]("near"));
	}

	if (p_jsonFile->find("far") != p_jsonFile->end()) {
		component->setFarClip(p_jsonFile->operator[]("far"));
	}

	if (p_jsonFile->find("width") != p_jsonFile->end()) {
		component->setWinWidth(p_jsonFile->operator[]("width"));
	}

	if (p_jsonFile->find("height") != p_jsonFile->end()) {
		component->setWinHeight(p_jsonFile->operator[]("height"));
	}

	return component;
}

#include "kitten/CubeRenderable.h"
kitten::K_Component* getCubeRenderable(nlohmann::json* p_jsonFile) {
	std::string texturefilename;

	SETOPTDEF(texturefilename, "texture", "textures/tiles/MISSING.tga");

	return new kitten::CubeRenderable(texturefilename.c_str());
}

#include "kitten/QuadRenderable.h"
kitten::K_Component* getQuadRenderable(nlohmann::json* p_jsonFile) {
	std::string texturefilename;
	bool isStatic;

	SETOPTDEF(texturefilename, "texture", "");
	SETOPTDEF(isStatic, "static", false);

	return new kitten::QuadRenderable(texturefilename.c_str(), isStatic);
}

#include "kitten/QuadRenderableRepeat.h"
kitten::K_Component* getQuadRenderableRepeat(nlohmann::json* p_jsonFile) {
	std::string texturefilename;
	bool isStatic, texRepeat;
	float uRepeat, vRepeat;

	SETOPTDEF(texturefilename, "texture", "");
	SETOPTDEF(isStatic, "static", false);
	SETOPTDEF(texRepeat, "texRepeat", false);
	SETOPTDEF(uRepeat, "uRepeat", 1.0f);
	SETOPTDEF(vRepeat, "vRepeat", 1.0f);

	return new kitten::QuadRenderableRepeat(texturefilename.c_str(), isStatic, texRepeat, uRepeat, vRepeat);
}

#include "gameworld/GrassLandInfoComponent.h"
kitten::K_Component* getGrassLandInfo(nlohmann::json* p_jsonFile) {
	return new gameworld::GrasslandInfoComponent();
}

#include "_Project/DebugPrintOnce.h"
kitten::K_Component* getDebugPrintOnce(nlohmann::json* p_jsonFile){
	std::string message;

	SETOPTDEF(message, "message", "Default Message");

	return new DebugPrintOnce(message);
}

#include "board/clickable/PrintWhenClicked.h"
kitten::K_Component* getPrintWhenClicked(nlohmann::json* p_jsonFile) {
	std::string message;

	SETOPTDEF(message, "message", "Default Message");

	return new PrintWhenClicked(message);
}

#include "kitten\mouse picking\ClickableBox.h"
kitten::K_Component* getClickableBox(nlohmann::json* p_jsonFile) {
	glm::vec3 minPoint, maxPoint;

	if (JSONHAS("minpoint")) {
		minPoint = glm::vec3(LOOKUP("minpoint")[0], LOOKUP("minpoint")[1], LOOKUP("minpoint")[2]);
	}

	if (JSONHAS("maxpoint")) {
		maxPoint = glm::vec3(LOOKUP("maxpoint")[0], LOOKUP("maxpoint")[1], LOOKUP("maxpoint")[2]);
	}

	return new kitten::ClickableBox(minPoint, maxPoint);
}

#include "_Project\DestroyOnClick.h"
kitten::K_Component* getDestroyOnClick(nlohmann::json* p_jsonFile){
	return new DestroyOnClick();
}


#include "board/component/BoardCreator.h"
kitten::K_Component* getBoardCreator(nlohmann::json* p_jsonFile){
	BoardCreator* component = new BoardCreator();

	int x,z;
	SETOPTDEF(x, "rows", 15);
	SETOPTDEF(z, "columns", 15);
	component->setDimension(x, z);
	
	bool enableTileInfoDisplay;
	SETOPTDEF(enableTileInfoDisplay, "enableTileInfoDisplay", false);
	component->setTileInfoDisplay(enableTileInfoDisplay);

	return component;
}

#include "kitten\audio\AudioSource.h"
kitten::K_Component* getAudioSource(nlohmann::json* p_jsonFile) {

	std::string pathToClip;
	bool is3D, enableEffects, causesDuck, getsDucked;

	SET(pathToClip, "clippath");
	SETOPT(is3D, "is3d");
	SETOPT(enableEffects, "enableeffects");
	SETOPTDEF(causesDuck, "causesduck", false);
	SETOPTDEF(getsDucked, "getsducked", false);

	kitten::AudioSource* toReturn = new kitten::AudioSource(pathToClip, is3D, enableEffects, causesDuck, getsDucked);

	if (is3D) {
		if (JSONHAS("mindistance")) {
			toReturn->setMinDistance(LOOKUP("mindistance"));
		}

		if (JSONHAS("maxdistance")) {
			toReturn->setMaxDistance(LOOKUP("maxdistance"));
		}
	}

	if (JSONHAS("loop")) {
		toReturn->setLooped(LOOKUP("loop"));
	}

	if (JSONHAS("volume")) {
		toReturn->setVolume(LOOKUP("volume"));
	}

	if (JSONHAS("playprogress")) {
		toReturn->setPlayProgress(LOOKUP("playprogress"));
	}

	if (enableEffects) {
		//Effects parsing here

#define innerJsonHas(str) innerJson.find(str) != innerJson.end()
#define setInnerVar(varName,jsonName) if(innerJsonHas(jsonName)) varName = innerJson.operator[](jsonName)

		auto sfx = toReturn->getSFXControl();

		if (p_jsonFile->find("choruseffect") != p_jsonFile->end()) {
			irrklang::ik_f32 wetDryMix = 50, depth = 10, feedback = 25, frequency = 1.1;
			bool sinusWaveForm = true;
			irrklang::ik_f32 delay = 16, phase = 90;
			
			auto innerJson = p_jsonFile->operator[]("choruseffect");

			setInnerVar(wetDryMix, "wetdrymix");
			setInnerVar(depth, "depth");
			setInnerVar(feedback, "feedback");
			setInnerVar(frequency, "frequency");
			setInnerVar(sinusWaveForm, "sinuswaveform");
			setInnerVar(delay, "delay");
			setInnerVar(phase, "phase");

			sfx->setChorusEffect(wetDryMix, depth, feedback, frequency, sinusWaveForm, delay, phase);
		}

		if (p_jsonFile->find("compressoreffect") != p_jsonFile->end()) {
			irrklang::ik_f32 gain = 0, attack = 10, release = 200, threshold = -20,
				ratio = 3, preDelay = 4;

			auto innerJson = p_jsonFile->operator[]("compressoreffect");

			setInnerVar(gain, "gain");
			setInnerVar(attack, "attack");
			setInnerVar(release, "release");
			setInnerVar(threshold, "threshold");
			setInnerVar(ratio, "ratio");
			setInnerVar(preDelay, "predelay");

			sfx->setCompressorEffect(gain, attack, release, threshold, ratio, preDelay);
		}

		if (p_jsonFile->find("distortioneffect") != p_jsonFile->end()) {
			irrklang::ik_f32 gain = -18, edge = 15, postEQCenterFrequency = 2400,
				postEQBandwidth = 2400, preLowpassCutoff = 8000;

			auto innerJson = p_jsonFile->operator[]("distortioneffect");

			setInnerVar(gain, "gain");
			setInnerVar(edge, "edge");
			setInnerVar(postEQCenterFrequency, "posteqcenterfrequency");
			setInnerVar(postEQBandwidth, "posteqbandwidth");
			setInnerVar(preLowpassCutoff, "prelowpasscutoff");

			sfx->setDistortionEffect(gain, edge, postEQCenterFrequency, postEQBandwidth, preLowpassCutoff);
		}

		if (p_jsonFile->find("echoeffect") != p_jsonFile->end()) {
			irrklang::ik_f32 wetDryMix = 50, feedback = 50,
				leftDelay = 500, rightDelay = 500, panDelay = 0;

			auto innerJson = p_jsonFile->operator[]("echoeffect");

			setInnerVar(wetDryMix, "wetdrymix");
			setInnerVar(feedback, "feedback");
			setInnerVar(leftDelay, "leftdelay");
			setInnerVar(rightDelay, "rightdelay");
			setInnerVar(panDelay, "pandelay");

			sfx->setEchoEffect(wetDryMix, feedback, leftDelay, rightDelay, panDelay);
		}

		if (p_jsonFile->find("flangereffect") != p_jsonFile->end()) {
			irrklang::ik_f32 wetDryMix = 50, depth = 100,
				feedback = -50, frequency = 0.25f, delay = 2;
			irrklang::ik_s32 phase = 0;
			bool triangleWaveForm = true;

			auto innerJson = p_jsonFile->operator[]("flangereffect");

			setInnerVar(wetDryMix, "wetdrymix");
			setInnerVar(depth, "depth");
			setInnerVar(feedback, "feedback");
			setInnerVar(frequency, "frequency");
			setInnerVar(delay, "delay");
			setInnerVar(phase, "phase");
			setInnerVar(triangleWaveForm, "trianglewaveform");

			sfx->setFlangerEffect(wetDryMix, depth, feedback, frequency, triangleWaveForm, delay, phase);
		}

		if (p_jsonFile->find("gargleeffect") != p_jsonFile->end()) {
			irrklang::ik_s32 rateHz = 20;
			bool sinusWaveForm = true;

			auto innerJson = p_jsonFile->operator[]("gargleffect");

			setInnerVar(rateHz, "ratehz");
			setInnerVar(sinusWaveForm, "sinuswaveform");

			sfx->setGargleEffect(rateHz, sinusWaveForm);
		}

		if (p_jsonFile->find("3dreverbeffect") != p_jsonFile->end()) {
			irrklang::ik_s32 room = -1000, roomHF = -100,
				reflections = -2602, reverb = 200;
			irrklang::ik_f32 roomRolloffFactor = 0, decayTime = 1.49f, decayHFRatio = 0.83f,
				reflectionsDelay = 0.007f, reverbDelay = 0.011f, diffusion = 100,
				density = 100, hfReference = 5000;

			auto innerJson = p_jsonFile->operator[]("3dreverbeffect");

			setInnerVar(room, "room");
			setInnerVar(roomHF, "roomHF");
			setInnerVar(reflections, "reflections");
			setInnerVar(reverb, "reverb");
			setInnerVar(roomRolloffFactor, "roomrollofffactor");
			setInnerVar(decayTime, "decaytime");
			setInnerVar(decayHFRatio, "decayhfratio");
			setInnerVar(reflectionsDelay, "reflectionsdelay");
			setInnerVar(reverbDelay, "reverbdelay");
			setInnerVar(diffusion, "diffusion");
			setInnerVar(density, "density");
			setInnerVar(hfReference, "hfReference");

			sfx->set3DReverbEffect(room, roomHF, roomRolloffFactor, decayTime, decayHFRatio, reflections, reflectionsDelay, reverb, reverbDelay, diffusion, density, hfReference);
		}

		if (p_jsonFile->find("parameqeffect") != p_jsonFile->end()) {
			irrklang::ik_f32 center = 8000, bandwidth = 12,
				gain = 0;

			auto innerJson = p_jsonFile->operator[]("parameqeffect");

			setInnerVar(center, "center");
			setInnerVar(bandwidth, "bandwidth");
			setInnerVar(gain, "gain");

			sfx->setParamEqEffect(center, bandwidth, gain);
		}

		if (p_jsonFile->find("wavesreverbeffect") != p_jsonFile->end()) {
			irrklang::ik_f32 gain = 0, reverbMix = 0,
				reverbTime = 1000, highFreqRTRatio = 0.001f;

			auto innerJson = p_jsonFile->operator[]("wavesreverbeffect");

			setInnerVar(gain, "gain");
			setInnerVar(reverbMix, "reverbmix");
			setInnerVar(reverbTime, "reverbtime");
			setInnerVar(highFreqRTRatio, "highfreqrtratio");

			sfx->setWavesReverbEffect(gain, reverbMix, reverbTime, highFreqRTRatio);
		}
	}

	return toReturn;
}

#include "kitten\audio\AudioListener.h"
kitten::K_Component* getAudioListener(nlohmann::json* p_jsonFile) {
	return new kitten::AudioListener();
}

#include "_Project\PlaySoundOnKeyPress.h"
kitten::K_Component* getPlaySoundOnKeyPress(nlohmann::json* p_jsonFile) {
	char key;

	if (JSONHAS("key")) {
		std::string strKey = LOOKUP("key");
		key = strKey[0];
	}

	return new PlaySoundOnKeyPress(key);
}

#include "_Project\PlaySoundOnStart.h"
kitten::K_Component* getPlaySoundOnStart(nlohmann::json* p_jsonFile) {
	return new PlaySoundOnStart();
}

#include "_Project\ToggleSoundOnKeyPress.h"
kitten::K_Component* getToggleSoundOnStart(nlohmann::json* p_jsonFile) {
	char key;

	if (JSONHAS("key")) {
		std::string strKey = LOOKUP("key");
		key = strKey[0];
	}

	return new ToggleSoundOnKeyPress(key);
}

#include "_Project\VolumeAdjustOnKeysPressed.h"
kitten::K_Component* getVolumeAdjustOnKeysPressed(nlohmann::json* p_jsonFile) {
	char increaseKey, decreaseKey;
	float changeAmount;

	if (JSONHAS("increasekey")) {
		std::string strKey = LOOKUP("increasekey");
		increaseKey = strKey[0];
	}

	if (JSONHAS("decreasekey")) {
		std::string strKey = LOOKUP("decreasekey");
		decreaseKey = strKey[0];
	}

	SETOPT(changeAmount, "changeamount");

	return new VolumeAdjustOnKeysPressed(increaseKey,decreaseKey,changeAmount);
}

#include "ui/UIFrame.h"
kitten::K_Component* getUIFrame(nlohmann::json* p_jsonFile) {
	std::string texture;

	SETOPTDEF(texture, "texture", "textures/ui/blankFrame.tga");

	return new userinterface::UIFrame(texture.c_str());
}

#include "unit/unitComponent/UnitMove.h"
kitten::K_Component* getUnitMove(nlohmann::json* p_jsonFile) {
	glm::vec3 offset;

	if (JSONHAS("offset")) {
		offset = glm::vec3(LOOKUP("offset")[0], LOOKUP("offset")[1], LOOKUP("offset")[2]);
		return new unit::UnitMove(offset);
	}

	return new unit::UnitMove();
}

#include "board/clickable/ManipulateTileOnClick.h"
kitten::K_Component* getManipulateTileOnClick(nlohmann::json* p_jsonFile) {
	return new ManipulateTileOnClick();
}

#include "_Project/UseAbilityWhenClicked.h"
kitten::K_Component* getUseAbilityWhenClicked(nlohmann::json* p_jsonFile) {
	return new UseAbilityWhenClicked();
}

#include "board/clickable/SendSelfOnClick.h"
kitten::K_Component* getSendSelfOnClick(nlohmann::json* p_jsonFile) {
	return new SendSelfOnClick();
}

#include "_Project/FPSCalc.h"
kitten::K_Component* getFPSCalc(nlohmann::json* p_jsonFile) {
	return new FPSCalc();
}

#include "unit/InitiativeTracker/TrackerBlock.h"
kitten::K_Component* getTrackerBlock(nlohmann::json* p_jsonFile) {
	return new unit::TrackerBlock();
}

#include "unit/InitiativeTracker/TrackerBlockClickable.h"
kitten::K_Component* getTrackerBlockClickable(nlohmann::json* p_jsonFile) {
	return new unit::TrackerBlockClickable();
}

#include "unit/InitiativeTracker/TrackerPointer.h"
kitten::K_Component* getTrackerPointer(nlohmann::json* p_jsonFile) {
	return new unit::TrackerPointer();
}

#include "unit/InitiativeTracker/PointerUI.h"
kitten::K_Component* getPointerUI(nlohmann::json* p_jsonFile) {
	return new userinterface::PointerUI();
}

#include "unit/unitComponent/UnitGraphic.h"
kitten::K_Component* getUnitGraphic(nlohmann::json* p_jsonFile) {
	std::string texture;
	unit::UnitSize size = unit::point;

	SETOPTDEF(texture, "texture", "textures/unit/Default.tga");

	if (p_jsonFile->find("unitsize") != p_jsonFile->end()) {
		std::string temp = p_jsonFile->operator[]("unitsize");
		if (temp == "point")
			size = unit::point;
		else if (temp == "cube")
			size = unit::UnitSize::cube;
	}

	return new unit::UnitGraphic(size, texture.c_str());
}

#include "puppy/Text/TextBox.h"
kitten::K_Component* getTextBox(nlohmann::json* p_jsonFile) {
	std::string font , message;
	float width, height;
	puppy::TextBox* textbox;

	SETOPTDEF(font, "font", "../fonts/common_dejavu.fnt");
	SETOPTDEF(message, "message", "DEFAULT TEXT");
	SETOPTDEF(width, "width", 500);
	SETOPTDEF(height, "height", 500);

	puppy::TextBox::Alignment align = puppy::TextBox::Alignment::left;

	if (JSONHAS("alignment")) {
		std::string temp = LOOKUP("alignment");
		if (temp == "left") {
			align = puppy::TextBox::Alignment::left;
		}
		else if (temp == "right") {
			align = puppy::TextBox::Alignment::right;
		}
		else if (temp == "center") {
			align = puppy::TextBox::Alignment::center;
		}
	}

	bool is3D;
	SETOPTDEF(is3D, "3D", false);

	textbox = new puppy::TextBox(puppy::FontTable::getInstance()->getFont(font.c_str()), message.c_str(), width, height, align, is3D);

	if (JSONHAS("color")) {
		textbox->setColor(LOOKUP("color")[0], LOOKUP("color")[1], LOOKUP("color")[2]);
	}

	

	return textbox;
}

#include "_Project\ToggleEnabledOnKeyPress.h"
kitten::K_Component* getToggleEnabledOnKeyPress(nlohmann::json* p_jsonFile) {
	char key;
	
	if (p_jsonFile->find("key") != p_jsonFile->end()) {
		std::string str = p_jsonFile->operator[]("key");
		key = str[0];
	}

	return new ToggleEnabledOnKeyPress(key);
}

#include "_Project\ToggleStringInputOnKeyPress.h"
kitten::K_Component* getToggleStringInputOnKeyPress(nlohmann::json* p_jsonFile) {
	char key;
	
	if (p_jsonFile->find("key") != p_jsonFile->end()) {
		std::string str = p_jsonFile->operator[]("key");
		key = str[0];
	}

	return new ToggleStringInputOnKeyPress(key);
}

#include "components\TileInfoDisplayOnKeyPress.h"
kitten::K_Component* getTileInfoDisplayOnKeyPress(nlohmann::json* p_jsonFile) {
	char key;

	if (p_jsonFile->find("key") != p_jsonFile->end()) {
		std::string str = p_jsonFile->operator[]("key");
		key = str[0];
	}

	return new TileInfoDisplayOnKeyPress(key);
}

#include "_Project\StringInputDisplay.h"
kitten::K_Component* getStringInputDisplay(nlohmann::json* p_jsonFile) {
	return new StringInputDisplay();
}

#include "components\SelectAbility.h"
kitten::K_Component* getSelectAbility(nlohmann::json* p_jsonFile) {
	return new SelectAbility();
}

#include "components\PowerTracker.h"
kitten::K_Component* getPowerTracker(nlohmann::json* p_jsonFile) {
	return new PowerTracker();
}

#include "board/component/Highlighter.h"
kitten::K_Component* getHighlighter(nlohmann::json* p_jsonFile) {
	return new Highlighter();
}

#include "UI/HandFrame.h"
kitten::K_Component* getHandFrame(nlohmann::json* p_jsonFile) {
	std::string texture;

	SETOPTDEF(texture, "texture", "textures/ui/blankFrame.tga");

	return new userinterface::HandFrame(texture.c_str());
}

#include "UI/CardUIO.h"
kitten::K_Component* getCardUIO(nlohmann::json* p_jsonFile) {
	std::string texture;

	SETOPTDEF(texture, "texture", "textures/ui/cardBack.tga");

	userinterface::CardUIO* object = new userinterface::CardUIO(texture.c_str());
	return object;
}

#include "kitten/mouse picking/ClickableFrame.h"
kitten::K_Component* getClickableFrame(nlohmann::json* p_jsonFile) {
	kitten::ClickableFrame::PivotType type = kitten::ClickableFrame::piv_BotLeft;
	if (JSONHAS("pivot")) {
		std::string temp = LOOKUP("pivot");
		if (temp == "left")
			type = kitten::ClickableFrame::piv_Left;
		else if (temp == "right")
			type = kitten::ClickableFrame::piv_Right;
		else if (temp == "center")
			type = kitten::ClickableFrame::piv_Center;
		else if (temp == "top")
			type = kitten::ClickableFrame::piv_Top;
		else if (temp == "bottom")
			type = kitten::ClickableFrame::piv_Bot;
		else if (temp == "botleft")
			type = kitten::ClickableFrame::piv_BotLeft;
		else if (temp == "botright")
			type = kitten::ClickableFrame::piv_BotRight;
		else if (temp == "topleft")
			type = kitten::ClickableFrame::piv_TopLeft;
		else if (temp == "topright")
			type = kitten::ClickableFrame::piv_TopRight;
	}
	return new kitten::ClickableFrame(type);
}

#include "kitten/mouse picking/ClickableUI.h"
kitten::K_Component* getClickableUI(nlohmann::json* p_jsonFile) {
	bool isEnabledOnPause;
	SETOPTDEF(isEnabledOnPause, "enabledOnPause", false);
	return new kitten::ClickableUI(isEnabledOnPause);
}

#include "UI/CommanderContext.h"
kitten::K_Component* getCommanderContext(nlohmann::json* p_jsonFile) {
	return new userinterface::CommanderContext();
}

#include "board/tile/TileInfo.h"
kitten::K_Component* getTileInfo(nlohmann::json* p_jsonFile) {
	return new TileInfo();
}

#include "networking\SpawnUnitOnKeyPress.h"
kitten::K_Component* getSpawnUnitOnKeyPress(nlohmann::json* p_jsonFile) {
	return new SpawnUnitOnKeyPress();
}

#include "networking\NetworkingConsoleMenu.h"
kitten::K_Component* getNetworkingConsoleMenu(nlohmann::json* p_jsonFile) {
	return new NetworkingConsoleMenu();
}

#include "networking\ConnectToHost.h"
kitten::K_Component* getConnectToHost(nlohmann::json* p_jsonFile) {
	return new ConnectToHost();
}

#include "kitten\sprites\SpriteAnimator.h"
kitten::K_Component* getSpriteAnimator(nlohmann::json* p_jsonFile) {

	std::string characterName;

	if (p_jsonFile->find("spritename") != p_jsonFile->end()) {
		std::string name = p_jsonFile->operator[]("spritename");
		characterName = name; // I do not know why this is needed but it is.
	}

	return new sprites::SpriteAnimator(characterName);
}

#include "kitten\sprites\SpriteRenderable.h"
kitten::K_Component* getSpriteRenderable(nlohmann::json* p_jsonFile) {
	return new kitten::SpriteRenderable();
}

#include "components/DeckAlterationComponent.h"
kitten::K_Component* getDeckAlterationComponent(nlohmann::json* p_jsonFile) {
	return new DeckAlterationComponent();
}

#include "components/ChangeSceneOnClick.hpp"
kitten::K_Component* getChangeSceneOnClick(nlohmann::json* p_jsonFile) {

	std::string nextScene;

	SET(nextScene, "scene");

	return new ChangeSceneOnClick(nextScene);
}

#include "components/SetupDeckEditOnSceneChange.h"
kitten::K_Component* getSetupDeckEditOnSceneChange(nlohmann::json* p_jsonFile) {

	std::string nextScene;

	SET(nextScene, "scene");

	return new SetupDeckEditOnSceneChange(nextScene);
}

#include "kitten\K_ParticleSystem.h"
kitten::K_Component* getKParticleSystem(nlohmann::json* p_jsonFile) {
	
	std::string pathToXML = p_jsonFile->operator[]("effectpath");
	
	return new kitten::K_ParticleSystem(pathToXML.c_str());
}

#include "_Project\ToggleParticleSystemOnKeyPress.h"
kitten::K_Component* getToggleParticleSystemOnKeyPress(nlohmann::json* p_jsonFile) {

	std::string keyStr = p_jsonFile->operator[]("key");
	return new ToggleParticleSystemOnKeyPress(keyStr[0]);
}

#include "_Project\UniversalPfx.h"
kitten::K_Component* getUniversalPfx(nlohmann::json* p_jsonFile) {
	
	std::list<std::tuple<std::string, std::string, int>> effects;

	auto end = p_jsonFile->at("effects").end();
	for (auto it = p_jsonFile->at("effects").begin(); it != end; ++it)
	{
		std::string effectName = (*it)["name"];
		std::string effectPath = (*it)["path"];
		int numPfxToPool = (*it)["poolednumber"];

		effects.push_back(std::make_tuple(effectName, effectPath, numPfxToPool));
	}

	return new UniversalPfx(effects);
}

#include "components/DecksDisplay/DisplayFramePickerOnClick.h"
kitten::K_Component* getDisplayFramePickerOnClick(nlohmann::json* p_jsonFile) {
	return new DisplayFramePickerOnClick();
}

#include "components/DecksDisplay/DisplayFrameSetChangeOnClick.h"
kitten::K_Component* getDisplayFrameSetChangeOnClick(nlohmann::json* p_jsonFile) {
	int offset;

	SETOPTDEF(offset, "offset", 0);

	return new DisplayFrameSetChangeOnClick(offset);
}

#include "components/DecksDisplayFrame.h"
kitten::K_Component* getDecksDisplayFrame(nlohmann::json* p_jsonFile) {
	int marginX;
	int marginY;

	SETOPTDEF(marginY, "marginY", 0);
	SETOPTDEF(marginX, "marginX", 0);

	return new DecksDisplayFrame(marginX, marginY);
}

#include "components/DisplayFrame/DeckUnitsDisplayFrame.h"
kitten::K_Component* getDeckUnitsDisplayFrame(nlohmann::json* p_jsonFile) {
	int marginX;
	int marginY;

	SETOPTDEF(marginY, "marginY", 0);
	SETOPTDEF(marginX, "marginX", 0);

	return new DeckUnitsDisplayFrame(marginX, marginY);
}

#include "components/DisplayFrame/CommanderDisplayFrame.h"
kitten::K_Component* getCommanderDisplayFrame(nlohmann::json* p_jsonFile) {
	int marginX;
	int marginY;

	SETOPTDEF(marginY, "marginY", 0);
	SETOPTDEF(marginX, "marginX", 0);

	return new CommanderDisplayFrame(marginX, marginY);
}

#include "components/DisplayFrame/UnitDisplayFrame.h"
kitten::K_Component* getUnitDisplayFrame(nlohmann::json* p_jsonFile) {
	int marginX;
	int marginY;

	SETOPTDEF(marginY, "marginY", 0);
	SETOPTDEF(marginX, "marginX", 0);

	return new UnitDisplayFrame(marginX, marginY);
}

#include "_Project\ClickableBoxRenderable.h"
kitten::K_Component* getClickableBoxRenderable(nlohmann::json* p_jsonFile) {
	return new ClickableBoxRenderable();
}

#include "components/DeckInitializingComponent.h"
kitten::K_Component* getDeckInitializingComponent(nlohmann::json* p_jsonFile) {
	return new DeckInitializingComponent();
}

#include "components/scene change/StartGameOnClick.h"
kitten::K_Component* getStartGameOnClick(nlohmann::json* p_jsonFile) {
	std::string targetScene;
	SET(targetScene, "scene");
	return new StartGameOnClick(targetScene);
}

#include "components/DeckComponent.hpp"
kitten::K_Component* getDeckComponent(nlohmann::json* p_jsonFile) {
	return new DeckComponent();
}

#include "components/testing/gameplay-init.h"
kitten::K_Component* getGameplayInit(nlohmann::json* p_jsonFile) {
	bool testing;
	SETOPTDEF(testing, "testing", true);
	return new GameplayInit(testing);
}

#include "networking\menu\NetworkJoinButton.h"
kitten::K_Component* getNetworkJoinButton(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");

	userinterface::NetworkJoinButton* button = new userinterface::NetworkJoinButton();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);

	return button;
}

#include "networking\menu\NetworkHostButton.h"
kitten::K_Component* getNetworkHostButton(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");

	userinterface::NetworkHostButton* button = new userinterface::NetworkHostButton();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);

	return button;
}

#include "networking\menu\NetworkConnectButton.h"
kitten::K_Component* getNetworkConnectButton(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");

	userinterface::NetworkConnectButton* button = new userinterface::NetworkConnectButton();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);

	return button;
}

#include "UI\TabMenu\TabMenu.h"
kitten::K_Component* getTabMenu(nlohmann::json* p_jsonFile) {
	std::string texture;

	SETOPTDEF(texture, "texture", "textures/ui/blankFrame.tga");
	return new TabMenu(texture.c_str());
}

#include "ui/UIObject.h"
kitten::K_Component* getUIObject(nlohmann::json* p_jsonFile) {
	std::string texture;

	SETOPTDEF(texture, "texture", "textures/ui/blankFrame.tga");

	return new userinterface::UIObject(texture.c_str());
}

#include "UI\TabMenu\ReturnToMainMenuButton.h"
kitten::K_Component* getReturnToMainMenuButton(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture;
	bool isEnabledOnPause;

	SETOPTDEF(isEnabledOnPause, "enabledOnPause", false);
	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");

	userinterface::ReturnToMainMenuButton* button = new userinterface::ReturnToMainMenuButton();
	button->setEnabledOnPause(isEnabledOnPause);
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);

	return button;
}

#include "UI\ClickableButton.h"
kitten::K_Component* getClickableButton(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture;
	bool isEnabledOnPause;
	SETOPTDEF(isEnabledOnPause, "enabledOnPause", false);
	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");

	userinterface::ClickableButton* button = new userinterface::ClickableButton();
	button->setEnabledOnPause(isEnabledOnPause);
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);

	return button;
}

#include "components\MainMenuBoard.h"
kitten::K_Component* getMainMenuBoard(nlohmann::json* p_jsonFile)
{
	return new MainMenuBoard();
}

#include "unit/InitiativeTracker/UnitAura.h"
kitten::K_Component* getUnitAura(nlohmann::json* p_jsonFile) {
	float p_speed, p_max, p_min;

	SETOPTDEF(p_speed, "speed",0.005);
	SETOPTDEF(p_max, "max",1.2);
	SETOPTDEF(p_min, "min",0.8);

	return new unit::UnitAura(p_speed,p_max,p_min);
}

#include "unit/unitComponent/UnitSelect.h"
kitten::K_Component* getUnitSelect(nlohmann::json* p_jsonFile) {
	return new unit::UnitSelect();
}

#include "components/DrawCardOnClickUI.h"
kitten::K_Component* getDrawCardOnClickUI(nlohmann::json* p_jsonFile) {
	return new DrawCardOnClickUI();
}

#include "kitten/sprites/SpriteGroup.h"
kitten::K_Component* getSpriteGroup(nlohmann::json* p_jsonFile) {
	std::string name;
	int n;
	SETOPT(name, "spritename");
	SETOPTDEF(n, "number", 1);

	sprites::SpriteGroup* sg = new sprites::SpriteGroup(name, n);

	if (p_jsonFile->find("rotate") != p_jsonFile->end()) {
		sg->setRotation(glm::vec3(p_jsonFile->operator[]("rotate")[0], p_jsonFile->operator[]("rotate")[1], p_jsonFile->operator[]("rotate")[2]));
	}

	if (p_jsonFile->find("scale") != p_jsonFile->end()) {
		sg->setScale(glm::vec3(p_jsonFile->operator[]("scale")[0], p_jsonFile->operator[]("scale")[1], p_jsonFile->operator[]("scale")[2]));
	}

	if (p_jsonFile->find("translate") != p_jsonFile->end()) {
		sg->setTranslation(glm::vec3(p_jsonFile->operator[]("translate")[0], p_jsonFile->operator[]("translate")[1], p_jsonFile->operator[]("translate")[2]));
	}

	return sg;
}

#include "components/CustomCursor.h"
kitten::K_Component* getCustomCursor(nlohmann::json* p_jsonFile) {
	if (JSONHAS("offset"))
	{
		return new CustomCursor(p_jsonFile->operator[]("offset")[0], p_jsonFile->operator[]("offset")[1]);
	}
	return new CustomCursor();
}

#include "UI/UIElement.h"
kitten::K_Component* getUIElement(nlohmann::json* p_jsonFile) {
	//
	std::string texture;
	SETOPTDEF(texture, "texture", "textures/ui/blankFrame.tga");

	userinterface::UIElement::pivotType type = userinterface::UIElement::piv_BotLeft;
	if (JSONHAS("pivot")) {
		std::string temp = LOOKUP("pivot");
		if (temp == "left")
			type = userinterface::UIElement::piv_Left;
		else if (temp == "right")
			type = userinterface::UIElement::piv_Right;
		else if (temp == "center")
			type = userinterface::UIElement::piv_Center;
		else if (temp == "top")
			type = userinterface::UIElement::piv_Top;
		else if (temp == "bottom")
			type = userinterface::UIElement::piv_Bot;
		else if (temp == "botleft")
			type = userinterface::UIElement::piv_BotLeft;
		else if (temp == "botright")
			type = userinterface::UIElement::piv_BotRight;
		else if (temp == "topleft")
			type = userinterface::UIElement::piv_TopLeft;
		else if (temp == "topright")
			type = userinterface::UIElement::piv_TopRight;
		else
			type = userinterface::UIElement::piv_BotLeft;
	}

	userinterface::UIElement::textureBehaviour tb = userinterface::UIElement::tbh_Stretch;

	if (JSONHAS("texture_behaviour"))
	{
		std::string temp = LOOKUP("behaviour");
		if (temp == "repeat")
		{
			tb = userinterface::UIElement::tbh_Repeat;
		}
		else if (temp == "mirror_repeat")
		{
			tb = userinterface::UIElement::tbh_RepeatMirrored;
		}
	}

	
	if (JSONHAS("behavior"))
	{
		std::string temp = LOOKUP("behavior");
		if (temp == "repeat")
			tb = userinterface::UIElement::tbh_Repeat;
		else if (temp == "repeat_mirror")
			tb = userinterface::UIElement::tbh_RepeatMirrored;
	}
	
	return new userinterface::UIElement(texture.c_str(),type, tb);
}

#include "kitten\ModelRenderable.h"
kitten::K_Component* getModelRenderable(nlohmann::json* p_jsonFile) {

	std::string modelPath = p_jsonFile->operator[]("path");

	bool flipUvs;
	SETOPTDEF(flipUvs, "flipUVs", false);

	bool isStatic;
	SETOPTDEF(isStatic, "static", true);

	return new ModelRenderable(modelPath.c_str(), flipUvs, isStatic);
}

#include "unit\unitComponent\UnitHealthBar.h"
kitten::K_Component* getUnitHealthBar(nlohmann::json* p_jsonFile) {

	glm::vec3 offset = glm::vec3(LOOKUP("offset")[0], LOOKUP("offset")[1], LOOKUP("offset")[2]);

	float lerpTime;
	SETOPTDEF(lerpTime, "lerptime", 4.0f);

	float rotation;
	SETOPTDEF(rotation, "rotation", -45);

	return new unit::UnitHealthBar(offset,lerpTime,rotation);
}

#include "_Project\LerpController.h"
kitten::K_Component* getLerpController(nlohmann::json* p_jsonFile) {

	return new LerpController();
}

#include "components/DragNDrop/SpawnUnitOnDrop.h"
kitten::K_Component* getSpawnUnitOnDrop(nlohmann::json* p_jsonFile) {
	return new SpawnUnitOnDrop();
}

#include "UI/DragNDrop.h"
kitten::K_Component* getDragNDrop(nlohmann::json* p_jsonFile) {
	bool backToOrigin;

	SETOPTDEF(backToOrigin, "backtoorigin", false);

	return new DragNDrop(backToOrigin);
}

#include "components/CameraMoveByEvent.h"
kitten::K_Component* getCameraMoveByEvent(nlohmann::json* p_jsonFile) {

	return new CameraMoveByEvent();
}

#include "components\ExitGameButton.h"
kitten::K_Component* getExitGameButton(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");

	ExitGameButton* button = new ExitGameButton();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);

	return button;
}

#include "components\clickables\SetCommanderOnClick.h"
kitten::K_Component* getSetCommanderOnClick(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture, inactiveTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");
	SETOPT(inactiveTexture, "inactiveTexture");

	SetCommanderOnClick* button = new SetCommanderOnClick();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);
	button->setInactiveTexture(inactiveTexture);

	return button;
}

#include "components\clickables\DiscardDeckOnClick.h"
kitten::K_Component* getDiscardDeckOnClick(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture, inactiveTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");
	SETOPT(inactiveTexture, "inactiveTexture");

	DiscardDeckOnClick* button = new DiscardDeckOnClick();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);
	button->setInactiveTexture(inactiveTexture);

	return button;
}

#include "components\clickables\SaveDeckOnClick.h"
kitten::K_Component* getSaveDeckOnClick(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture, inactiveTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");
	SETOPT(inactiveTexture, "inactiveTexture");

	SaveDeckOnClick* button = new SaveDeckOnClick();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);
	button->setInactiveTexture(inactiveTexture);

	return button;
}

#include "components\clickables\AddUnitOnClick.h"
kitten::K_Component* getAddUnitOnClick(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture, inactiveTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");
	SETOPT(inactiveTexture, "inactiveTexture");

	AddUnitOnClick* button = new AddUnitOnClick();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);
	button->setInactiveTexture(inactiveTexture);

	return button;
}

#include "components\clickables\DeleteDeckOnClick.h"
kitten::K_Component* getDeleteDeckOnClick(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture, inactiveTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");
	SETOPT(inactiveTexture, "inactiveTexture");

	DeleteDeckOnClick* button = new DeleteDeckOnClick();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);

	return button;
}

#include "components\clickables\AddNewDeckOnClick.h"
kitten::K_Component* getAddNewDeckOnClick(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture, inactiveTexture;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");
	SETOPT(inactiveTexture, "inactiveTexture");

	AddNewDeckOnClick* button = new AddNewDeckOnClick();
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);
	button->setInactiveTexture(inactiveTexture);

	return button;
}

#include "components\clickables\ChangeUnitOnClick.h"
kitten::K_Component* getChangeUnitOnClick(nlohmann::json* p_jsonFile) {
	std::string regularTexture, highlightedTexture, inactiveTexture;
	bool increments;

	SETOPT(regularTexture, "regularTexture");
	SETOPT(highlightedTexture, "highlightedTexture");
	SETOPT(inactiveTexture, "inactiveTexture");
	SETOPTDEF(increments, "increments", true);

	ChangeUnitOnClick* button = new ChangeUnitOnClick(increments);
	button->setRegularTexture(regularTexture);
	button->setHighlightedTexture(highlightedTexture);
	button->setInactiveTexture(inactiveTexture);

	return button;
}

#include "unit/unitComponent/unitAction/ActionSelect.h"
kitten::K_Component* getActionSelect(nlohmann::json* p_jsonFile) {
	std::pair<int, int> offset, offset2;

	if (JSONHAS("action offset"))
	{
		int x = p_jsonFile->operator[]("action offset")[0];
		int y = p_jsonFile->operator[]("action offset")[1];
		offset = std::make_pair(x, y);
	}
	if (JSONHAS("cd offset"))
	{
		int x = p_jsonFile->operator[]("cd offset")[0];
		int y = p_jsonFile->operator[]("cd offset")[1];
		offset2 = std::make_pair(x, y);
	}

	unit::ActionSelect* select = new unit::ActionSelect(offset,offset2);

	return select;
}

#include "_Project\CombatText.h"
kitten::K_Component* getCombatText(nlohmann::json* p_jsonFile) {
	int poolSize = p_jsonFile->operator[]("poolsize");
	return new CombatText(poolSize);
}

#include "puppy\Text\PivotTextBox.h"
kitten::K_Component* getPivotTextBox(nlohmann::json* p_jsonFile) {
	std::string font, message;
	userinterface::UIElement::pivotType pivot;
	float width, height;
	puppy::PivotTextBox* textbox;

	SETOPTDEF(font, "font", "../fonts/common_dejavu.fnt");
	SETOPTDEF(message, "message", "DEFAULT TEXT");
	SETOPTDEF(width, "width", 500);
	SETOPTDEF(height, "height", 500);

	puppy::TextBox::Alignment align = puppy::TextBox::Alignment::left;

	if (JSONHAS("alignment")) {
		std::string temp = LOOKUP("alignment");
		if (temp == "left") {
			align = puppy::TextBox::Alignment::left;
		}
		else if (temp == "right") {
			align = puppy::TextBox::Alignment::left;
		}
		else if (temp == "center") {
			align = puppy::TextBox::Alignment::left;
		}
	}

	if (JSONHAS("pivot")) {
		std::string temp = LOOKUP("pivot");
		if (temp == "left")
		{
			pivot = userinterface::UIElement::piv_Left;
		}
		else if (temp == "topleft")
		{
			pivot = userinterface::UIElement::piv_TopLeft;

		}
		else if (temp == "top")
		{
			pivot = userinterface::UIElement::piv_Top;

		}
		else if (temp == "topright")
		{
			pivot = userinterface::UIElement::piv_TopRight;

		}
		else if (temp == "right")
		{
			pivot = userinterface::UIElement::piv_Right;

		}
		else if (temp == "botright")
		{
			pivot = userinterface::UIElement::piv_BotRight;

		}
		else if (temp == "bot")
		{
			pivot = userinterface::UIElement::piv_Bot;

		}
		else
		{
			pivot = userinterface::UIElement::piv_BotLeft;
		}

		textbox = new puppy::PivotTextBox(puppy::FontTable::getInstance()->getFont(font.c_str()), message.c_str(), width, height, pivot, align);


	if (JSONHAS("color")) {
		textbox->setColor(LOOKUP("color")[0], LOOKUP("color")[1], LOOKUP("color")[2]);
	}

	return textbox;
	}
}

#include "_Project\DisableAfterTime.h"
kitten::K_Component* getDisableAfterTime(nlohmann::json* p_jsonFile) {
	float time = p_jsonFile->operator[]("time");
	return new DisableAfterTime(time);
}

#include "UI\MainMenu\MainMenu.h"
kitten::K_Component* getMainMenu(nlohmann::json* p_jsonFile) {
	return new MainMenu();
}

#include "components/CustomDataComponent.h"
kitten::K_Component* getCustomDataComponent(nlohmann::json* p_jsonFile) {
	CustomDataComponent* comp = new CustomDataComponent();
	
	for (nlohmann::json::iterator attribute = p_jsonFile->begin(); attribute != p_jsonFile->end(); ++attribute)
	{
		if(attribute.key() == "name")
		{ }
		else if (attribute->is_boolean())
		{
			comp->m_boolMap[attribute.key()] = *attribute;
		}
		else if (attribute->is_number_integer())
		{
			comp->m_intMap[attribute.key()] = *attribute;
		}
		else if (attribute->is_number_float())
		{
			comp->m_floatMap[attribute.key()] = *attribute;
		}
		else 
		{
			comp->m_stringMap[attribute.key()] = *attribute;
		}
	}

	return comp;
}
#include "UI\CardContext.h"
kitten::K_Component* getCardContext(nlohmann::json* p_jsonFile) {
	return new CardContext();
}

#include "UI\ClickableCard.h"
kitten::K_Component* getClickableCard(nlohmann::json* p_jsonFile)
{
	bool isEnabledOnPause;
	SETOPTDEF(isEnabledOnPause, "enabledOnPause", false);
	return new userinterface::ClickableCard(isEnabledOnPause);
}

#include "kitten/QuadEdgeRenderable.h"
kitten::K_Component* getQuadEdgeRenderable(nlohmann::json* p_jsonFile)
{
	return new kitten::QuadEdgeRenderable();
}

#include "board/tile/TileDecoration.h"
kitten::K_Component* getTileDecoration(nlohmann::json* p_jsonFile)
{
	TileDecoration* d = new TileDecoration();

	if (JSONHAS("texture")) {
		std::vector<std::string> list;
		//assert(p_jsonfile["components"].is_array());
		for (nlohmann::json::iterator it = p_jsonFile->operator[]("texture").begin(); it != p_jsonFile->operator[]("texture").end(); ++it) {
			list.push_back(*it);
		}
		d->setTextureList(list);
	}

	if (JSONHAS("min_scale") && JSONHAS("max_scale"))
	{
		float x1 = p_jsonFile->operator[]("min_scale")[0];
		float y1 = p_jsonFile->operator[]("min_scale")[1];
		float z1 = p_jsonFile->operator[]("min_scale")[2];
		float x2 = p_jsonFile->operator[]("max_scale")[0];
		float y2 = p_jsonFile->operator[]("max_scale")[1];
		float z2 = p_jsonFile->operator[]("max_scale")[2];
		d->setScaleRange(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2));
	}
	if (JSONHAS("min_rotation") && JSONHAS("max_rotation"))
	{
		float x1 = p_jsonFile->operator[]("min_rotation")[0];
		float y1 = p_jsonFile->operator[]("min_rotation")[1];
		float z1 = p_jsonFile->operator[]("min_rotation")[2];
		float x2 = p_jsonFile->operator[]("max_rotation")[0];
		float y2 = p_jsonFile->operator[]("max_rotation")[1];
		float z2 = p_jsonFile->operator[]("max_rotation")[2];
		d->setRotationRange(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2));
	}
	if (JSONHAS("min_trans") && JSONHAS("max_trans"))
	{
		float x1 = p_jsonFile->operator[]("min_trans")[0];
		float y1 = p_jsonFile->operator[]("min_trans")[1];
		float z1 = p_jsonFile->operator[]("min_trans")[2];
		float x2 = p_jsonFile->operator[]("max_trans")[0];
		float y2 = p_jsonFile->operator[]("max_trans")[1];
		float z2 = p_jsonFile->operator[]("max_trans")[2];
		d->setTransRange(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2));
	}

	return d;
}

#include "kitten/SimpleQuadRenderable.h"
kitten::K_Component* getSimpleQuadRenderable(nlohmann::json* p_jsonFile) {
	std::string texturefilename;
	bool isStatic;

	SETOPTDEF(texturefilename, "texture", "");
	SETOPTDEF(isStatic, "static", false);

	return new kitten::SimpleQuadRenderable(texturefilename.c_str(), isStatic);
}

#include "UI\CardArt.h"
kitten::K_Component* getCardArt(nlohmann::json* p_jsonFile)
{
	std::string texture;
	SETOPTDEF(texture, "texture", "textures/ui/blankFrame.tga");

	return new userinterface::CardArt(texture);
}

#include "_Project\ShowLoadingOnClick.h"
kitten::K_Component* getShowLoadingOnClick(nlohmann::json* p_jsonFile) {
	return new ShowLoadingOnClick();
}

#include "kitten\lights\K_DirectionalLight.h"
kitten::K_Component* getDirectionalLight(nlohmann::json* p_jsonFile) {

	bool useCallbacks;
	glm::vec3 ambientColor;
	glm::vec3 directionalColor;

	SETOPTDEF(useCallbacks, "useCallbacks", false);
	
	if (JSONHAS("ambientColor"))
	{
		ambientColor = glm::vec3(LOOKUP("ambientColor")[0], LOOKUP("ambientColor")[1], LOOKUP("ambientColor")[2]);
	}

	if (JSONHAS("directionalColor"))
	{
		directionalColor = glm::vec3(LOOKUP("directionalColor")[0], LOOKUP("directionalColor")[1], LOOKUP("directionalColor")[2]);
	}

	return new kitten::K_DirectionalLight(ambientColor, directionalColor, useCallbacks);
}

#include "unitInteraction/CounterGetterButton.h"
kitten::K_Component* getCounterGetterButton(nlohmann::json* p_jsonFile) {
	std::string behavior;
	CounterGetterButton::ButtonBehavior b;
	SETOPT(behavior, "behavior");
	
	if (behavior == "plus")
		b = CounterGetterButton::plus;
	else if (behavior == "minus")
		b = CounterGetterButton::minus;
	else if (behavior == "check")
		b = CounterGetterButton::check;
	else
		b = CounterGetterButton::cancel;

	return new CounterGetterButton(b);
}

#include "unitInteraction/CounterGetterController.h"
kitten::K_Component* getCounterGetterController(nlohmann::json* p_jsonFile) {
	return new CounterGetterController();
}

std::map<std::string, kitten::K_Component* (*)(nlohmann::json* p_jsonFile)> jsonComponentMap;
void setupComponentMap() {
	jsonComponentMap["MoveByMouseRightClickDrag"] = &getMoveByMouseRightClickDrag;
	jsonComponentMap["ZoomByMouseWheel"] = &getZoomByMouseWheel;
	jsonComponentMap["Camera"] = &getCamera;
	jsonComponentMap["QuadRenderable"] = &getQuadRenderable;
	jsonComponentMap["QuadRenderableRepeat"] = &getQuadRenderableRepeat;
	jsonComponentMap["CubeRenderable"] = &getCubeRenderable;
	jsonComponentMap["GrassLandInfo"] = &getGrassLandInfo;
	jsonComponentMap["DebugPrintOnce"] = &getDebugPrintOnce;
	jsonComponentMap["PrintWhenClicked"] = &getPrintWhenClicked;
	jsonComponentMap["DestroyOnClick"] = &getDestroyOnClick;
	jsonComponentMap["ClickableBox"] = &getClickableBox;
	jsonComponentMap["AudioSource"] = &getAudioSource;
	jsonComponentMap["AudioListener"] = &getAudioListener;
	jsonComponentMap["PlaySoundOnKeyPress"] = &getPlaySoundOnKeyPress;
	jsonComponentMap["PlaySoundOnStart"] = &getPlaySoundOnStart;
	jsonComponentMap["ToggleSoundOnKeyPress"] = &getToggleSoundOnStart;
	jsonComponentMap["VolumeAdjustOnKeysPressed"] = &getVolumeAdjustOnKeysPressed;
	jsonComponentMap["UIFrame"] = &getUIFrame;
	jsonComponentMap["UnitMove"] = &getUnitMove;
	jsonComponentMap["ManipulateTileOnClick"] = &getManipulateTileOnClick;
	jsonComponentMap["UseAbilityWhenClicked"] = &getUseAbilityWhenClicked;
	jsonComponentMap["SendSelfOnClick"] = &getSendSelfOnClick;
	jsonComponentMap["FPSCalc"] = &getFPSCalc;
	jsonComponentMap["TrackerBlock"] = &getTrackerBlock;
	jsonComponentMap["TrackerBlockClickable"] = &getTrackerBlockClickable;
	jsonComponentMap["PointerUI"] = &getPointerUI;
	jsonComponentMap["UnitGraphic"] = &getUnitGraphic;
	jsonComponentMap["TextBox"] = &getTextBox;
	jsonComponentMap["ToggleEnabledOnKeyPress"] = &getToggleEnabledOnKeyPress;
	jsonComponentMap["TileInfoDisplayOnKeyPress"] = &getTileInfoDisplayOnKeyPress;
	jsonComponentMap["BoardCreator"] = &getBoardCreator;
	jsonComponentMap["ToggleStringInputOnKeyPress"] = &getToggleStringInputOnKeyPress;
	jsonComponentMap["StringInputDisplay"] = &getStringInputDisplay;
	jsonComponentMap["SelectAbility"] = &getSelectAbility;
	jsonComponentMap["PowerTracker"] = &getPowerTracker;
	jsonComponentMap["Highlighter"] = &getHighlighter;
	jsonComponentMap["HandFrame"] = &getHandFrame;
	jsonComponentMap["CardUIO"] = &getCardUIO;
	jsonComponentMap["ClickableFrame"] = &getClickableFrame;
	jsonComponentMap["ClickableUI"] = &getClickableUI;
	jsonComponentMap["TileInfo"] = &getTileInfo;
	jsonComponentMap["SpawnUnitOnKeyPress"] = &getSpawnUnitOnKeyPress;
	jsonComponentMap["NetworkingConsoleMenu"] = &getNetworkingConsoleMenu;
	jsonComponentMap["ConnectToHost"] = &getConnectToHost;
	jsonComponentMap["ChangeSceneOnClick"] = &getChangeSceneOnClick;
	jsonComponentMap["SetupDeckEditOnSceneChange"] = &getSetupDeckEditOnSceneChange;
	jsonComponentMap["UniversalPfx"] = &getUniversalPfx;
	jsonComponentMap["K_ParticleSystem"] = &getKParticleSystem;
	jsonComponentMap["ToggleParticleSystemOnKeyPress"] = &getToggleParticleSystemOnKeyPress;
	jsonComponentMap["SpriteAnimator"] = &getSpriteAnimator;
	jsonComponentMap["SpriteRenderable"] = &getSpriteRenderable;
	jsonComponentMap["DisplayFrameSetChangeOnClick"] = &getDisplayFrameSetChangeOnClick;
	jsonComponentMap["DisplayFramePickerOnClick"] = &getDisplayFramePickerOnClick;
	jsonComponentMap["DecksDisplayFrame"] = &getDecksDisplayFrame;
	jsonComponentMap["DeckUnitsDisplayFrame"] = &getDeckUnitsDisplayFrame;
	jsonComponentMap["CommanderDisplayFrame"] = &getCommanderDisplayFrame;
	jsonComponentMap["UnitDisplayFrame"] = &getUnitDisplayFrame;
	jsonComponentMap["ClickableBoxRenderable"] = &getClickableBoxRenderable;
	jsonComponentMap["DeckInitializingComponent"] = &getDeckInitializingComponent;
	jsonComponentMap["DeckAlterationComponent"] = &getDeckAlterationComponent;
	jsonComponentMap["StartGameOnClick"] = &getStartGameOnClick;
	jsonComponentMap["DeckComponent"] = &getDeckComponent;
	jsonComponentMap["GameplayInit"] = &getGameplayInit;
	jsonComponentMap["NetworkJoinButton"] = &getNetworkJoinButton;
	jsonComponentMap["NetworkHostButton"] = &getNetworkHostButton;	
	jsonComponentMap["NetworkConnectButton"] = &getNetworkConnectButton;
	jsonComponentMap["TabMenu"] = &getTabMenu;
	jsonComponentMap["UIObject"] = &getUIObject;
	jsonComponentMap["ReturnToMainMenuButton"] = &getReturnToMainMenuButton;
	jsonComponentMap["UnitAura"] = &getUnitAura;
	jsonComponentMap["UnitSelect"] = &getUnitSelect;
	jsonComponentMap["SpriteGroup"] = &getSpriteGroup;
	jsonComponentMap["ClickableButton"] = &getClickableButton;
	jsonComponentMap["MainMenuBoard"] = &getMainMenuBoard;
	jsonComponentMap["CustomCursor"] = &getCustomCursor;
	jsonComponentMap["UIElement"] = &getUIElement;
	jsonComponentMap["ModelRenderable"] = &getModelRenderable;
	jsonComponentMap["UnitHealthBar"] = &getUnitHealthBar;
	jsonComponentMap["CameraMoveByEvent"] = &getCameraMoveByEvent;
	jsonComponentMap["LerpController"] = &getLerpController;
	jsonComponentMap["ExitGameButton"] = &getExitGameButton;
	jsonComponentMap["SetCommanderOnClick"] = &getSetCommanderOnClick;
	jsonComponentMap["SaveDeckOnClick"] = &getSaveDeckOnClick;
	jsonComponentMap["DiscardDeckOnClick"] = &getDiscardDeckOnClick;
	jsonComponentMap["AddUnitOnClick"] = &getAddUnitOnClick;
	jsonComponentMap["ChangeUnitOnClick"] = &getChangeUnitOnClick;
	jsonComponentMap["DeleteDeckOnClick"] = &getDeleteDeckOnClick;
	jsonComponentMap["AddNewDeckOnClick"] = &getAddNewDeckOnClick;
	jsonComponentMap["CommanderContext"] = &getCommanderContext;
	jsonComponentMap["ActionSelect"] = &getActionSelect;
	jsonComponentMap["DragNDrop"] = &getDragNDrop;
	jsonComponentMap["SpawnUnitOnDrop"] = &getSpawnUnitOnDrop;
	jsonComponentMap["CombatText"] = &getCombatText;
	jsonComponentMap["CustomDataComponent"] = &getCustomDataComponent;
	jsonComponentMap["DisableAfterTime"] = &getDisableAfterTime;
	jsonComponentMap["MainMenu"] = &getMainMenu;
	jsonComponentMap["ClickableCard"] = &getClickableCard;
	jsonComponentMap["CardContext"] = &getCardContext;
	jsonComponentMap["DrawCardOnClickUI"] = &getDrawCardOnClickUI;
	jsonComponentMap["QuadEdgeRenderable"] = &getQuadEdgeRenderable;
	jsonComponentMap["TileDecoration"] = &getTileDecoration;
	jsonComponentMap["SimpleQuadRenderable"] = &getSimpleQuadRenderable;
	jsonComponentMap["ShowLoadingOnClick"] = &getShowLoadingOnClick;
	jsonComponentMap["DirectionalLight"] = &getDirectionalLight;
	jsonComponentMap["PivotTextBox"] = &getPivotTextBox;
	jsonComponentMap["CardArt"] = &getCardArt;
	jsonComponentMap["CounterGetterController"] = &getCounterGetterController;
	jsonComponentMap["CounterGetterButton"] = &getCounterGetterButton;
}

kitten::K_Component* getRelatedComponentBy(nlohmann::json* p_jsonFile) {
	std::string key = p_jsonFile->operator[]("name");
	if (jsonComponentMap.find(key) != jsonComponentMap.end()) {
		kitten::K_Component* component = jsonComponentMap[key](p_jsonFile);
		if (JSONHAS("enabled")) {
			component->setEnabled((int)LOOKUP("enabled"));
		}
		return component;
	}
	return nullptr;
}
