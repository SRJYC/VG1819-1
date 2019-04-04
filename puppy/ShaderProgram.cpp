#include "ShaderProgram.h"
#include "../wolf/W_Common.h"

#include "lights\P_LightList.h"

#define NO_AMBIENT_LIGHT_VAL glm::vec4(0.5, 0.5, 0.5, 1)
#define MAX_POINT_LIGHTS 2

namespace puppy
{
	ShaderProgram::ShaderProgram(const std::string& p_vertexShaderPath, const std::string& p_pixelShaderPath, ShaderType p_type) : m_type(p_type), m_hasDirectionalLights(false), m_hasPointLights(false)
	{
		//If I were to write my own shader compiling / loading it would be the same as wolf's.
		m_id = wolf::LoadShaders(p_vertexShaderPath, p_pixelShaderPath);

		if (m_type == basic_directional_light || m_type == solid_color_directional_light || m_type == directional_light_alphaTest) // Not sure what the better way to do this is
		{
			m_hasDirectionalLights = true;
		}

		if (m_type == point_light_alphaTest || m_type == texture_blend_zero_point_light || m_type == texture_blend_one_point_light || 
			m_type ==  texture_blend_two_point_light || m_type == texture_blend_three_point_light || m_type == texture_blend_four_point_light)
		{
			m_hasPointLights = true;
		}
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteShader(m_id);
	}

	void ShaderProgram::apply()
	{
		glUseProgram(m_id);

		if (m_type == point_light_alphaTest)
		{
			int i = 0;
		}

		if (m_hasDirectionalLights) // If this _shader_ supports directional lights,
		{
			// Set the lights
			auto lightList = P_LightList::getInstance();
			if (lightList != nullptr)
			{
				auto& directionalLights = lightList->getDirectionalLights();
				if (directionalLights.empty())
				{
					// No light, use default ambient so its not black
					glUniform4fv(getUniformPlace("lightDirectionalColor"), 1, glm::value_ptr(glm::vec4(0, 0, 0, 1)));
					glUniform4fv(getUniformPlace("lightAmbientColor"), 1, glm::value_ptr(NO_AMBIENT_LIGHT_VAL));
				}
				else
				{
					auto firstLight = *(directionalLights.begin());

					glUniform4fv(getUniformPlace("lightDirectionalColor"), 1, glm::value_ptr(firstLight->getDirectionalColor()));
					glUniform4fv(getUniformPlace("lightAmbientColor"), 1, glm::value_ptr(firstLight->getAmbientColor()));
					glUniform3fv(getUniformPlace("lightPos"), 1, glm::value_ptr(firstLight->getPosition()));
				}
			}
		}

		if (m_hasPointLights)
		{
			auto lightList = P_LightList::getInstance();
			if (lightList != nullptr)
			{
				auto& pointLights = lightList->getPointLights();
				auto& directionalLights = lightList->getDirectionalLights();

				const P_DirectionalLight* firstDirLight = nullptr;
				if (!directionalLights.empty())
				{
					firstDirLight = *directionalLights.begin();
				}

				// Use the directional light as ambient if it exists
				if (firstDirLight != nullptr)
				{
					glUniform4fv(getUniformPlace("lightAmbientColor"), 1, glm::value_ptr(firstDirLight->getAmbientColor()));
				}
				else
				{
					glUniform4fv(getUniformPlace("lightAmbientColor"), 1, glm::value_ptr(NO_AMBIENT_LIGHT_VAL));
				}

				unsigned int lightsToEnum = MIN(pointLights.size(), MAX_POINT_LIGHTS);
				int missingLights = MAX_POINT_LIGHTS - pointLights.size();

				auto it = pointLights.begin();
				for (unsigned int i = 1; i < lightsToEnum+1; ++i)
				{
					auto light = (*it);
					glUniform4fv(getUniformPlace("light" + std::to_string(i) + "Diffuse"), 1, glm::value_ptr(light->getPointColor()));
					glUniform3fv(getUniformPlace("light" + std::to_string(i) + "Pos"), 1, glm::value_ptr(light->getPosition()));
					glUniform3fv(getUniformPlace("light" + std::to_string(i) + "Attenuation"), 1, glm::value_ptr(light->getAttenuation()));
					glUniform1f(getUniformPlace("light"  + std::to_string(i) + "Range"), light->getRange());
					++it;
				}

				for (int i = lightsToEnum+1; i < lightsToEnum + missingLights + 1; ++i)
				{
					glUniform1f(getUniformPlace("light" + std::to_string(i) + "Range"), 0);
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
#ifdef DEBUG

		return glGetUniformLocation(m_id, p_name.c_str());

#else

		auto& it = m_uniformPlaces.find(p_name);

		if (it == m_uniformPlaces.end())
		{
			int place = glGetUniformLocation(m_id, p_name.c_str());
			m_uniformPlaces.insert(std::make_pair(p_name, place));

			return place;
		}
		else
		{
			return (*it).second;
		}


#endif
	}
}