#include "Behavior.h"
#include "AI/controller.h"

#define LOOKUP(var) p_json[var]
#define LOOKUPDEF(var,def) ((p_json.find(var) != p_json.end())? p_json[var]: def)

template<class T>
Behavior* getBehavior(nlohmann::json& p_json) {
	return new T(p_json);
}
std::map<std::string, Behavior* (*)(nlohmann::json& p_json)> behaviorMap;
#define ADDTOMAP(type) behaviorMap[#type] = &getBehavior<type>
void setupBehaviors() {
	ADDTOMAP(Behavior);
	ADDTOMAP(NearestUnit);
	ADDTOMAP(LowestAttribUnit);
	ADDTOMAP(HighestAttribUnit);
	ADDTOMAP(TileOwnership);
	ADDTOMAP(TargetAlignment);
	ADDTOMAP(OwnAttribute);
	ADDTOMAP(TargetAttribute);
	ADDTOMAP(RepeatedTarget);
	ADDTOMAP(ForEachTarget);
	ADDTOMAP(SelfHasStatus);
	ADDTOMAP(TargetHasStatus);
}

Behavior * generateBehavior(nlohmann::json & p_json)
{
	return behaviorMap[p_json["name"].get<std::string>()](p_json);
}

Behavior::Behavior(nlohmann::json & p_json)
{
	if (p_json.find("subBehavior") != p_json.end()) {
		for (nlohmann::json::iterator it = p_json["subBehavior"].begin(); it != p_json["subBehavior"].end(); ++it) {
			subBehaviors.push_back(generateBehavior(*it));
		}
	}

	internalMultiplier = LOOKUPDEF("multiplier", 1);
}

Behavior::Behavior(const Behavior * source)
{

}

double Behavior::calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo) 
{ 
	if (this->subBehaviors.size() > 0) {
		double total = 0;
		for (auto behavior : subBehaviors) {
			total += behavior->calculateWeight(p_retainedInfo, p_passedInfo, p_targgetingInfo);
		}
		return this->internalMultiplier * this->calculateMultiplier(p_retainedInfo, p_passedInfo, p_targgetingInfo) * total;
	}
	else
		return this->internalMultiplier * this->calculateMultiplier(p_retainedInfo, p_passedInfo, p_targgetingInfo);
	
}

double Behavior::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo & p_targgetingInfo)
{
	return 1;
}

ForEachTarget::ForEachTarget(nlohmann::json & p_json) : Behavior(p_json)
{
}

double ForEachTarget::calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo)
{
	double overall = 0;
	for (int i = 0; i < p_targgetingInfo.targets.size(); i++) {
		p_targgetingInfo.focusedTarget = i;
		for (auto behavior : subBehaviors) {
			overall += behavior->calculateWeight(p_retainedInfo, p_passedInfo, p_targgetingInfo);
		}
	}
	p_targgetingInfo.focusedTarget = 0;

	return this->internalMultiplier * overall;
}


NearestUnit::NearestUnit(nlohmann::json & p_json) : Behavior(p_json)
{
	std::string target = LOOKUPDEF("target", "");
	if (target == "") this->targetType = 0;
	else if (target == "enemy") this->targetType = 1;
	else if (target == "ally") this->targetType = 2;
}

double NearestUnit::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo)
{
	std::vector<unit::Unit*> targetUnits;
	switch (targetType) {
	case 1: {
		targetUnits = p_retainedInfo.enemyUnits;
		break;
	}
	case 2: {
		targetUnits = p_retainedInfo.allyUnits;
		break;
	}
	default: {
		targetUnits = p_retainedInfo.allUnits;
		break;
	}
	}
	if (targetUnits.size() == 0) return 0.0;
	unit::Unit* nearestEnemy = targetUnits[0]; // replace with preference weighting here
	std::pair<int, int> enemyPos = nearestEnemy->getTile()->getComponent<TileInfo>()->getPos();
	int distance = std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second);
	for (auto target : targetUnits) {
		enemyPos = target->getTile()->getComponent<TileInfo>()->getPos();
		if ((std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second)) < distance) {
			distance = std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second);
			nearestEnemy = target;
		}
	}

	enemyPos = nearestEnemy->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second)) 
		/ (double)(p_retainedInfo.board.tile.size() + p_retainedInfo.board.tile[0].size()));
}

