#pragma once

#include "Font.h"
#include "../VertexEnvironment.h"
#include "../Texture.h"
#include "kitten\K_UIRenderable.h"
#include "puppy\P_Renderable.h"
#include <map>
#include <string>
/*
	Callum MacKenzie
*/

namespace puppy
{
	class TextBox : public kitten::K_UIRenderable, public P_Renderable
	{
	public:
		enum Alignment { left, right, center };

	protected:
		std::map<const Texture*, VertexEnvironment*> m_textMap;

		std::string m_text;
		float m_boxWidth, m_boxHeight;
		int m_xPos = 0, m_yPos = 0;
		
		GLfloat m_color[4];
		Alignment m_alignment;
		Font* m_font;

		bool m_is3D;
		bool m_isDirty;

		void constructLeftAlignVertices();
		void constructRightOrCenterAlignVertices(bool p_isRight);
		virtual void constructQuad(int p_charId, int p_xPos, int p_yPos, TexturedVertex p_toSet[]);
		void removeOldText();

	public:
		TextBox(Font* p_fontToUse, const std::string& p_text, float p_boxWidth, float p_boxHeight, bool p_is3D = false);
		TextBox(Font* p_fontToUse, const std::string& p_text, float p_boxWidth, float p_boxHeight, Alignment p_alignment, bool p_is3D = false);
		~TextBox();

		virtual void start() override;
		virtual void onDisabled() override;
		virtual void onEnabled() override;

		void setFont(Font* p_font);
		void setBoxBounds(float p_width, float p_height);
		void setColor(GLfloat p_redVal, GLfloat p_greenVal, GLfloat p_blueVal);
		void setText(const std::string& p_text);
		void setAlignment(Alignment p_alignment);

		const std::string& getText() const;
		int getBoxWidth() const;
		int getBoxHeight() const;
		int getLastLineUsedWidth();
		int getUsedHeight();

		virtual void uiRender(kitten::Camera* p_cam) override;
		virtual void render(kitten::Camera* p_cam) override;
	};
}