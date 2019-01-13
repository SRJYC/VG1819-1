#include "Material.h"

namespace puppy
{
	Material::Material(ShaderType p_shaderType) : m_tex(nullptr)
	{
		m_shader = ShaderManager::getShaderProgram(p_shaderType);
	}

	Material::~Material()
	{
		if (m_tex != nullptr)
		{
			delete m_tex;
		}
	}
	
	void Material::apply()
	{
		if (m_shader != nullptr)
		{
			ShaderManager::applyShader(m_shader);
		}
		
		if (m_tex != nullptr)
		{
			m_tex->apply();
		}
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

	Texture* Material::getTexture() const
	{
		return m_tex;
	}

	int Material::getUniformPlace(const std::string& p_name)
	{
		return m_shader->getUniformPlace(p_name);
	}

	void Material::setUniform(const std::string& p_name, const glm::mat4& p_mat4)
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniformMatrix4fv(place, 1, GL_FALSE, glm::value_ptr(p_mat4));
	}

	void Material::setUniform(const std::string& p_name, const glm::mat3& p_mat3)
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniformMatrix3fv(place, 1, GL_FALSE, glm::value_ptr(p_mat3));
	}

	void Material::setUniform(const std::string& p_name, const glm::vec4& p_vec4)
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform4fv(place, 1, glm::value_ptr(p_vec4));
	}

	void Material::setUniform(const std::string& p_name, const glm::vec3& p_vec3)
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform3fv(place, 1, glm::value_ptr(p_vec3));
	}

	void Material::setUniform(const std::string& p_name, const glm::vec2& p_vec2)
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform2fv(place, 1, glm::value_ptr(p_vec2));
	}

	void Material::setUniform(const std::string& p_name, const float& p_float)
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform1f(place, p_float);
	}

	void Material::setUniform(const std::string& p_name, const int p_int)
	{
		GLint place = m_shader->getUniformPlace(p_name);
		glUniform1i(place, p_int);
	}
}