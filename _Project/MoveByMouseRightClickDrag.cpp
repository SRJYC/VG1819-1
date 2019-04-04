#include "MoveByMouseRightClickDrag.h"
#include "kitten\InputManager.h"
#include "kitten\Transform.h"
#include "kitten\K_Time.h"

#include "util\MathUtil.h"
#include <iostream>

MoveByMouseRightClickDrag::MoveByMouseRightClickDrag(float p_speed, const glm::vec2& p_minClamp, const glm::vec2& p_maxClamp) : m_speed(p_speed),
	m_minClamp(p_minClamp), m_maxClamp(p_maxClamp)
{

}

MoveByMouseRightClickDrag::~MoveByMouseRightClickDrag()
{

}

bool MoveByMouseRightClickDrag::hasUpdate() const
{
	return true;
}

void MoveByMouseRightClickDrag::update()
{
	input::InputManager* inputMan = input::InputManager::getInstance();
	if (inputMan->mouseDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float xChange = -(inputMan->getMouseXChange());
		float yChange = -(inputMan->getMouseYChange());

		kitten::Transform& transform = getTransform();
		
		glm::vec3 newPos = transform.getTranslation();

		// Don't actually have to use delta time here, mouse change is independent of framerate
		newPos.x = CLAMP(newPos.x + xChange * m_speed, m_minClamp.x, m_maxClamp.x);
		newPos.z = CLAMP(newPos.z + yChange * m_speed, m_minClamp.y, m_maxClamp.y);

		transform.place(newPos.x, newPos.y, newPos.z);
	}
}