LowestAttribUnit::LowestAttribUnit(nlohmann::json & p_json) : Behavior(p_json)
{
	std::string target = LOOKUPDEF("target", "");
	if (target == "") this->targetType = 0;
	else if (target == "enemy") this->targetType = 1;
	else if (target == "ally") this->targetType = 2;
	attribute = p_json["attribute"].get<std::string>();
}

double LowestAttribUnit::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo)
{
	std::vector<unit::Unit*> targetUnits;
	switch (targetType) {
	case 1: {
		targetUnits = p_retainedInfo.enemyUnits;
		break;
	}
	case 2: {
		targetUnits = p_retainedInfo.allyUnits;
		break;
	}
	default: {
		targetUnits = p_retainedInfo.allUnits;
		break;
	}
	}
	if (targetUnits.size() == 0) return 0.0;
	unit::Unit* lowestAttribUnit = targetUnits[0];

	for (auto target : targetUnits) {
		if (target->m_attributes[attribute] < lowestAttribUnit->m_attributes[attribute])
			lowestAttribUnit = target;
	}
	std::pair<int, int> enemyPos = lowestAttribUnit->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second))
		/ (double)(p_retainedInfo.board.tile.size() + p_retainedInfo.board.tile[0].size()));
}

HighestAttribUnit::HighestAttribUnit(nlohmann::json & p_json) : Behavior(p_json)
{
	std::string target = LOOKUPDEF("target", "");
	if (target == "") this->targetType = 0;
	else if (target == "enemy") this->targetType = 1;
	else if (target == "ally") this->targetType = 2;
	attribute = p_json["attribute"].get<std::string>();
}

double HighestAttribUnit::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo)
{
	std::vector<unit::Unit*> targetUnits;
	switch (targetType) {
	case 1: {
		targetUnits = p_retainedInfo.enemyUnits;
		break;
	}
	case 2: {
		targetUnits = p_retainedInfo.allyUnits;
		break;
	}
	default: {
		targetUnits = p_retainedInfo.allUnits;
		break;
	}
	}
	if (targetUnits.size() == 0) return 0.0;
	unit::Unit* highestAttribUnit = targetUnits[0];

	for (auto target : targetUnits) {
		if (target->m_attributes[attribute] > highestAttribUnit->m_attributes[attribute])
			highestAttribUnit = target;
	}
	std::pair<int, int> enemyPos = highestAttribUnit->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second))
		/ (double)(p_retainedInfo.board.tile.size() + p_retainedInfo.board.tile[0].size()));
}

TileOwnership::TileOwnership(nlohmann::json & p_json) : Behavior(p_json)
{
	own = p_json["ownTeam"];
	passWeight = LOOKUPDEF("pass", 1);
	failWeight = LOOKUPDEF("fail", 0.25);
}

double TileOwnership::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo& p_targgetingInfo)
{
	if (p_retainedInfo.board.tile[p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].first][p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].second]->getOwnerId() == p_retainedInfo.source.clientId == own)
		return passWeight;
	else
		return failWeight;
}

TargetAlignment::TargetAlignment(nlohmann::json & p_json) : Behavior(p_json)
{
	sameTeamWeight = LOOKUPDEF("same", -1);
	differentTeamWeight = LOOKUPDEF("different", 1);
}

double TargetAlignment::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo & p_targgetingInfo)
{
	if (p_retainedInfo.board.unit[p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].first][p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].second]->m_clientId == p_retainedInfo.source.clientId)
		return sameTeamWeight;
	else
		return differentTeamWeight;
}

