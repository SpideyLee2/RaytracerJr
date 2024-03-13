#pragma once

#include <string>

namespace mtn {

	class Shader {
	public:
		Shader(const char* vertexShaderDir, const char* fragmentShaderDir);

		unsigned int getId();
		void use() const;

	private:
		void loadShaderSource(std::string& source, const char* shaderDir);
		void checkShaderComp(const unsigned int& shader, const char* const type);
		void checkProgramLink(const unsigned int& program, const char* const type);

		unsigned int m_Id;
	};

}