#include "ComponentDataType.hpp"
#include <string>

#include "_Project/MoveByMouseRightClickDrag.h"
kitten::K_Component* getMoveByMouseRightClickDrag(nlohmann::json* p_jsonFile) {
	float speed = 0.005f;

	if (p_jsonFile->find("speed") != p_jsonFile->end()) {
		speed = p_jsonFile->operator[]("speed");
	}

	return new MoveByMouseRightClickDrag(speed);
}

#include "_Project/ZoomByMouseWheel.h"
kitten::K_Component* getZoomByMouseWheel(nlohmann::json* p_jsonFile) {
	float speed = 1.0f;
	int minFOV = 1, maxFOV = 90;

	if (p_jsonFile->find("speed") != p_jsonFile->end()) {
		speed = p_jsonFile->operator[]("speed");
	}

	if (p_jsonFile->find("minfov") != p_jsonFile->end()) {
		minFOV = p_jsonFile->operator[]("minfov");
	}

	if (p_jsonFile->find("maxfov") != p_jsonFile->end()) {
		maxFOV = p_jsonFile->operator[]("maxfov");
	}

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

	if (p_jsonFile->find("texture") != p_jsonFile->end()) {
		texturefilename = p_jsonFile->operator[]("texture");
	}
	else {
		texturefilename = "textures/tiles/MISSING.tga";
	}

	return new kitten::CubeRenderable(texturefilename.c_str());
}

#include "kitten/QuadRenderable.h"
kitten::K_Component* getQuadRenderable(nlohmann::json* p_jsonFile) {
	std::string texturefilename = "textures/tiles/MISSING.tga";
	bool isStatic = false;

	if (p_jsonFile->find("texture") != p_jsonFile->end()) {
		texturefilename = p_jsonFile->operator[]("texture");
	}

	if (p_jsonFile->find("static") != p_jsonFile->end()) {
		isStatic = p_jsonFile->operator[]("static");
	}

	return new kitten::QuadRenderable(texturefilename.c_str(), isStatic);
}

#include "gameworld/GrassLandInfoComponent.h"
kitten::K_Component* getGrassLandInfo(nlohmann::json* p_jsonFile) {
	return new gameworld::GrasslandInfoComponent();
}

#include "_Project/DebugPrintOnce.h"
kitten::K_Component* getDebugPrintOnce(nlohmann::json* p_jsonFile){
	std::string message = "Default Message";

	if (p_jsonFile->find("message") != p_jsonFile->end()) {
		message = p_jsonFile->operator[]("message");
	}

	return new DebugPrintOnce(message);
}

#include "board/clickable/PrintWhenClicked.h"
kitten::K_Component* getPrintWhenClicked(nlohmann::json* p_jsonFile) {
	std::string message = "Default Message";

	if (p_jsonFile->find("message") != p_jsonFile->end()) {
		message = p_jsonFile->operator[]("message");
	}

	return new PrintWhenClicked(message);
}

#include "kitten\mouse picking\ClickableBox.h"
kitten::K_Component* getClickableBox(nlohmann::json* p_jsonFile) {

	glm::vec3 minPoint, maxPoint;

	if (p_jsonFile->find("minpoint") != p_jsonFile->end()) {
		minPoint = glm::vec3(p_jsonFile->operator[]("minpoint")[0], p_jsonFile->operator[]("minpoint")[1], p_jsonFile->operator[]("minpoint")[2]);
	}

	if (p_jsonFile->find("maxpoint") != p_jsonFile->end()) {
		minPoint = glm::vec3(p_jsonFile->operator[]("maxpoint")[0], p_jsonFile->operator[]("maxpoint")[1], p_jsonFile->operator[]("maxpoint")[2]);
	}

	return new kitten::ClickableBox(minPoint, maxPoint);
}

#include "_Project\DestroyOnClick.h"
kitten::K_Component* getDestroyOnClick(nlohmann::json* p_jsonFile){
	return new DestroyOnClick();
}


#include "board/component/BoardCreator.h"
kitten::K_Component* getBoardCreator(nlohmann::json* p_jsonFile){
	return new BoardCreator();
}

