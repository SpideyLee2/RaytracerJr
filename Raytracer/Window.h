#pragma once

#define GLFW_INCLUDE_NONE
#include "glfw/glfw3.h"

namespace mtn {

	struct WindowSettings {
		int width = 1000;
		int height = 800;
		const char* title = "Raytracer Jr";
	};

	class Window {
	public:
		Window(const WindowSettings& settings = WindowSettings());
		~Window() = default;

		void destroy();
		bool shouldClose();
		void setVsync(const bool b) const;

		// Set GLFW callback functions
		void setFramebufferSizeCallback(GLFWframebuffersizefun fun) const;
		void setCursorPosCallback(GLFWcursorposfun fun) const;
		void setScrollCallback(GLFWscrollfun fun) const;
		void setKeyCallback(GLFWkeyfun fun) const;
		void setWindowSizeCallback(GLFWwindowsizefun fun) const;
		void setWindowPosCallback(GLFWwindowposfun fun) const;
		void setWindowMaximizeCallback(GLFWwindowmaximizefun fun) const;
		void setInputMode(const int mode, const int value);

		GLFWwindow* handle() const;

		WindowSettings windowSettings;
		bool isFullscreen = false;
		float aspectRatio;

	private:
		GLFWwindow* pWindowHandle_;
	};

}