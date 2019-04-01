#pragma once
#include "kitten/K_Common.h"
#include "UI/UIObject.h"
#include "puppy/Text/TextBox.h"
#include "kitten/event_system/EventManager.h"
#include "kibble/map/MapReader.h"

class MapListController : public kitten::K_Component
{
public:
	MapListController();
	~MapListController();

	bool hasUpdate() const override { return true; };
	void update() override;
	void start() override;

private:

	std::vector<kibble::MapReader::Map> m_mapList;
};
