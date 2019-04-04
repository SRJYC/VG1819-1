#include "SpriteGroup.h"
#include "kitten/K_GameObjectManager.h"
#include "kitten/K_ComponentManager.h"

sprites::SpriteGroup::SpriteGroup(const std::string & p_spriteName, int p_num)
	:m_spriteName(p_spriteName),
	m_spriteNum(p_num),
	m_spriteNumToShow(p_num),
	m_scale(glm::vec3(1,1,1)),
	m_rotation(glm::vec3(45, 0, 0)),
	m_translation(glm::vec3(0, 0, 0))
{
}

sprites::SpriteGroup::~SpriteGroup()
{
}

void sprites::SpriteGroup::setScale(const glm::vec3& p_sc)
{
	m_scale = p_sc;
}

void sprites::SpriteGroup::setRotation(const glm::vec3& p_rot)
{
	m_rotation = p_rot;
}

void sprites::SpriteGroup::setTranslation(const glm::vec3& p_tran)
{
	m_translation = p_tran;
}

void sprites::SpriteGroup::start()
{
	createSpriteGO();
}

void sprites::SpriteGroup::checkHP(int p_hp, int p_max)
{
	//get percentage
	float percent = (float)p_hp / (float)p_max;

	//get number of sprite shoulbe be shown
	int show = ceil(m_spriteNum * percent);

	//there will be at least one sprite
	if (show < 1)
		show = 1;

	//if the number is different than current number to show
	if (show != m_spriteNumToShow)
	{
		m_spriteNumToShow = show;
		changeEnable();
	}
}

void sprites::SpriteGroup::createSpriteGO()
{
	kitten::K_GameObjectManager* gm = kitten::K_GameObjectManager::getInstance();
	kitten::K_ComponentManager* cm = kitten::K_ComponentManager::getInstance();

	for (int i = 0; i < m_spriteNum && i<m_maxNum; i++)
	{
		kitten::K_GameObject* go = gm->createNewGameObject();

		go->addComponent(cm->createComponent("SpriteRenderable"));

		SpriteAnimator* animator = static_cast<SpriteAnimator*>(cm->createComponent("SpriteAnimator"));
		animator->setSpriteSheet(m_spriteName);
		go->addComponent(animator);

		go->getTransform().setIgnoreParent(false);
		go->getTransform().setParent(&m_attachedObject->getTransform());
		
		go->getTransform().scaleAbsolute(m_scale.x, m_scale.y, m_scale.z);
		go->getTransform().rotateAbsolute(m_rotation);

		glm::vec3 pos = getSubTranslation(i);
		go->getTransform().place(pos.x+m_translation.x, pos.y+m_translation.y, pos.z+ m_translation.z);

		m_spriteGOList.push_back(go);
	}
}

const glm::vec3 sprites::SpriteGroup::getSubTranslation(int p_n)
{
	switch (m_spriteNum)
	{
	case 1:
		return glm::vec3(0, 0, 0);
	case 2:
		switch (p_n)
		{
		case 0:
			return glm::vec3(0.25, 0, 0.25);
		case 1:
			return glm::vec3(-0.25, 0, -0.25);
		}
	case 3:
		switch (p_n)
		{
		case 0:
			return glm::vec3(0.25, 0, 0.25);
		case 1:
			return glm::vec3(-0.25, 0, 0.25);
		case 2:
			return glm::vec3(0, 0, -0.25);
		}
	case 4:
		switch (p_n)
		{
		case 0:
			return glm::vec3(0.25, 0, 0.25);
		case 1:
			return glm::vec3(-0.25, 0, 0.25);
		case 2:
			return glm::vec3(0.25, 0, -0.25);
		case 3:
			return glm::vec3(-0.25, 0, -0.25);
		}
	}

	return glm::vec3();
}

void sprites::SpriteGroup::changeEnable()
{
	//enable the number of sprite
	for (int i = 0; i < m_spriteNumToShow; i++)
	{
		m_spriteGOList[i]->setEnabled(true);
	}

	//disable the rest
	for (int i = m_spriteNumToShow; i < m_spriteGOList.size(); i++)
	{
		m_spriteGOList[i]->setEnabled(false);
	}
}
