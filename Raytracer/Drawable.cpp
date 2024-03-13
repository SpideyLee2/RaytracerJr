#include "Drawable.h"

namespace mtn {

	Drawable::Drawable() 
		: model(glm::mat4(1.0f)), vertices(), indices(), shader(nullptr),
		  vao(0), vbo(0), ibo(0) {}

	Drawable::~Drawable() {
		shader = nullptr;
		delete(shader);
	}

	void Drawable::genVertsAndInds() {
		// Planets will be drawn on a quad
		vertices.reserve(quadVerts.size());
		vertices.insert(vertices.end(), quadVerts.begin(), quadVerts.end());
		indices.reserve(quadIndices.size());
		indices.insert(indices.end(), quadIndices.begin(), quadIndices.end());
	}

}