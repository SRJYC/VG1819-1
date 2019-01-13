#include "TextureBlendMaterial.h"

namespace puppy
{
	TextureBlendMaterial::TextureBlendMaterial() : Material(ShaderType::texture_blend_zero), m_ownedTexture(nullptr)
	{

	}

	TextureBlendMaterial::~TextureBlendMaterial()
	{
		/*
		auto end = m_additionalTextures.cend();
		for (auto it = m_additionalTextures.cbegin(); it != end; ++it)
		{
			delete std::get<0>((*it).second);
		}
		*/

		//We don't own the textures in m_textures except m_ownedTexture

		if (m_ownedTexture != nullptr)
		{
			delete m_ownedTexture;
		}
	}

	void TextureBlendMaterial::setTexture(const char* p_pathToTex)
	{
		if (m_ownedTexture != nullptr)
		{
			m_textures.erase(m_ownedTexture);
			delete m_ownedTexture;
		}

		m_ownedTexture = new Texture(p_pathToTex);
		addTexture(m_ownedTexture);
	}

	Texture* TextureBlendMaterial::getOwnedTexture()
	{
		return m_ownedTexture;
	}

	/*
	void TextureBlendMaterial::addTexture(const char* p_pathToTexToAdd, const float& p_weight)
	{
		int numTextures = m_additionalTextures.size();
		assert(numTextures + 1 <= MAX_BLEND_TEXTURES);

		//Change shader
		if (numTextures > 0)
		{
			int i = 0;
		}

		std::tuple<Texture*, int, float> toInsert = std::make_tuple(new Texture(p_pathToTexToAdd, numTextures), numTextures, p_weight);
		m_additionalTextures.insert(std::make_pair(p_pathToTexToAdd,toInsert));

		m_shader = ShaderManager::getShaderProgram(static_cast<ShaderType>(ShaderType::texture_blend_zero + numTextures));
	}
	*/

	void TextureBlendMaterial::addTexture(Texture* p_tex, const float& p_weight)
	{
		m_textures.insert(std::make_pair(p_tex, p_weight));
		m_shader = ShaderManager::getShaderProgram(static_cast<ShaderType>(ShaderType::texture_blend_zero + m_textures.size() - 1));
	}

	/*
	void TextureBlendMaterial::removeTexture(const char* p_pathToTexToRemove)
	{
		auto found = m_additionalTextures.find(p_pathToTexToRemove);
		if (found != m_additionalTextures.end())
		{
			delete std::get<0>((*found).second);
			m_additionalTextures.erase(found);

			m_shader = ShaderManager::getShaderProgram(static_cast<ShaderType>(ShaderType::texture_blend_zero + m_additionalTextures.size()-1));

			//Shift texture slots
			int i = 0;
			auto end = m_additionalTextures.cend();
			for (auto it = m_additionalTextures.cbegin(); it != end; ++it)
			{
				auto texTuple = (*it).second;
				int& slot = std::get<1>(texTuple);
				slot = i;
				auto tex = std::get<0>(texTuple);
				tex->setSlot(i);

				++i;
			}
		}
		else
		{
			//assert(false);
			int i = 0;
		}
	}
	*/

	void TextureBlendMaterial::removeTexture(Texture* p_tex)
	{
		m_textures.erase(p_tex);
		m_shader = ShaderManager::getShaderProgram(static_cast<ShaderType>(ShaderType::texture_blend_zero + m_textures.size() -1));
	}

	void TextureBlendMaterial::changeWeight(Texture* p_tex, const float& p_weight)
	{
		auto found = m_textures.find(p_tex);
		if (found != m_textures.cend())
		{
			auto& pair = *found;	
			pair.second = p_weight;
		}

		/*auto found = m_additionalTextures.find(p_pathToTex);

		assert(found != m_additionalTextures.cend());

		float& oldWeight = std::get<2>((*found).second);
		oldWeight = p_weight;
		*/
	}

	int TextureBlendMaterial::getNumberOfTextures() const
	{
		return m_textures.size();
	}

	void TextureBlendMaterial::apply()
	{
		bool wasBound = ShaderManager::applyShader(m_shader);

		if (!wasBound)
		{
			//Set texture (sampler) uniforms
			if (m_shader->getType() > texture_blend_zero)
			{
				int texUniforms = m_shader->getType() - ShaderType::texture_blend_zero;
				for (int i = 1; i <= texUniforms; ++i)
				{
					setUniform(SAMPLER2D_NAME + std::to_string(i), i);
				}
			}
		}

		int i = 0;
		auto end = m_textures.cend();
		for (auto it = m_textures.cbegin(); it != end; ++it)
		{
			auto pair = (*it);
			auto tex = pair.first;
			tex->setSlot(i);
			tex->apply();

			std::string uniformName = TEXTURE_BLEND_WEIGHT_UNIFORM_NAME;
			uniformName += std::to_string(i);
			setUniform(uniformName, pair.second);


			++i;
		}

		/*
		//Apply textures
		auto end = m_additionalTextures.cend();
		for (auto it = m_additionalTextures.cbegin(); it != end; ++it)
		{
			auto texTuple = (*it).second;
			std::get<0>(texTuple)->apply();

			//Set weight
			std::string uniformName = TEXTURE_BLEND_WEIGHT_UNIFORM_NAME;
			uniformName += std::to_string(std::get<1>(texTuple));
			setUniform(uniformName,std::get<2>(texTuple));
		}
		*/
	}
}