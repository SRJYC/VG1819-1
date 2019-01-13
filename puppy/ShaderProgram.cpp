#include "ShaderProgram.h"
#include "../wolf/W_Common.h"

#include "lights\P_LightList.h"

namespace puppy
{
	ShaderProgram::ShaderProgram(const std::string& p_vertexShaderPath, const std::string& p_pixelShaderPath, ShaderType p_type) : m_type(p_type), m_hasLights(false)
	{
		//If I were to write my own shader compiling / loading it would be the same as wolf's.
		m_id = wolf::LoadShaders(p_vertexShaderPath, p_pixelShaderPath);

		if (m_type == basic_directional_light || m_type == solid_color_directional_light) // Not sure what the better way to do this is
		{
			m_hasLights = true;
		}
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteShader(m_id);
	}

	void ShaderProgram::apply()
	{
		glUseProgram(m_id);

		if (m_hasLights) // If this _shader_ supports lights,
		{
			// Set the lights
			auto lightList = P_LightList::getInstance();
			if (lightList != nullptr)
			{
				auto& directionalLights = lightList->getDirectionalLights();
				if (directionalLights.empty())
				{
					// No light, all we have to do is set color to black and we are fine
					glUniform3fv(getUniformPlace("lightDirectionalColor"), 1, glm::value_ptr(glm::vec3(0, 0, 0)));
					glUniform3fv(getUniformPlace("lightAmbientColor"), 1, glm::value_ptr(glm::vec3(0, 0, 0)));
				}
				else
				{
					if (m_type == basic_directional_light)
					{
						int i = 0;
					}

					auto firstLight = *(directionalLights.begin());

					glUniform3fv(getUniformPlace("lightDirectionalColor"), 1, glm::value_ptr(firstLight->getDirectionalColor()));
					glUniform3fv(getUniformPlace("lightAmbientColor"), 1, glm::value_ptr(firstLight->getAmbientColor()));
					glUniform3fv(getUniformPlace("lightPos"), 1, glm::value_ptr(firstLight->getPosition()));
				}
			}
		}
	}

	int ShaderProgram::getAttrLocation(const char* p_name) const
	{
		return glGetAttribLocation(m_id, p_name);
	}

	int ShaderProgram::getUniformPlace(const std::string& p_name)
	{
		/*auto it = m_uniformPlaces.find(p_name.c_str());

		if (it == m_uniformPlaces.end())
		{
			m_uniformPlaces.insert(std::make_pair(p_name, glGetUniformLocation(m_id, p_name.c_str())));
		}
		
		return m_uniformPlaces[p_name];
		*/
		return glGetUniformLocation(m_id, p_name.c_str());
	}
}