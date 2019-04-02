#pragma once
#include "AI/model.h"
#include "kibble/json/jsonextra.hpp"

struct Behavior {
	std::vector<Behavior*> subBehaviors;
	Behavior(){}
	Behavior(nlohmann::json& p_json);
	Behavior(const Behavior* source);
	virtual double calculateWeight(AI::Model::TargetRange& p_target, AI::Model& p_data);
};

struct NearestEnemy : public Behavior {

	NearestEnemy(){}
	NearestEnemy(nlohmann::json& p_json);
	virtual double calculateWeight(AI::Model::TargetRange& p_target, AI::Model& p_data) override;
};

struct LowestAttribEnemy : public Behavior {
	std::string attribute;

	LowestAttribEnemy(nlohmann::json& p_json);
	virtual double calculateWeight(AI::Model::TargetRange& p_target, AI::Model& p_data) override;
};

struct HighestAttribEnemy : public Behavior {
	std::string attribute;

	HighestAttribEnemy(nlohmann::json& p_json);
	virtual double calculateWeight(AI::Model::TargetRange& p_target, AI::Model& p_data) override;
};

struct TileOwnership : public Behavior {
	bool own;
	double passWeight, failWeight;
	TileOwnership(nlohmann::json& p_json);
	virtual double calculateWeight(AI::Model::TargetRange& p_target, AI::Model& p_data) override;
};

void setupBehaviors();
Behavior* generateBehavior(nlohmann::json& p_json);