OwnAttribute::OwnAttribute(nlohmann::json & p_json) : Behavior(p_json)
{
	attribute = LOOKUP("attribute").get<std::string>();
	attributeFrom = LOOKUPDEF("attFrom", 0);
	attributeTo = LOOKUPDEF("attTo", 0);
	weightRangeFrom = LOOKUPDEF("wFrom", 0);
	weightRangeTo = LOOKUPDEF("wTo", 1);
}

double OwnAttribute::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo & p_targgetingInfo)
{
	int unitAttVal = p_retainedInfo.source.source->m_attributes[attribute];
	if (attributeFrom < attributeTo) {
		if (unitAttVal < attributeFrom) unitAttVal = attributeFrom;
		else if (unitAttVal > attributeTo) unitAttVal = attributeTo;
	}
	else if (unitAttVal > attributeFrom) unitAttVal = attributeFrom;
	else if (unitAttVal < attributeTo) unitAttVal = attributeTo;
	double percentage = (double)std::abs(unitAttVal- attributeFrom) / std::abs(attributeTo - attributeFrom);
	return (double)(weightRangeTo-weightRangeFrom) * percentage + weightRangeFrom;
}

TargetAttribute::TargetAttribute(nlohmann::json & p_json) : Behavior(p_json)
{
	attribute = LOOKUP("attribute").get<std::string>();
	attributeFrom = LOOKUPDEF("attFrom", 0);
	attributeTo = LOOKUPDEF("attTo", 0);
	weightRangeFrom = LOOKUPDEF("wFrom", 0);
	weightRangeTo = LOOKUPDEF("wTo", 1);
}

double TargetAttribute::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo & p_targgetingInfo)
{
	int unitAttVal = p_retainedInfo.board.unit[p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].first][p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].second]->m_attributes[attribute];
	if (attributeFrom < attributeTo) {
		if (unitAttVal < attributeFrom) unitAttVal = attributeFrom;
		else if (unitAttVal > attributeTo) unitAttVal = attributeTo;
	}
	else if (unitAttVal > attributeFrom) unitAttVal = attributeFrom;
	else if (unitAttVal < attributeTo) unitAttVal = attributeTo;

	double percentage = (double)std::abs(unitAttVal- attributeFrom) / std::abs(attributeTo - attributeFrom);
	return (double)(weightRangeTo-weightRangeFrom) * percentage + weightRangeFrom;
}

SelfHasStatus::SelfHasStatus(nlohmann::json & p_json) : Behavior(p_json)
{
	status = LOOKUP("status").get<std::string>();
	wIfHas = LOOKUPDEF("weightIfHas", 0);
	wIfNot = LOOKUPDEF("weightIfNot", 1);
}

double SelfHasStatus::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo & p_targgetingInfo)
{
	if (p_retainedInfo.source.source->getStatusContainer()->getStatus(status,"ANY") != nullptr) {
		return wIfHas;
	}
	else {
		return wIfNot;
	}
}

TargetHasStatus::TargetHasStatus(nlohmann::json & p_json) : Behavior(p_json)
{
	status = LOOKUP("status").get<std::string>();
	wIfHas = LOOKUPDEF("weightIfHas", 0);
	wIfNot = LOOKUPDEF("weightIfNot", 1);
}

double TargetHasStatus::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo & p_targgetingInfo)
{
	if (p_retainedInfo.board.unit[p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].first][p_targgetingInfo.targets[p_targgetingInfo.focusedTarget].second]->getStatusContainer()->getStatus(status, "ANY") != nullptr) {
		return wIfHas;
	}
	else {
		return wIfNot;
	}
}

RepeatedTarget::RepeatedTarget(nlohmann::json & p_json) : Behavior(p_json)
{
	newWeight = LOOKUPDEF("newWeight", 0.1);
}

double RepeatedTarget::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, AI::targettingInfo & p_targgetingInfo)
{
	if (p_targgetingInfo.lastTarget == p_targgetingInfo.targets[p_targgetingInfo.focusedTarget]) {
		return newWeight;
	}
	else {
		p_targgetingInfo.lastTarget = p_targgetingInfo.targets[p_targgetingInfo.focusedTarget];
		return 1;
	}
}