#include "kitten\audio\AudioSource.h"
kitten::K_Component* getAudioSource(nlohmann::json* p_jsonFile) {

	std::string pathToClip;
	bool is3D, enableEffects, causesDuck = false, getsDucked = false;

	if (p_jsonFile->find("clippath") != p_jsonFile->end()) {
		pathToClip = p_jsonFile->operator[]("clippath");
	}

	if (p_jsonFile->find("is3d") != p_jsonFile->end()) {
		is3D = p_jsonFile->operator[]("is3d");
	}

	if (p_jsonFile->find("enableeffects") != p_jsonFile->end()) {
		enableEffects = p_jsonFile->operator[]("enableeffects");
	}

	if (p_jsonFile->find("causesduck") != p_jsonFile->end()) {
		causesDuck = p_jsonFile->operator[]("causesduck");
	}

	if (p_jsonFile->find("getsducked") != p_jsonFile->end()) {
		getsDucked = p_jsonFile->operator[]("getsducked");
	}

	kitten::AudioSource* toReturn = new kitten::AudioSource(pathToClip, is3D, enableEffects, causesDuck, getsDucked);

	if (is3D) {
		
		if (p_jsonFile->find("mindistance") != p_jsonFile->end()) {
			float minDistance = p_jsonFile->operator[]("mindistance");
			toReturn->setMinDistance(minDistance);
		}

		if (p_jsonFile->find("maxdistance") != p_jsonFile->end()) {
			float maxDistance = p_jsonFile->operator[]("maxdistance");
			toReturn->setMaxDistance(maxDistance);
		}
	}

	if (p_jsonFile->find("loop") != p_jsonFile->end()) {
		if (p_jsonFile->operator[]("loop")) {
			toReturn->setLooped(true);
		}
	}

	if (p_jsonFile->find("volume") != p_jsonFile->end()) {
		float volume = p_jsonFile->operator[]("volume");
		toReturn->setVolume(volume);
	}

	if (p_jsonFile->find("playprogress") != p_jsonFile->end()) {
		float progress = p_jsonFile->operator[]("playprogress");
		toReturn->setPlayProgress(progress);
	}

	if (enableEffects) {
		//Effects parsing here

#define jsonHas(str) innerJson.find(str) != innerJson.end()
#define setVar(varName,jsonName) if(jsonHas(jsonName)) varName = innerJson.operator[](jsonName)

		auto sfx = toReturn->getSFXControl();

		if (p_jsonFile->find("choruseffect") != p_jsonFile->end()) {
			irrklang::ik_f32 wetDryMix = 50, depth = 10, feedback = 25, frequency = 1.1;
			bool sinusWaveForm = true;
			irrklang::ik_f32 delay = 16, phase = 90;
			
			auto innerJson = p_jsonFile->operator[]("choruseffect");

			setVar(wetDryMix, "wetdrymix");
			setVar(depth, "depth");
			setVar(feedback, "feedback");
			setVar(frequency, "frequency");
			setVar(sinusWaveForm, "sinuswaveform");
			setVar(delay, "delay");
			setVar(phase, "phase");

			sfx->setChorusEffect(wetDryMix, depth, feedback, frequency, sinusWaveForm, delay, phase);
		}

		if (p_jsonFile->find("compressoreffect") != p_jsonFile->end()) {
			irrklang::ik_f32 gain = 0, attack = 10, release = 200, threshold = -20,
				ratio = 3, preDelay = 4;

			auto innerJson = p_jsonFile->operator[]("compressoreffect");

			setVar(gain, "gain");
			setVar(attack, "attack");
			setVar(release, "release");
			setVar(threshold, "threshold");
			setVar(ratio, "ratio");
			setVar(preDelay, "predelay");

			sfx->setCompressorEffect(gain, attack, release, threshold, ratio, preDelay);
		}

		if (p_jsonFile->find("distortioneffect") != p_jsonFile->end()) {
			irrklang::ik_f32 gain = -18, edge = 15, postEQCenterFrequency = 2400,
				postEQBandwidth = 2400, preLowpassCutoff = 8000;

			auto innerJson = p_jsonFile->operator[]("distortioneffect");

			setVar(gain, "gain");
			setVar(edge, "edge");
			setVar(postEQCenterFrequency, "posteqcenterfrequency");
			setVar(postEQBandwidth, "posteqbandwidth");
			setVar(preLowpassCutoff, "prelowpasscutoff");

			sfx->setDistortionEffect(gain, edge, postEQCenterFrequency, postEQBandwidth, preLowpassCutoff);
		}

		if (p_jsonFile->find("echoeffect") != p_jsonFile->end()) {
			irrklang::ik_f32 wetDryMix = 50, feedback = 50,
				leftDelay = 500, rightDelay = 500, panDelay = 0;

			auto innerJson = p_jsonFile->operator[]("echoeffect");

			setVar(wetDryMix, "wetdrymix");
			setVar(feedback, "feedback");
			setVar(leftDelay, "leftdelay");
			setVar(rightDelay, "rightdelay");
			setVar(panDelay, "pandelay");

			sfx->setEchoEffect(wetDryMix, feedback, leftDelay, rightDelay, panDelay);
		}

		if (p_jsonFile->find("flangereffect") != p_jsonFile->end()) {
			irrklang::ik_f32 wetDryMix = 50, depth = 100,
				feedback = -50, frequency = 0.25f, delay = 2;
			irrklang::ik_s32 phase = 0;
			bool triangleWaveForm = true;

			auto innerJson = p_jsonFile->operator[]("flangereffect");

			setVar(wetDryMix, "wetdrymix");
			setVar(depth, "depth");
			setVar(feedback, "feedback");
			setVar(frequency, "frequency");
			setVar(delay, "delay");
			setVar(phase, "phase");
			setVar(triangleWaveForm, "trianglewaveform");

			sfx->setFlangerEffect(wetDryMix, depth, feedback, frequency, triangleWaveForm, delay, phase);
		}

		if (p_jsonFile->find("gargleeffect") != p_jsonFile->end()) {
			irrklang::ik_s32 rateHz = 20;
			bool sinusWaveForm = true;

			auto innerJson = p_jsonFile->operator[]("gargleffect");

			setVar(rateHz, "ratehz");
			setVar(sinusWaveForm, "sinuswaveform");

			sfx->setGargleEffect(rateHz, sinusWaveForm);
		}

		if (p_jsonFile->find("3dreverbeffect") != p_jsonFile->end()) {
			irrklang::ik_s32 room = -1000, roomHF = -100,
				reflections = -2602, reverb = 200;
			irrklang::ik_f32 roomRolloffFactor = 0, decayTime = 1.49f, decayHFRatio = 0.83f,
				reflectionsDelay = 0.007f, reverbDelay = 0.011f, diffusion = 100,
				density = 100, hfReference = 5000;

			auto innerJson = p_jsonFile->operator[]("3dreverbeffect");

			setVar(room, "room");
			setVar(roomHF, "roomHF");
			setVar(reflections, "reflections");
			setVar(reverb, "reverb");
			setVar(roomRolloffFactor, "roomrollofffactor");
			setVar(decayTime, "decaytime");
			setVar(decayHFRatio, "decayhfratio");
			setVar(reflectionsDelay, "reflectionsdelay");
			setVar(reverbDelay, "reverbdelay");
			setVar(diffusion, "diffusion");
			setVar(density, "density");
			setVar(hfReference, "hfReference");

			sfx->set3DReverbEffect(room, roomHF, roomRolloffFactor, decayTime, decayHFRatio, reflections, reflectionsDelay, reverb, reverbDelay, diffusion, density, hfReference);
		}

		if (p_jsonFile->find("parameqeffect") != p_jsonFile->end()) {
			irrklang::ik_f32 center = 8000, bandwidth = 12,
				gain = 0;

			auto innerJson = p_jsonFile->operator[]("parameqeffect");

			setVar(center, "center");
			setVar(bandwidth, "bandwidth");
			setVar(gain, "gain");

			sfx->setParamEqEffect(center, bandwidth, gain);
		}

		if (p_jsonFile->find("wavesreverbeffect") != p_jsonFile->end()) {
			irrklang::ik_f32 gain = 0, reverbMix = 0,
				reverbTime = 1000, highFreqRTRatio = 0.001f;

			auto innerJson = p_jsonFile->operator[]("wavesreverbeffect");

			setVar(gain, "gain");
			setVar(reverbMix, "reverbmix");
			setVar(reverbTime, "reverbtime");
			setVar(highFreqRTRatio, "highfreqrtratio");

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

	if (p_jsonFile->find("key") != p_jsonFile->end()) {
		std::string strKey = p_jsonFile->operator[]("key");
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

	if (p_jsonFile->find("key") != p_jsonFile->end()) {
		std::string strKey = p_jsonFile->operator[]("key");
		key = strKey[0];
	}

	return new ToggleSoundOnKeyPress(key);
}

#include "_Project\VolumeAdjustOnKeysPressed.h"
kitten::K_Component* getVolumeAdjustOnKeysPressed(nlohmann::json* p_jsonFile) {
	char increaseKey, decreaseKey;
	float changeAmount;

	if (p_jsonFile->find("increasekey") != p_jsonFile->end()) {
		std::string strKey = p_jsonFile->operator[]("increasekey");
		increaseKey = strKey[0];
	}

	if (p_jsonFile->find("decreasekey") != p_jsonFile->end()) {
		std::string strKey = p_jsonFile->operator[]("decreasekey");
		decreaseKey = strKey[0];
	}

	if (p_jsonFile->find("changeamount") != p_jsonFile->end()) {
		changeAmount = p_jsonFile->operator[]("changeamount");
	}

	return new VolumeAdjustOnKeysPressed(increaseKey,decreaseKey,changeAmount);
}

#include "userinterface/UIFrame.h"
kitten::K_Component* getUIFrame(nlohmann::json* p_jsonFile) {
	std::string texture = "textures/ui/blankFrame.tga";

	if (p_jsonFile->find("texture") != p_jsonFile->end()) {
		texture = p_jsonFile->operator[]("texture");
	}

	return new userinterface::UIFrame(texture.c_str());
}

#include "unit/unitComponent/UnitMove.h"
kitten::K_Component* getUnitMove(nlohmann::json* p_jsonFile) {
	return new unit::UnitMove();
}

#include "unit/unitComponent/UnitClickable.h"
kitten::K_Component* getUnitClickable(nlohmann::json* p_jsonFile) {
	return new unit::UnitClickable();
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
	std::string texture = "textures/unit/Default.tga";
	unit::UnitSize size = unit::point;

	if (p_jsonFile->find("texture") != p_jsonFile->end()) {
		texture = p_jsonFile->operator[]("texture");
	}

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
	std::string font = "../fonts/common_consolas.fnt", message = "DEFAULT TEXT";
	float width = 500, height= 500;
	puppy::TextBox* textbox;

	if (p_jsonFile->find("font") != p_jsonFile->end()) {
		font = p_jsonFile->operator[]("font");
	}

	if (p_jsonFile->find("message") != p_jsonFile->end()) {
		message = p_jsonFile->operator[]("message");
	}

	if (p_jsonFile->find("width") != p_jsonFile->end()) {
		width = p_jsonFile->operator[]("width");
	}

	if (p_jsonFile->find("height") != p_jsonFile->end()) {
		height = p_jsonFile->operator[]("height");
	}

	textbox = new puppy::TextBox(puppy::FontTable::getInstance()->getFont(font.c_str()), message.c_str(), width, height);

	if (p_jsonFile->find("color") != p_jsonFile->end()) {
		textbox->setColor(p_jsonFile->operator[]("color")[0], p_jsonFile->operator[]("color")[1], p_jsonFile->operator[]("color")[2]);
	}

	if (p_jsonFile->find("alignment") != p_jsonFile->end()) {
		std::string temp = p_jsonFile->operator[]("alignment");
		if (temp == "left")
			textbox->setAlignment(puppy::TextBox::Alignment::left);
		else if (temp == "right")
			textbox->setAlignment(puppy::TextBox::Alignment::right);
		else if (temp == "center")
			textbox->setAlignment(puppy::TextBox::Alignment::center);
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

std::map<std::string, kitten::K_Component* (*)(nlohmann::json* p_jsonFile)> jsonComponentMap;
void setupComponentMap() {
	jsonComponentMap["MoveByMouseRightClickDrag"] = &getMoveByMouseRightClickDrag;
	jsonComponentMap["ZoomByMouseWheel"] = &getZoomByMouseWheel;
	jsonComponentMap["Camera"] = &getCamera;
	jsonComponentMap["QuadRenderable"] = &getQuadRenderable;
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
	jsonComponentMap["UnitClickable"] = &getUnitClickable;
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
	jsonComponentMap["BoardCreator"] = &getBoardCreator;
	
}

kitten::K_Component* getRelatedComponentBy(nlohmann::json* p_jsonFile) {
	std::string key = p_jsonFile->operator[]("name");
	if (jsonComponentMap.find(key) != jsonComponentMap.end()) {
		return jsonComponentMap[key](p_jsonFile);
	}
	return nullptr;
}