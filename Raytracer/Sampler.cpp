#include "Sampler.h"

Sampler::Sampler(const GLint& minFilter, const GLint& magFilter, const GLint& wrap) {
	glCreateSamplers(1, &m_ID);

	glSamplerParameteri(m_ID, GL_TEXTURE_WRAP_S, wrap);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrap);
	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilter);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, magFilter);
}

unsigned int Sampler::getID() const {
	return m_ID;
}