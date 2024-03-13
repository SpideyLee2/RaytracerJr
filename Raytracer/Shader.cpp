#include "Shader.h"
#include "Logger.h"

#include <glad.h>

#include <sstream>
#include <fstream>

namespace mtn {

	Shader::Shader(const char* vertexShaderDir, const char* fragmentShaderDir) {
		// Loads the source files into cstrings
		std::string vSourceString = "";
		std::string fSourceString = "";
		loadShaderSource(vSourceString, vertexShaderDir);
		loadShaderSource(fSourceString, fragmentShaderDir);
		const char* vSource = vSourceString.c_str();
		const char* fSource = fSourceString.c_str();

		// Creates and compiles the shaders
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vSource, NULL);
		glCompileShader(vs);
		checkShaderComp(vs, "VERTEX");

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fSource, NULL);
		glCompileShader(fs);
		checkShaderComp(fs, "FRAGMENT");

		// Creates and links the shaders to the shader program
		m_Id = glCreateProgram();
		glAttachShader(m_Id, vs);
		glAttachShader(m_Id, fs);
		glLinkProgram(m_Id);
		checkProgramLink(m_Id, "SHADER");

		// Deletes shaders, since they are already linked in the program
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	unsigned int Shader::getId() {
		return m_Id;
	}

	void Shader::use() const {
		glUseProgram(m_Id);
	}

	void Shader::checkShaderComp(const unsigned int& shader, const char* const type) {
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, 0, infoLog);
			Logger::error("{} shader failed compilation!\n{}\n", type, infoLog);
		}
	}

	void Shader::checkProgramLink(const unsigned int& program, const char* const type) {
		int success;
		char infoLog[512];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, 0, infoLog);
			Logger::error("{} program failed linking!\n{}\n", type, infoLog);
		}
	}

	void Shader::loadShaderSource(std::string& sourceString, const char* shaderDir) {
		std::stringstream source;
		try {
			std::ifstream file;
			file.open(shaderDir);
			source << file.rdbuf();
			file.close();
			sourceString = source.str();
		}
		catch (std::ifstream::failure e) {
			Logger::fatal("Failed to read shader source file at {}\n", shaderDir);
			exit(1);
		}
	}
}