#include "components\MainMenuBoard.h"

#include "kitten\K_GameObjectManager.h"
#include <time.h>
#include <algorithm>

#define SIMPLE_PRIEST "simple_units/Priest.txt"
#define SIMPLE_ARCHER "simple_units/Archer.txt"
#define SIMPLE_DUELIST "simple_units/Duelist.txt"
#define SIMPLE_ENGINEER "simple_units/Engineer.txt"
#define SIMPLE_NEWUNITOBJECT "simple_units/newUnitObject.txt"
#define SIMPLE_PYROMANCER "simple_units/Pyromancer.txt"
#define SIMPLE_STONESLINGER "simple_units/StoneSlinger.txt"
#define SIMPLE_OLIGARCH "simple_units/Oligarch.txt"
#define SIMPLE_KING "simple_units/King.txt"
#define SIMPLE_QUEEN "simple_units/Queen.txt"

MainMenuBoard::MainMenuBoard()
{
	srand(time(NULL));
}

MainMenuBoard::~MainMenuBoard()
{
}

void MainMenuBoard::start()
{
	/*int units[] = { 
		unit::UnitSpawn::Priest, unit::UnitSpawn::Archer, unit::UnitSpawn::Duelist, 
		unit::UnitSpawn::Engineer, unit::UnitSpawn::newUnitObject, unit::UnitSpawn::Pyromancer, 
		unit::UnitSpawn::StoneSlinger, unit::UnitSpawn::Oligarch, unit::UnitSpawn::King, unit::UnitSpawn::Queen 
	};
	std::random_shuffle(units, units + (sizeof(units)/sizeof(*units)));*/

	std::string simpleUnits[] = {
		SIMPLE_PRIEST, SIMPLE_ARCHER, SIMPLE_DUELIST, SIMPLE_ENGINEER, SIMPLE_NEWUNITOBJECT,
		SIMPLE_PYROMANCER, SIMPLE_STONESLINGER, SIMPLE_OLIGARCH, SIMPLE_KING, SIMPLE_QUEEN
	};
	std::random_shuffle(simpleUnits, simpleUnits + (sizeof(simpleUnits) / sizeof(*simpleUnits)));

	kitten::K_GameObject* u0 = kitten::K_GameObjectManager::getInstance()->createNewGameObject(simpleUnits[0]);
	u0->getTransform().move(5.0f, 0.0f, 4.5f);

	kitten::K_GameObject* u1 = kitten::K_GameObjectManager::getInstance()->createNewGameObject(simpleUnits[1]);
	u1->getTransform().move(6.0f, 0.0f, 5.5f);

	kitten::K_GameObject* u2 = kitten::K_GameObjectManager::getInstance()->createNewGameObject(simpleUnits[2]);
	u2->getTransform().move(8.0f, 0.0f, 5.5f);

	kitten::K_GameObject* u3 = kitten::K_GameObjectManager::getInstance()->createNewGameObject(simpleUnits[3]);
	u3->getTransform().move(9.0f, 0.0f, 4.5f);
}