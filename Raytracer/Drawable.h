#pragma once

#include "Shader.h"

#include <vector>
#include <array>

#include "glm/glm.hpp"

namespace mtn {

	struct Drawable {
	public:
		Drawable();
		virtual ~Drawable(); // Mostly to get around dynamic_cast requirement

		virtual void genVertsAndInds();

		glm::mat4 model;
		std::vector<glm::vec2> vertices;
		std::vector<unsigned int> indices;
		// Make sure to set these. Possible undefined behavior otherwise.
		Shader* shader;
		unsigned int vao;
		unsigned int vbo;
		unsigned int ibo;

		constexpr static std::array<glm::vec2, 4> quadVerts{
			glm::vec2(-0.5f,  0.5f), // top-left
			glm::vec2(-0.5f, -0.5f), // bottom-left
			glm::vec2(0.5f, -0.5f), // bottom-right
			glm::vec2(0.5f,  0.5f)  // top-right
		};

		constexpr static std::array<unsigned int, 6> quadIndices{
			0, 1, 2,
			2, 3, 0
		};
	};

}