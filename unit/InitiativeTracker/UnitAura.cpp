#include "UnitAura.h"
#include "kitten/K_GameObjectManager.h"
#include "kibble/kibble.hpp"

unit::UnitAura::UnitAura(float p_speed, float p_max, float p_min)
	:m_speed(p_speed),m_max(p_max),m_min(p_min)
{
	m_scale = 1.0;
	m_pos = true;
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
	if (m_pos)
	{
		m_scale += m_speed;
		if (m_scale >= m_max)
			m_pos = false;
	}
	else
	{
		m_scale -= m_speed;
		if (m_scale <= m_min)
			m_pos = true;
	}

	m_attachedObject->getTransform().scaleAbsolute(m_scale,1,m_scale);
}
