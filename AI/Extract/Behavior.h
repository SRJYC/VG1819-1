#pragma once
#include "AI/model.h"
#include "kibble/json/jsonextra.hpp"

struct Behavior {
	std::vector<Behavior*> subBehaviors;
	Behavior(){}
	Behavior(nlohmann::json& p_json);
	Behavior(const Behavior* source);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo);
};

struct NearestEnemy : public Behavior {

	NearestEnemy(){}
	NearestEnemy(nlohmann::json& p_json);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo) override;
};

struct LowestAttribEnemy : public Behavior {
	std::string attribute;

	LowestAttribEnemy(nlohmann::json& p_json);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo) override;
};

struct HighestAttribEnemy : public Behavior {
	std::string attribute;

	HighestAttribEnemy(nlohmann::json& p_json);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo) override;
};

struct TileOwnership : public Behavior {
	bool own;
	double passWeight, failWeight;
	TileOwnership(nlohmann::json& p_json);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo) override;
};

struct TargetAlignment : public Behavior {
	double sameTeamWeight, differentTeamWeight;
	TargetAlignment(nlohmann::json& p_json);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo) override;
};

void setupBehaviors();
Behavior* generateBehavior(nlohmann::json& p_json);
