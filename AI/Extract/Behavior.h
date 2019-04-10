#pragma once
#include "AI/model.h"
#include "kibble/json/jsonextra.hpp"

struct Behavior {
	std::vector<Behavior*> subBehaviors;
	double internalMultiplier = 1;
	Behavior(){}
	Behavior(nlohmann::json& p_json);
	Behavior(const Behavior* source);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo);
};

struct ForEachTarget : public Behavior {

	ForEachTarget(nlohmann::json& p_json);
	virtual double calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct NearestUnit : public Behavior {
	int targetType= 0; // 0 for all, 1 for enemy , 2 for Ally
	NearestUnit(){}
	NearestUnit(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct LowestAttribUnit : public Behavior {
	int targetType = 0; // 0 for all, 1 for enemy , 2 for Ally
	std::string attribute;

	LowestAttribUnit(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct HighestAttribUnit : public Behavior {
	int targetType = 0; // 0 for all, 1 for enemy , 2 for Ally
	std::string attribute;

	HighestAttribUnit(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct TileOwnership : public Behavior {
	bool own;
	double passWeight, failWeight;
	TileOwnership(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct TargetAlignment : public Behavior {
	double sameTeamWeight, differentTeamWeight;
	TargetAlignment(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct TileType : public Behavior {
	LandInformation::TileType type;
	double sameTeamWeight, differentTeamWeight;
	TileType(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct OwnAttribute : public Behavior {
	std::string attribute;
	double weightRangeFrom, weightRangeTo;
	int attributeFrom, attributeTo;
	OwnAttribute(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct TargetAttribute : public Behavior {
	std::string attribute;
	double weightRangeFrom, weightRangeTo;
	int attributeFrom, attributeTo;
	TargetAttribute(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct TargetAttributePercentage : public Behavior {
	std::string attribute;
	double weightRangeFrom, weightRangeTo;
	double attributeFrom, attributeTo;
	TargetAttributePercentage(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct RepeatedTarget : public Behavior {
	double newWeight;
	RepeatedTarget(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct SelfHasStatus : public Behavior {
	std::string status;
	int wIfHas = 0, wIfNot = 1;
	SelfHasStatus(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

struct TargetHasStatus : public Behavior {
	std::string status;
	int wIfHas = 0, wIfNot = 1;
	TargetHasStatus(nlohmann::json& p_json);
	virtual double calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) override;
};

void setupBehaviors();
Behavior* generateBehavior(nlohmann::json& p_json);
