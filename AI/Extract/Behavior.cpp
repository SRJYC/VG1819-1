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
	ADDTOMAP(NearestEnemy);
	ADDTOMAP(LowestAttribEnemy);
	ADDTOMAP(HighestAttribEnemy);
	ADDTOMAP(TileOwnership);
	ADDTOMAP(TargetAlignment);
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

double Behavior::calculateWeight(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo) 
{ 
	if (this->subBehaviors.size() > 0) {
		double total = 0;
		for (auto behavior : subBehaviors) {
			total += behavior->calculateWeight(p_retainedInfo, p_passedInfo, p_targgetingInfo);
		}
		return this->internalMultiplier * this->calculateMultiplier(p_retainedInfo, p_passedInfo, p_targgetingInfo) * total;
	}
	else
		return this->internalMultiplier * this->calculateMultiplier(p_retainedInfo,p_passedInfo,p_targgetingInfo); 
}

double Behavior::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo & p_targgetingInfo)
{
	return 1;
}

NearestEnemy::NearestEnemy(nlohmann::json & p_json) : Behavior(p_json)
{
}

double NearestEnemy::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo)
{
	auto enemyUnits = p_retainedInfo.enemyUnits;
	if (enemyUnits.size() == 0) return 0.0;
	unit::Unit* nearestEnemy = enemyUnits[0]; // replace with preference weighting here
	std::pair<int, int> enemyPos = nearestEnemy->getTile()->getComponent<TileInfo>()->getPos();
	int distance = std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second);
	for (auto enemy : enemyUnits) {
		enemyPos = enemy->getTile()->getComponent<TileInfo>()->getPos();
		if ((std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second)) < distance) {
			distance = std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second);
			nearestEnemy = enemy;
		}
	}

	enemyPos = nearestEnemy->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second)) 
		/ (double)(p_retainedInfo.board.tile.size() + p_retainedInfo.board.tile[0].size()));
}

LowestAttribEnemy::LowestAttribEnemy(nlohmann::json & p_json) : Behavior(p_json)
{
	attribute = p_json["attribute"].get<std::string>();
}

double LowestAttribEnemy::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo)
{
	auto enemyUnits = p_retainedInfo.enemyUnits;
	if (enemyUnits.size() == 0) return 0.0;
	unit::Unit* lowestAttribUnit = enemyUnits[0];

	for (auto enemy : enemyUnits) {
		if (enemy->m_attributes[attribute] < lowestAttribUnit->m_attributes[attribute])
			lowestAttribUnit = enemy;
	}
	std::pair<int, int> enemyPos = lowestAttribUnit->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_passedInfo.curPos.first - enemyPos.first) + std::abs(p_passedInfo.curPos.second - enemyPos.second))
		/ (double)(p_retainedInfo.board.tile.size() + p_retainedInfo.board.tile[0].size()));
}

HighestAttribEnemy::HighestAttribEnemy(nlohmann::json & p_json) : Behavior(p_json)
{
	attribute = p_json["attribute"].get<std::string>();
}

double HighestAttribEnemy::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo)
{
	auto enemyUnits = p_retainedInfo.enemyUnits;
	if (enemyUnits.size() == 0) return 0.0;
	unit::Unit* highestAttribUnit = enemyUnits[0];

	for (auto enemy : enemyUnits) {
		if (enemy->m_attributes[attribute] > highestAttribUnit->m_attributes[attribute])
			highestAttribUnit = enemy;
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

double TileOwnership::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo& p_targgetingInfo)
{
	if (p_retainedInfo.board.tile[p_targgetingInfo.targets[0].first][p_targgetingInfo.targets[0].second]->getOwnerId() == p_retainedInfo.source.clientId == own)	
		return passWeight;
	else
		return failWeight;
}

TargetAlignment::TargetAlignment(nlohmann::json & p_json) : Behavior(p_json)
{
	sameTeamWeight = LOOKUPDEF("sameTeam", -1);
	differentTeamWeight = LOOKUPDEF("differentTeam", 1);
}

double TargetAlignment::calculateMultiplier(const AI::retainedInfo & p_retainedInfo, const AI::passedInfo & p_passedInfo, const AI::targettingInfo & p_targgetingInfo)
{
	if (p_retainedInfo.board.unit[p_targgetingInfo.targets[0].first][p_targgetingInfo.targets[0].second]->m_clientId == p_retainedInfo.source.clientId)
		return sameTeamWeight;
	else
		return differentTeamWeight;
}
