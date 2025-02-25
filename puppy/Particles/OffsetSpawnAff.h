#pragma once
#include <list>
#include "SpawnAffector.h"
#include "Particle.h"
#include "Emitter.h"
namespace puppy
{
	enum OffsetType {point,disc,sphere,box, MAX = box};

	class OffsetSpawnAff : public SpawnAffector
	{
	private:
		OffsetType m_mode;

		glm::vec3 m_offset; //always added

		float m_radius; //For disc and sphere types
		glm::vec3 m_topLeftBound, m_bottomRightBound; //For box type
	public:
		OffsetSpawnAff(const glm::vec3& p_staticOffset, OffsetType p_type) : m_offset(p_staticOffset), m_mode(p_type) {};

		void setRadius(float p_radius) { m_radius = p_radius; }
		void setBoxBounds(const glm::vec3& p_topLeft, const glm::vec3& p_bottomRight) { m_topLeftBound = p_topLeft; m_bottomRightBound = p_bottomRight; }


		virtual void apply(std::list<Particle*>& p_toApply);
	};
}