#include "FBO.h"
#include <iostream>

namespace puppy
{
	GLuint FBO::m_boundBuffer = 0;

	FBO::FBO()
	{
		glGenFramebuffers(1, &m_id);
	}

	FBO::~FBO()
	{
		glDeleteFramebuffers(1, &m_id);
	}

	void FBO::setTexture2D(GLenum p_attachment, const puppy::Texture* p_tex, int p_mipMapLevel)
	{
		if (m_boundBuffer != m_id)
		{
			m_boundBuffer = m_id;
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, p_attachment, GL_TEXTURE_2D, p_tex->getTex(), p_mipMapLevel);
	}

	void FBO::setRenderBuffer(GLenum p_attachment, const puppy::RenderBuffer* p_buffer)
	{
		if (m_boundBuffer != m_id)
		{
			m_boundBuffer = m_id;
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, p_attachment, GL_RENDERBUFFER, p_buffer->getId());
	}

	void FBO::apply()
	{
		if (m_boundBuffer != m_id)
		{
			m_boundBuffer = m_id;
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		}
	}

	GLenum FBO::getFrameBufferStatus()
	{
		if (m_boundBuffer != m_id)
		{
			m_boundBuffer = m_id;
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		}
		return glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}

	void FBO::printFrameBufferStatus()
	{
		GLenum stat = this->getFrameBufferStatus();
		switch (stat)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			std::cout << "Framebuffer Complete" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cout << "Incomplete attachment" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cout << "Incomplete missing attachment" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cout << "Incomplete draw buffer" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cout << "Incomplete read buffer" << std::endl;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cout << "Framebuffer unsupported" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			std::cout << "Incomplete multisample" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			std::cout << "Incomplete layer targets" << std::endl;
			break;
		default:
			std::cout << "Unknown error" << std::endl;
			break;
		}
	}

	//public static
	void FBO::bindDefault()
	{
		if (m_boundBuffer != 0)
		{
			m_boundBuffer = 0;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}