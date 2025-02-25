#include "FPSCamera.h"
#include "InputManager.h"
#include "K_Time.h"

#include <iostream>

namespace scene
{
	FPSCamera::FPSCamera() : m_cameraPos(0.0f,5.0f,0.0f), m_lookVertAngle(0.0f)
	{
	}

	FPSCamera::~FPSCamera()
	{
	}

	glm::mat4 FPSCamera::getViewProj()
	{
		return m_viewProj;
	}

	void FPSCamera::setLookSensitivity(float p_sensitivity)
	{
		m_lookSensitivity = p_sensitivity;
	}

	void FPSCamera::setMoveSpeed(float p_speed)
	{
		m_moveSpeed = p_speed;
	}

	void FPSCamera::update()
	{
		float deltaTime = kitten::K_Time::getInstance()->getDeltaTime();

		//Compute angles
		m_lookHorizAngle += m_lookSensitivity * deltaTime * input::InputManager::getInstance()->getMouseXChange();
		m_lookVertAngle += m_lookSensitivity * deltaTime * input::InputManager::getInstance()->getMouseYChange();

		//Restrict camera from looping around vertically
		if (m_lookVertAngle < -1.6f)
		{
			m_lookVertAngle = -1.6f;
		}
		if (m_lookVertAngle > 1.6f)
		{
			m_lookVertAngle = 1.6f;
		}

		glm::vec3 lookDirection = glm::vec3(
			cos(m_lookVertAngle) * sin(m_lookHorizAngle),
			sin(m_lookVertAngle),
			cos(m_lookVertAngle) * cos(m_lookHorizAngle)
		);

		glm::vec3 rightVector = glm::vec3(
			sin(m_lookHorizAngle - PI / 2.0f),
			0,
			cos(m_lookHorizAngle - PI / 2.0f)
		);

		glm::vec3 upVector = glm::cross(rightVector, lookDirection);


		input::InputManager* im = input::InputManager::getInstance();

		float speed = m_moveSpeed;

		//Keyboard related
		if (im->keyDown(GLFW_KEY_LSHIFT))
		{
			speed *= 5.0f;
		}
		if (im->keyDown('W') || im->keyDown('w'))
		{
			m_cameraPos += lookDirection * deltaTime * speed;
		}
		if (im->keyDown('S') || im->keyDown('s'))
		{
			m_cameraPos -= lookDirection * deltaTime * speed;
		}
		if (im->keyDown('A') || im->keyDown('a'))
		{
			m_cameraPos -= rightVector * deltaTime * speed;
		}
		if (im->keyDown('D') || im->keyDown('s'))
		{
			m_cameraPos += rightVector * deltaTime * speed;
		}

		//Update camera
		glm::mat4 view = glm::lookAt(m_cameraPos, m_cameraPos + lookDirection, upVector);
		
		glm::mat3 inv = (glm::mat3) view;
		m_viewInverse =  glm::transpose(inv);

		m_viewProj = getProj() * view; //glm::lookAt(m_cameraPos, m_cameraPos + lookDirection, upVector);

		//m_viewFrustum = computeViewFrustum(m_cameraPos, m_cameraPos + lookDirection, upVector);
	}

	const Frustum& FPSCamera::getViewFrustum()
	{
		return m_viewFrustum;
	}

	glm::mat3 FPSCamera::getViewInverse() const
	{
		return m_viewInverse;
	}
}