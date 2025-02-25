#include "SpriteAnimator.h"
#include "kitten\K_GameObject.h"
#include "kibble\sprites\SpriteLoader.h"

namespace sprites
{
	SpriteAnimator::SpriteAnimator(const std::string& p_spriteName, bool p_randomStart) : m_renderable(nullptr), m_currentAnimationFrameTime(0), m_randomStart(p_randomStart)
	{
		m_spriteSheet = kibble::SpriteLoader::sm_instance->getSpriteSheet(p_spriteName);
	}

	SpriteAnimator::~SpriteAnimator()
	{

	}

	//This only exists because units do not construct their gameobjects like everything else..
	//should be changed so this can be removed.
	//Setting this after start will result in bonked animations
	void SpriteAnimator::setSpriteSheet(const std::string& p_characterName)
	{
		m_spriteSheet = kibble::SpriteLoader::sm_instance->getSpriteSheet(p_characterName);
	}

	void SpriteAnimator::start()
	{
		assert(m_spriteSheet != nullptr);

		m_currentFrame = m_spriteSheet->getDefaultAnimation();

		m_renderable = m_attachedObject->getComponent<kitten::SpriteRenderable>();
		assert(m_renderable != nullptr);
		float uMax = (float)m_spriteSheet->m_characterWidth / m_spriteSheet->m_sheetWidth;
		float vMax = (float)m_spriteSheet->m_characterHeight / m_spriteSheet->m_sheetHeight;
		m_renderable->setupRenderable(uMax, vMax, m_spriteSheet->m_characterWidth, m_spriteSheet->m_characterHeight, *m_spriteSheet->m_material);

		m_time = kitten::K_Time::getInstance();
		assert(m_time != nullptr);

		if (m_randomStart)
		{
			setRandomStartFrame();
		}

		setRenderableTexture();
	}

	void SpriteAnimator::setAnimation(const std::string& p_animationName)
	{
		m_currentFrame = m_spriteSheet->getAnimation(p_animationName);
		m_currentAnimationFrameTime = 0.0f;

		assert(m_currentFrame != nullptr);

		setRenderableTexture();
	}

	void SpriteAnimator::setRenderableTexture()
	{
		assert(m_currentFrame != nullptr);

		m_renderable->setTextureOffset(m_currentFrame->textureOffset);
	}

	void SpriteAnimator::update()
	{
		if (m_currentAnimationFrameTime > m_currentFrame->time)
		{
			// go to next frame
			m_currentFrame = m_currentFrame->next;

			m_currentAnimationFrameTime = 0.0f;

			setRenderableTexture();
		}
		else
		{
			m_currentAnimationFrameTime += m_time->getDeltaTime();
		}
	}

	void SpriteAnimator::setRandomStartFrame()
	{
		int random = rand() % 10;
		for (int i = 0; i < random; i++)
		{
			m_currentFrame = m_currentFrame->next;
		}
	}
}