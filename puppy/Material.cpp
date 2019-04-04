#include "Material.h"

namespace puppy
{
	Material::Material(ShaderType p_shaderType) : m_tex(nullptr), m_matAmbient(0,0,0,1), m_matDiffuse(0,0,0,1), m_hasColour(false), m_hasMatLightProperties(false)
	{
		m_shader = ShaderManager::getShaderProgram(p_shaderType);
	}

	Material::Material() : m_tex(nullptr), m_matAmbient(0, 0, 0, 1), m_matDiffuse(0, 0, 0, 1), m_hasColour(false), m_hasMatLightProperties(false), m_shader(nullptr)
	{

	}
	
	Material* Material::clone() const
	{
		Material* toReturn = new Material(m_shader->getType());

		if (m_tex != nullptr)
		{
			toReturn->m_tex = new Texture(m_tex->getPath());
		}

		toReturn->m_hasColour = m_hasColour;
		toReturn->m_colour = m_colour;

		toReturn->m_hasMatLightProperties = m_hasMatLightProperties;
		toReturn->m_matAmbient = m_matAmbient;
		toReturn->m_matDiffuse = m_matDiffuse;

		return toReturn;
	}

	bool Material::operator==(const Material& p_other) const
	{
		if (m_hasColour != p_other.m_hasColour)
		{
			return false;
		}

		if (m_hasColour)
		{
			if (m_colour != p_other.m_colour)
			{
				return false;
			}
		}

		if (m_hasMatLightProperties != p_other.m_hasMatLightProperties)
		{
			return false;
		}
		
		if (m_hasMatLightProperties)
		{
			if (m_matAmbient != p_other.m_matAmbient)
			{
				return false;
			}

			if (m_matDiffuse != p_other.m_matDiffuse)
			{
				return false;
			}
		}

		if (m_tex == nullptr && p_other.m_tex == nullptr)
		{
			if (m_shader->getType() == p_other.m_shader->getType())
			{
				return true;
			}
		}
		else if (m_tex != nullptr && p_other.m_tex != nullptr)
		{
			if ((m_tex->getTex() == p_other.m_tex->getTex())
				&& m_shader->getType() == p_other.m_shader->getType())
			{
				
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		return false;
	}

	void Material::apply() const
	{
		if (m_shader != nullptr)
		{
			ShaderManager::applyShader(m_shader);
		}
		
		if (m_tex != nullptr)
		{
			m_tex->apply();
		}

		if (m_hasColour)
		{
			setUniform("colour", m_colour);
		}

		applyMatLightProperties();
	}

	void Material::setTexture(const char* p_pathToTex)
	{
		if (m_tex != nullptr)
		{
			delete m_tex;
		}
		m_tex = new Texture(p_pathToTex);
	}

	void Material::setTexture(Texture* p_tex)
	{
		if (m_tex != nullptr)
		{
			delete m_tex;
		}
		m_tex = p_tex;
	}

	void Material::setColour(const glm::vec4& p_colour)
	{
		m_hasColour = true;
		m_colour = p_colour;
	}

	void Material::setMatAmbient(const glm::vec4& p_amb)
	{
		m_matAmbient = p_amb;
		m_hasMatLightProperties = true;
	}

	void Material::setMatDiffuse(const glm::vec4& p_diff)
	{
		m_matDiffuse = p_diff;
		m_hasMatLightProperties = true;
	}

	void Material::setShader(ShaderType p_type)
	{
		m_shader = ShaderManager::getShaderProgram(p_type);
	}

	Texture* Material::getTexture() const
	{
		return m_tex;
	}

	int Material::getUniformPlace(const std::string& p_name)
	{
		return m_shader->getUniformPlace(p_name);
	}

	void Material::setUniform(const std::string& p_name, const glm::mat4& p_mat4) const
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniformMatrix4fv(place, 1, GL_FALSE, glm::value_ptr(p_mat4));
	}

	void Material::setUniform(const std::string& p_name, const glm::mat3& p_mat3) const
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniformMatrix3fv(place, 1, GL_FALSE, glm::value_ptr(p_mat3));
	}

	void Material::setUniform(const std::string& p_name, const glm::vec4& p_vec4) const
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform4fv(place, 1, glm::value_ptr(p_vec4));
	}

	void Material::setUniform(const std::string& p_name, const glm::vec3& p_vec3) const
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform3fv(place, 1, glm::value_ptr(p_vec3));
	}

	void Material::setUniform(const std::string& p_name, const glm::vec2& p_vec2) const
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform2fv(place, 1, glm::value_ptr(p_vec2));
	}

	void Material::setUniform(const std::string& p_name, const float& p_float) const
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform1f(place, p_float);
	}

	void Material::setUniform(const std::string& p_name, const int p_int) const
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform1i(place, p_int);
	}
}