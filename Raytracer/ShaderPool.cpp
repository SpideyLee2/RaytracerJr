#include "ShaderPool.h"

namespace mtn {

	ShaderPool::ShaderPool(const size_t size) {
		m_Pool = std::unordered_map<ShaderName, Shader>(size);
	}

	void ShaderPool::addShader(const Shader& shader, const ShaderName shaderName) {
		m_Pool.emplace(std::make_pair(shaderName, shader));
	}

	Shader* ShaderPool::getShader(const ShaderName shaderName) {
		try {
			return &m_Pool.at(shaderName);
		}
		catch (std::exception e) {
			return nullptr;
		}
	}

}