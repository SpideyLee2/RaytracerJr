#include "Input.h"

namespace mtn {

	GLFWwindow* Input::window_ = nullptr;

	Input::Input(Window* window) {
		window_ = window->handle();
	}

	glm::vec2 Input::getCursorPos() {
		double xPos, yPos;
		glfwGetCursorPos(window_, &xPos, &yPos);

		return glm::vec2((float)xPos, (float)yPos);
	}

	bool Input::isMouseButtonDown(MouseButton mb) {
		return glfwGetMouseButton(window_, (int)mb);
	}

	bool Input::isKeyDown(Key k) {
		return glfwGetKey(window_, (int)k);
	}

	void Input::setCursorMode(CursorMode cm) {
		glfwSetInputMode(window_, GLFW_CURSOR, (int)cm);
	}

}