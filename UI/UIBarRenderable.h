#pragma once
#include "UI\UIElement.h"

namespace userinterface {
	class UIBarRenderable : public UIElement
	{
	private:
		float m_uScale;

	public:
		UIBarRenderable(const char* p_pathToTexture = nullptr);
		~UIBarRenderable();

		void setUScale(const float& p_scale);

		virtual void uiRender(kitten::Camera* p_cam) override;
	};

}