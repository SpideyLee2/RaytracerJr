#include "Window.h"
#include "Logger.h"

#include "glad.h"

#include <iostream>

namespace mtn {

	Window::Window(const WindowSettings& settings) : windowSettings(settings) {
		if (!glfwInit()) {
			Logger::fatal("Failed to initialize GLFW library.");
			exit(-1);
		}

		glfwSetErrorCallback([](int error, const char* description) {
			Logger::error("GLFW: ({}): {}", error, description);
		});

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		pWindowHandle_ = glfwCreateWindow(settings.width, settings.height, settings.title, 
										   nullptr, nullptr);
		aspectRatio = settings.width / (float)settings.height;

		if (!pWindowHandle_) {
			Logger::fatal("Failed to create GLFW window.");
			glfwTerminate();
			exit(-1);
		}

		glfwMakeContextCurrent(pWindowHandle_);
	}

	GLFWwindow* Window::handle() const {
		return pWindowHandle_;
	}

	void Window::destroy() {
		glfwDestroyWindow(pWindowHandle_);
	}

	bool Window::shouldClose() {
		return glfwWindowShouldClose(pWindowHandle_);
	}

	void Window::setVsync(const bool b) const {
		glfwSwapInterval(b);
	}

	void Window::setFramebufferSizeCallback(GLFWframebuffersizefun fun) const {
		glfwSetFramebufferSizeCallback(pWindowHandle_, fun);
	}

	void Window::setCursorPosCallback(GLFWcursorposfun fun) const {
		glfwSetCursorPosCallback(pWindowHandle_, fun);
	}

	void Window::setScrollCallback(GLFWscrollfun fun) const {
		glfwSetScrollCallback(pWindowHandle_, fun);
	}

	void Window::setKeyCallback(GLFWkeyfun fun) const {
		glfwSetKeyCallback(pWindowHandle_, fun);
	}

	void Window::setWindowSizeCallback(GLFWwindowsizefun fun) const {
		glfwSetWindowSizeCallback(pWindowHandle_, fun);
	}

	void Window::setWindowPosCallback(GLFWwindowposfun fun) const {
		glfwSetWindowPosCallback(pWindowHandle_, fun);
	}

	void Window::setWindowMaximizeCallback(GLFWwindowmaximizefun fun) const {
		glfwSetWindowMaximizeCallback(pWindowHandle_, fun);
	}

	void Window::setInputMode(const int mode, const int value) {
		glfwSetInputMode(pWindowHandle_, mode, value);
	}

}