#include "UnitTest.h"
#include "ability/node/AbilityNodeManager.h"
#include <iostream>

UnitTest* UnitTest::m_instance = nullptr;

UnitTest::UnitTest()
{
}

UnitTest * UnitTest::getInstance()
{
	return m_instance;
}

UnitTest * UnitTest::getInstanceSafe()
{
	if (m_instance == nullptr)
		m_instance = new UnitTest();
	return m_instance;
}

void UnitTest::test()
{
	//createInstance
	AbilityManager::createInstance();
	AbilityNodeManager::createInstance();

	std::string name = "testDummy";
	int HP = 3;
	int MV = 3;
	int IN = 3;
	int Cost=3;
	UnitSize size=point;

	std::vector<std::string> tags;
	tags.push_back("Dummy");
	tags.push_back("Neutral");

	std::vector<std::string> abilityDescription;
	abilityDescription.push_back("Heal");
	std::vector<std::string> statusDescription;

	UnitData* data = new UnitData(name,HP,MV,IN,Cost,size,tags,abilityDescription,statusDescription);

	Unit* dummy = UnitSpawn::getInstanceSafe()->spawnUnitFromData(data);
	Unit* dummyC = UnitSpawn::getInstance()->spawnCommanderFromData(data);

	dummy->m_attributes["HP"] = 0;
	std::cout << "Dummy with 0 HP" << std::endl;
	std::cout << std::endl;
	UnitMonitor::getInstanceSafe()->printUnit(dummy);

	dummy->useAbility(0);

	std::cout << std::endl;
	std::cout << "Dummy used heal (+4HP) on itself." << std::endl;
	UnitMonitor::getInstance()->printUnit(dummy);
}


