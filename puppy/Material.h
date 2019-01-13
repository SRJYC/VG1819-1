#pragma once

#include "P_Common.h"
#include "puppy\ShaderProgram.h"
#include "puppy\Texture.h"
#include "puppy\ShaderManager.h"

#include <string>

namespace puppy
{
	class Material
	{
	private:
		//Member variables
		std::string m_name;

		puppy::Texture* m_tex;

	protected:
		puppy::ShaderProgram* m_shader;
		
	public:
		Material(ShaderType p_shaderType);
		~Material();

		virtual void setTexture(const char* p_pathToTex);
		virtual void setTexture(puppy::Texture* p_tex);
		puppy::Texture* getTexture() const;

		//One-time set
		void setUniform(const std::string& p_name, const glm::mat4& p_mat4);
		void setUniform(const std::string& p_name, const glm::mat3& p_mat3);
		void setUniform(const std::string& p_name, const glm::vec4& p_vec4);
		void setUniform(const std::string& p_name, const glm::vec3& p_vec3);
		void setUniform(const std::string& p_name, const glm::vec2& p_vec2);
		void setUniform(const std::string& p_name, const float& p_float);
		void setUniform(const std::string& p_name, const int p_int);

		int getUniformPlace(const std::string& p_name);

		virtual void apply();
	};
}