#pragma once

#include "Material.h"
#include <map>


namespace puppy
{
	class TextureBlendMaterial : public Material
	{
	private:
		std::unordered_map<Texture*, float> m_textures;

		Texture* m_ownedTexture;

	public:
		TextureBlendMaterial();
		~TextureBlendMaterial();

		virtual bool operator==(const Material& p_other) const;
		virtual Material* clone() const override;

		virtual void setTexture(const char* p_pathToTex) override;
		Texture* getOwnedTexture();

		void addTexture(Texture* p_tex, const float& p_weight = 1.0f);
		void removeTexture(Texture* p_tex);

		void changeWeight(Texture* p_tex, const float& p_weight);
		int getNumberOfTextures() const;

		virtual void apply() const override;
	};
}