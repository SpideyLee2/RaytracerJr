#pragma once

#include "glad.h"

class Sampler {
public:
	Sampler() = delete;
	Sampler(const GLint& minFilter, const GLint& magFilter, const GLint& wrap);

	unsigned int getID() const;

private:
	unsigned int m_ID;
};

