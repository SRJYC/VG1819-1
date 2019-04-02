#include "Behavior.h"

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
}

Behavior::Behavior(const Behavior * source)
{
}

double Behavior::calculateWeight(AI::Model::TargetRange& p_target, AI::Model& p_data) 
{ 
	return 0; 
}

NearestEnemy::NearestEnemy(nlohmann::json & p_json) : Behavior(p_json)
{
}

double NearestEnemy::calculateWeight(AI::Model::TargetRange& p_target, AI::Model & p_data)
{
	auto enemyUnits = p_data.getOtherPlayerUnits();
	if (enemyUnits.size() == 0) return 0.0;
	unit::Unit* nearestEnemy = enemyUnits[0]; // replace with preference weighting here
	std::pair<int, int> enemyPos = nearestEnemy->getTile()->getComponent<TileInfo>()->getPos();
	int distance = std::abs(p_target.targetPlacement.first - enemyPos.first) + std::abs(p_target.targetPlacement.second - enemyPos.second);
	for (auto enemy : enemyUnits) {
		enemyPos = enemy->getTile()->getComponent<TileInfo>()->getPos();
		if ((std::abs(p_target.targetPlacement.first - enemyPos.first) + std::abs(p_target.targetPlacement.second - enemyPos.second)) < distance) {
			distance = std::abs(p_target.targetPlacement.first - enemyPos.first) + std::abs(p_target.targetPlacement.second - enemyPos.second);
			nearestEnemy = enemy;
		}
	}

	enemyPos = nearestEnemy->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_target.targetPlacement.first - enemyPos.first) + std::abs(p_target.targetPlacement.second - enemyPos.second)) / (double)(p_data.board.board.size() + p_data.board.board[0].size()));
}

LowestAttribEnemy::LowestAttribEnemy(nlohmann::json & p_json) : Behavior(p_json)
{
	attribute = p_json["attribute"].get<std::string>();
}

double LowestAttribEnemy::calculateWeight(AI::Model::TargetRange& p_target, AI::Model & p_data)
{
	auto enemyUnits = p_data.getOtherPlayerUnits();
	unit::Unit* lowestAttribUnit = enemyUnits[0];

	for (auto enemy : enemyUnits) {
		if (enemy->m_attributes[attribute] < lowestAttribUnit->m_attributes[attribute])
			lowestAttribUnit = enemy;
	}
	std::pair<int, int> enemyPos = lowestAttribUnit->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_target.targetPlacement.first - enemyPos.first) + std::abs(p_target.targetPlacement.second - enemyPos.second)) / (double)(p_data.board.board.size() + p_data.board.board[0].size()));
}

HighestAttribEnemy::HighestAttribEnemy(nlohmann::json & p_json)
{
	attribute = p_json["attribute"].get<std::string>();
}

double HighestAttribEnemy::calculateWeight(AI::Model::TargetRange & p_target, AI::Model & p_data)
{
	auto enemyUnits = p_data.getOtherPlayerUnits();
	unit::Unit* highestAttribUnit = enemyUnits[0];

	for (auto enemy : enemyUnits) {
		if (enemy->m_attributes[attribute] > highestAttribUnit->m_attributes[attribute])
			highestAttribUnit = enemy;
	}
	std::pair<int, int> enemyPos = highestAttribUnit->getTile()->getComponent<TileInfo>()->getPos();
	return 1 - ((double)(std::abs(p_target.targetPlacement.first - enemyPos.first) + std::abs(p_target.targetPlacement.second - enemyPos.second)) / (double)(p_data.board.board.size() + p_data.board.board[0].size()));
}

TileOwnership::TileOwnership(nlohmann::json & p_json)
{
	own = p_json["ownTeam"];
	passWeight = LOOKUPDEF("pass", 1);
	failWeight = LOOKUPDEF("fail", 0.25);
}

double TileOwnership::calculateWeight(AI::Model::TargetRange & p_target, AI::Model & p_data)
{
	if (p_data.board.board[p_target.targetPlacement.first][p_target.targetPlacement.second]->getOwnerId() == p_target.unit->m_clientId == own)
		if (subBehaviors.size() > 0)
			return passWeight * subBehaviors[0]->calculateWeight(p_target,p_data);
		else
			return passWeight;
	else
		if (subBehaviors.size() > 0)
			return failWeight * subBehaviors[0]->calculateWeight(p_target,p_data);
		else
			return failWeight;
}
