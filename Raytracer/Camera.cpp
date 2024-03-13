#include "Camera.h"

#include "Input/Input.h";
#include "Logger.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

mtn::Camera::Camera(float verticalFov, float nearClip, float farClip) 
	: fov_(verticalFov), nearClip_(nearClip), farClip_(farClip) {}

bool mtn::Camera::update(float deltaTime) {
	glm::vec2 mousePos = Input::getCursorPos();
	glm::vec2 deltaPos = mousePos - lastMousePos_;
	lastMousePos_ = mousePos;

	bool moved = false;

	if (!Input::isMouseButtonDown(MouseButton::RIGHT)) {
		// Keep the cursor unlocked
		Input::setCursorMode(CursorMode::NORMAL);
	}
	else {
		// Lock the cursor to the screen when the right mouse button is pressed, for rotating
		Input::setCursorMode(CursorMode::LOCKED);

		// Camera Rotation
		if (deltaPos.x != 0.0f || deltaPos.y != 0.0f) {
			yaw_ += deltaPos.x * mouseSensitivity_;
			pitch_ += deltaPos.y * mouseSensitivity_;
			pitch_ = std::clamp(pitch_, pitchMin_, pitchMax_);

			forwardDir_ = glm::vec3(cos(glm::radians(yaw_) * cos(glm::radians(pitch_))),
									sin(glm::radians(-pitch_)),
									sin(glm::radians(yaw_) * cos(glm::radians(pitch_))));
			forwardDir_ = glm::normalize(forwardDir_);

			moved = true;
		}
	}

	glm::vec3 upDir{ 0.0f, 1.0f, 0.0f };
	glm::vec3 rightDir = glm::cross(forwardDir_, upDir);

	// Camera Movement
	if (Input::isKeyDown(Key::W)) {
		position_ += forwardDir_ * speed_ * deltaTime;
		moved = true;
	}
	else if (Input::isKeyDown(Key::S)) {
		position_ -= forwardDir_ * speed_ * deltaTime;
		moved = true;
	}
	if (Input::isKeyDown(Key::A)) {
		position_ -= rightDir * speed_ * deltaTime;
		moved = true;
	}
	else if (Input::isKeyDown(Key::D)) {
		position_ += rightDir * speed_ * deltaTime;
		moved = true;
	}
	if (Input::isKeyDown(Key::SPACE)) {
		position_ += upDir * speed_ * deltaTime;
		moved = true;
	}
	else if (Input::isKeyDown(Key::LEFT_CONTROL)) {
		position_ -= upDir * speed_ * deltaTime;
		moved = true;
	}

	if (moved) {
		recalculateView();
		recalculateRayDirections();
	}

	return moved;
}

bool mtn::Camera::resize(uint32_t width, uint32_t height) {
	if (width == viewportWidth_ && height == viewportHeight_) {
		return false;
	}

	viewportWidth_ = width;
	viewportHeight_ = height;

	recalculateView();
	recalculateProjection();
	recalculateRayDirections();
	
	return true;
}

void mtn::Camera::recalculateProjection() {
	projection_ = glm::perspective(glm::radians(fov_), viewportWidth_ / (float)viewportHeight_, nearClip_, farClip_);
	inverseProjection_ = glm::inverse(projection_);
}

void mtn::Camera::recalculateView() {
	view_ = glm::lookAt(position_, position_ + forwardDir_, glm::vec3(0.0f, 1.0f, 0.0f));
	inverseView_ = glm::inverse(view_);
}

void mtn::Camera::recalculateRayDirections() {
	rayDirections_.resize(viewportWidth_ * viewportHeight_);

	// Get coordinates and convert from screen space back to world space
	for (uint32_t y = 0; y < viewportHeight_; ++y) {
		for (uint32_t x = 0; x < viewportWidth_; ++x) {
			uint32_t index = x + y * viewportWidth_;
			glm::vec2 coord{ x / (float)viewportWidth_, y / (float)viewportHeight_ };

			// Convert coord from [0, 1] to NDC
			coord = coord * 2.0f - 1.0f;

			// Convert coord from clip space to view space
			// *** Step 1: Apply the inverse projection to the coord
			glm::vec4 target = inverseProjection_ * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
			// *** Step 2: Perform the perspective divide
			target = glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0.0f);

			// Convert coord from view space to world space
			glm::vec3 rayDirection = glm::vec3(inverseView_ * target);

			rayDirections_[index] = rayDirection;
		}
	}
}
