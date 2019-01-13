#include "UnitAura.h"
#include "kitten/K_GameObjectManager.h"
#include "kibble/kibble.hpp"

unit::UnitAura::UnitAura()
{
}

unit::UnitAura::~UnitAura()
{
}

void unit::UnitAura::start()
{
}

bool unit::UnitAura::hasUpdate() const
{
	return true;
}

void unit::UnitAura::update()
{
	m_attachedObject->getTransform().rotateRelative(glm::vec3(0, m_speed, 0));
}
