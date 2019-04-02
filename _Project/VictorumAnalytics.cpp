#include "VictorumAnalyitics.h"
#include "GameAnalytics.h"

VictorumAnalytics::VictorumAnalytics()
{

#ifdef DEBUG
	gameanalytics::GameAnalytics::setEnabledInfoLog(true);
	gameanalytics::GameAnalytics::setEnabledVerboseLog(true);
#else
	gameanalytics::GameAnalytics::setEnabledInfoLog(false);
#endif

	// Uncomment below to not send telemetry 
	//gameanalytics::GameAnalytics::setEnabledEventSubmission(false);

	// Version number Not set in data to avoid 
	// our large playerbase hacking the game (changing a plaintext file) 
	// and changing the version
	gameanalytics::GameAnalytics::configureBuild("0.2.0");

	gameanalytics::GameAnalytics::initialize("55cd478182ed717f7652a3e1f2c96fe7", "5ea8ad5ff6c3099ed3fb1e2c33e443c769552d93");

	gameanalytics::GameAnalytics::startSession();
}

VictorumAnalytics::~VictorumAnalytics()
{
	gameanalytics::GameAnalytics::endSession();
	gameanalytics::GameAnalytics::onQuit();
}

void VictorumAnalytics::sendDesignEvent(const std::string& p_event)
{
	gameanalytics::GameAnalytics::addDesignEvent(p_event);
}

void VictorumAnalytics::sendDesignEvent(const std::string& p_event, const double& p_value)
{
	gameanalytics::GameAnalytics::addDesignEvent(p_event, p_value);
}

void VictorumAnalytics::sendErrorEvent(const std::string& p_message)
{
	gameanalytics::GameAnalytics::addErrorEvent(gameanalytics::EGAErrorSeverity::Error, p_message);
}

void VictorumAnalytics::sendInfoEvent(const std::string& p_message)
{
	gameanalytics::GameAnalytics::addErrorEvent(gameanalytics::EGAErrorSeverity::Info, p_message);
}

void VictorumAnalytics::sendDebugEvent(const std::string& p_message)
{
	gameanalytics::GameAnalytics::addErrorEvent(gameanalytics::EGAErrorSeverity::Debug, p_message);
}

void VictorumAnalytics::onMultiplayerGameStart()
{
	sendDesignEvent("Multiplayer:Start");
	
	// Send information about deck

}