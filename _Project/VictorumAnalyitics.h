#pragma once
#include "kitten\K_Component.h"

#include <string>

class VictorumAnalytics : public kitten::K_Component
{
private:
public:
	VictorumAnalytics();
	~VictorumAnalytics();

	static void sendDesignEvent(const std::string& p_event);
	static void sendDesignEvent(const std::string& p_event, const double& p_value);

	static void sendErrorEvent(const std::string& p_message);
	static void sendInfoEvent(const std::string& p_message);
	static void sendDebugEvent(const std::string& p_message);
};
