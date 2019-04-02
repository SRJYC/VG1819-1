#pragma once
#include "kitten/K_Common.h"
#include "UI/UIObject.h"
#include "puppy/Text/TextBox.h"
#include "kitten/event_system/EventManager.h"
#include "kibble/map/MapReader.h"

class MapListController : public kitten::K_Component
{
public:
	MapListController(const std::string& p_randomImgPath, const std::string& p_randomDes,
		const std::string& p_upData, const std::string& p_downData, const std::string& p_mapData,
		float p_startX, float p_startY, float p_offsetY, int p_limitNum);
	~MapListController();

	bool hasUpdate() const override { return true; };
	void update() override;
	void start() override;

	void updateMapDetail(int p_mapId);

	void changeButton(int p_move);
private:
	std::vector<kibble::MapReader::Map> m_mapList;

	//info for default(random) map
	const std::string m_randomMapImagePath;
	const std::string m_randomMapImageDescription;

	//game object data file name
	const std::string m_mapButtonData;
	const std::string m_upButtonData;
	const std::string m_downButtonData;
	
	//limit number of map button
	const int m_limit;

	//start position for button
	const float m_startX;
	const float m_startY;

	//offset for button
	const float m_offsetY;

	//pointer to mapId that's showed in list
	int m_firstId;
	int m_lastId;

	std::vector<kitten::K_GameObject*> m_mapButtonList;
	kitten::K_GameObject* m_upButton;
	kitten::K_GameObject* m_downButton;

	void resetButton(int p_firstId);
};
