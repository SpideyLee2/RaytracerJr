#pragma once

#include "Shader.h"

#include <unordered_map>

namespace mtn {

	enum class ShaderName {
		STAR,
		PLANET
	};

	class ShaderPool {
	public:
		ShaderPool(const size_t buckets);
		void addShader(const Shader& shader, const ShaderName shaderName);
		Shader* getShader(const ShaderName shaderName);

	private:
		std::unordered_map<ShaderName, Shader> m_Pool;
	};

}