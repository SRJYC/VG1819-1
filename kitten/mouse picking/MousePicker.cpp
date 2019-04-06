#include "MousePicker.h"
#include "ActiveClickables.h"

namespace MousePicker
{
	//Based on the method outlined in:
	//http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/

	bool rayHits(const kitten::Ray& p_ray, kitten::ClickableBox* p_clickable, double* p_hitOutput)
	{
		double minHit = 0.0f;
		double maxHit = 9999999.9f;
		
		const glm::vec3 aabbMinPoint = p_clickable->getMinPoint();
		const glm::vec3 aabbMaxPoint = p_clickable->getMaxPoint();


		const glm::mat4& worldMat = p_clickable->getTransform().getWorldTransformNoScale();

		glm::vec3 worldPos = glm::vec3(worldMat[3].x, worldMat[3].y, worldMat[3].z);
		glm::vec3 rayDistance = worldPos - p_ray.origin;

		//For each axis
		for (int i = 0; i < 3; ++i)
		{
			glm::vec3 axis(worldMat[i].x, worldMat[i].y, worldMat[i].z);

			double e = glm::dot(axis, rayDistance);
			double f = glm::dot(p_ray.direction, axis);

			if (fabs(f) > 0.001f)
			{
				double t1 = (e + aabbMinPoint[i]) / f; //Left plane intersection
				double t2 = (e + aabbMaxPoint[i]) / f; //Right plane intersection

				if (t1 > t2)
				{
					//Swap values
					double temp = t1;
					t1 = t2;
					t2 = temp;
				}

				if (t2 < maxHit)
				{
					maxHit = t2;
				}
				if (t1 > minHit)
				{
					minHit = t1;
				}

				if (maxHit < minHit)
				{
					return false;
				}
				//otherwise, keep looking for a hit
			}
			else //ray almost parallel to plane
			{
				if (-e + aabbMinPoint[i] > 0.0f || -e + aabbMaxPoint[i] < 0.0f)
				{
					return false;
				}
			}
		}
		
		assert(p_hitOutput != nullptr);

		*p_hitOutput = minHit;
		return true;
	}

	//austin's ui hit check method
	bool uiHit(const kitten::ClickableFrame* p_clickable, float p_mouseX, float p_mouseY)
	{
		glm::vec2 maxpoint = p_clickable->getMaxPoint();
		glm::vec2 minpoint = p_clickable->getMinPoint();

		if (p_mouseX > minpoint.x && p_mouseY  > minpoint.y)
		{
			if (p_mouseX < maxpoint.x && p_mouseY < maxpoint.y)
			{
				return true;
			}
			else {
				return false;
			}
		} else {
			return false;
		}
	}

	kitten::ClickableBox* getClosestHit(const kitten::Ray& p_ray)
	{
		double minHit = 9999999.9f;
		double tempHit = -1.0f;
		kitten::ClickableBox* minClick = nullptr;

		const auto& activeClickables = kitten::ActiveClickables::getInstance()->getClickableList();

		auto end = activeClickables.cend();
#pragma loop(hint_parallel(2))
		for (auto it = activeClickables.cbegin(); it != end; ++it)
		{
			if (rayHits(p_ray, *it, &tempHit))
			{
				if (tempHit < minHit)
				{
					minHit = tempHit;
					minClick = *it;
				}
			}
		}

		return minClick;
	}

	kitten::ClickableFrame* getClosestHitFrame(int p_mouseX, int p_mouseY)
	{
		const auto& activeUIClickables = kitten::ActiveClickables::getInstance()->getClickableUIList();
		kitten::ClickableFrame* hitFrame = nullptr;
		float z = -9999999.9f;

		auto end = activeUIClickables.cend();
		for (auto it = activeUIClickables.cbegin(); it != end; ++it)
		{
			if (uiHit(*it, p_mouseX, p_mouseY) && (*it)->getTransform().getTranslation().z > z)
			{
				hitFrame = *it;
				z = (*it)->getTransform().getTranslation().z;
			}
		}
		return hitFrame;
	}
}