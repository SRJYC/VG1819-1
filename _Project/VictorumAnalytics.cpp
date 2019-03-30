#include "VictorumAnalyitics.h"
#include "GameAnalytics.h"

VictorumAnalytics::VictorumAnalytics()
{
	gameanalytics::GameAnalytics::setEnabledInfoLog(true);
	gameanalytics::GameAnalytics::setEnabledVerboseLog(true);

	// Version number Not set in data to avoid 
	// our large playerbase hacking the game (changing a plaintext file) 
	// and changing the version
	gameanalytics::GameAnalytics::configureBuild("0.2.0");

	gameanalytics::GameAnalytics::initialize("55cd478182ed717f7652a3e1f2c96fe7", "5ea8ad5ff6c3099ed3fb1e2c33e443c769552d93");

	gameanalytics::GameAnalytics::startSession();

	gameanalytics::GameAnalytics::addDesignEvent("TestEvent", 1);
}

VictorumAnalytics::~VictorumAnalytics()
{
	gameanalytics::GameAnalytics::endSession();
	gameanalytics::GameAnalytics::onQuit();
}