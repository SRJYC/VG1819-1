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
		bool m_hasColour;
		glm::vec4 m_colour; //hacky because adding memorizing uniforms will take a bit and it is not scheduled

	public:
		Material(ShaderType p_shaderType);
		Material(const Material& p_other);
		~Material();

		virtual bool operator==(const Material& p_other) const;
		virtual Material* clone() const;

		virtual void setTexture(const char* p_pathToTex);
		virtual void setTexture(puppy::Texture* p_tex);
		puppy::Texture* getTexture() const;

		virtual void setColour(const glm::vec4& m_colour); //see above comment

		//One-time set
		void setUniform(const std::string& p_name, const glm::mat4& p_mat4) const;
		void setUniform(const std::string& p_name, const glm::mat3& p_mat3) const;
		void setUniform(const std::string& p_name, const glm::vec4& p_vec4) const;
		void setUniform(const std::string& p_name, const glm::vec3& p_vec3) const;
		void setUniform(const std::string& p_name, const glm::vec2& p_vec2) const;
		void setUniform(const std::string& p_name, const float& p_float) const;
		void setUniform(const std::string& p_name, const int p_int) const;

		int getUniformPlace(const std::string& p_name);

		virtual void apply() const;
	};
}