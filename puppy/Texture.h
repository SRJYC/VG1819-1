#pragma once

//-----------------------------------------------------------------------------
// File:			Texture.h
// Original Author:	Callum MacKenzie
//
// Class that creates and handles binding of textures
// only TGA supported
//-----------------------------------------------------------------------------

#include <unordered_map>
#include <string>
#include "P_Common.h"

#define BASE_TEXTURE GL_TEXTURE0
#define MAX_TEXTURE BASE_TEXTURE + MAX_BLEND_TEXTURES

namespace puppy
{
	class Texture
	{
	private:
		GLuint m_tex; //The member texture
		std::string m_name;
		int m_slot;

		int m_wrapMode, m_minFiltering, m_magFiltering;
		static int sm_boundWrapMode, sm_boundMinFiltering, sm_boundMagFiltering;

		static std::unordered_map<std::string, std::pair<GLuint, int>> sm_loadedTextures;
		
		static int sm_activeTexture;
		static GLuint sm_boundTexture[MAX_BLEND_TEXTURES]; //Currently bound tex

		void bind() const;
	public:
		Texture(const std::string& p_texName, int p_slot=0); //Takes the name of the file and where to bind it
		Texture(); //empty texture

		~Texture();

		void setWrapping(const int p_mode);
		void setMinFiltering(const int p_mode);
		void setMagFiltering(const int p_mode);

		void setSlot(int p_slot);

		const GLuint* getTex() const;
		const std::string& getPath() const;

		void apply() const;
	};
}