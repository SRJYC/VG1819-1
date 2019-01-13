#include "ShaderManager.h"
#include "wolf\W_Common.h"

namespace puppy
{
	//initialize static member variables
	const ShaderProgram* ShaderManager::m_boundShader = nullptr;
	ShaderProgram* ShaderManager::m_createdPresetShaders[ShaderType::SHADERTYPE_MAX+1];

	bool ShaderManager::applyShader(ShaderProgram* p_sp)
	{
		//Use shader if it isn't currently being used
		if (m_boundShader != p_sp)
		{
			p_sp->apply();
			m_boundShader = p_sp;

			
			return true;
		}
		return false;
	}

	bool ShaderManager::applyShader(ShaderType p_st)
	{
		if (m_boundShader == m_createdPresetShaders[p_st] && m_boundShader != nullptr)
		{
			return false; //shader already bound
		}
		//else

		//Check if shader type has already been created
		if (m_createdPresetShaders[p_st] != nullptr)
		{
			//Shader already created, use it.
			return applyShader(m_createdPresetShaders[p_st]);
		}
		//else

		//We need to create the shadertype specified
		std::string vertexShaderPath, pixelShaderPath;
		if (getShaderPaths(p_st, &vertexShaderPath, &pixelShaderPath))
		{
			m_createdPresetShaders[p_st] = new ShaderProgram(vertexShaderPath, pixelShaderPath,p_st);
			//apply 
			return applyShader(m_createdPresetShaders[p_st]);
		}
	}

	bool ShaderManager::getShaderPaths(ShaderType p_st, std::string* p_vertexShaderPath, std::string* p_pixelShaderPath)
	{
		switch (p_st)
		{
		case ShaderType::basic:
			*p_vertexShaderPath = "Shaders/basic.vsh";
			*p_pixelShaderPath = "Shaders/basic.fsh";
			return true;
		case ShaderType::sprite:
			*p_vertexShaderPath = "Shaders/sprite.vsh";
			*p_pixelShaderPath = "Shaders/sprite.fsh";
			return true;
		case ShaderType::alphaTest:
			*p_vertexShaderPath = "Shaders/basic_alphatest.vsh";
			*p_pixelShaderPath = "Shaders/basic_alphatest.fsh";
			return true;
		case ShaderType::colorTint_alphaTest:
			*p_vertexShaderPath = "Shaders/colorTint_alphatest.vsh";
			*p_pixelShaderPath = "Shaders/colorTint_alphatest.fsh";
			return true;
		case ShaderType::particles:
			*p_vertexShaderPath = "Shaders/particles.vsh";
			*p_pixelShaderPath = "Shaders/particles.fsh";
			return true;
		case ShaderType::solid_color:
			*p_vertexShaderPath = "Shaders/solid_colour.vsh";
			*p_pixelShaderPath = "Shaders/solid_colour.fsh";
			return true;
		case ShaderType::basic_directional_light:
			*p_vertexShaderPath = "Shaders/basic_directional_light.vsh";
			*p_pixelShaderPath = "Shaders/basic_directional_light.fsh";
			return true;
		case ShaderType::solid_color_directional_light:
			*p_vertexShaderPath = "Shaders/solid_colour_directional_light.vsh";
			*p_pixelShaderPath = "Shaders/solid_colour_directional_light.fsh";
			return true;
		case ShaderType::texture_blend_zero:
			*p_vertexShaderPath = "Shaders/basic.vsh";
			*p_pixelShaderPath = "Shaders/basic_zero_blend.fsh";
			return true;
		case ShaderType::texture_blend_one:
			*p_vertexShaderPath = "Shaders/basic.vsh";
			*p_pixelShaderPath = "Shaders/basic_one_blend.fsh";
			return true;
		case ShaderType::texture_blend_two:
			*p_vertexShaderPath = "Shaders/basic.vsh";
			*p_pixelShaderPath = "Shaders/basic_two_blend.fsh";
			return true;
		case ShaderType::texture_blend_three:
			*p_vertexShaderPath = "Shaders/basic.vsh";
			*p_pixelShaderPath = "Shaders/basic_three_blend.fsh";
			return true;
		case ShaderType::texture_blend_four:
			*p_vertexShaderPath = "Shaders/basic.vsh";
			*p_pixelShaderPath = "Shaders/basic_four_blend.fsh";
			return true;
		case ShaderType::basic_u_scale:
			*p_vertexShaderPath = "Shaders/basic_u_scale.vsh";
			*p_pixelShaderPath = "Shaders/basic_u_scale.fsh";
			return true;
		default:
			return false;
		}
	}

	ShaderProgram* ShaderManager::getShaderProgram(ShaderType p_st)
	{
		if (m_createdPresetShaders[p_st] == nullptr)
		{
			std::string vertexShaderPath, pixelShaderPath;
			if (getShaderPaths(p_st, &vertexShaderPath, &pixelShaderPath))
			{
				m_createdPresetShaders[p_st] = new ShaderProgram(vertexShaderPath, pixelShaderPath, p_st);
			}
		}

		return m_createdPresetShaders[p_st];
	}

	void ShaderManager::destroyAllShaders()
	{
		for (int i =0; i < SHADERTYPE_MAX+1; ++i)
		{
			if(m_createdPresetShaders[i] != nullptr)
			{
				delete m_createdPresetShaders[i];
			}
		}
	}
}