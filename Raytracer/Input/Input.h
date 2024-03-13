#pragma once

#include "../Window.h"
#include "Keys.h"

#include <glm/glm.hpp>

namespace mtn {

	class Input {
	public:
		Input() = default;
		Input(Window* window);

		static glm::vec2 getCursorPos();

		static bool isMouseButtonDown(MouseButton mb);
		static bool isKeyDown(Key k);

		static void setCursorMode(CursorMode cm);

	private:
		static GLFWwindow* window_;
	};

}