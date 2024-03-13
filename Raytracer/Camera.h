#pragma once

#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace mtn {

	enum class CameraMovement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	class Camera {
	public:
		Camera(float verticalFov = 60.0f, float nearClip = 0.1f, float farClip = 100.0f);

		inline const glm::mat4& getProjection() const { return projection_; }
		inline const glm::mat4& getInverseProjection() const { return inverseProjection_; }
		inline const glm::mat4& getView() const { return view_; }
		inline const glm::mat4& getInverseView() const { return inverseView_; }

		inline const glm::vec3& getPosition() const { return position_; }
		inline const glm::vec3& getDirection() const { return forwardDir_; }

		inline const std::vector<glm::vec3>& getRayDirections() const { return rayDirections_; }

		inline bool movedThisFrame() { return moved_; }

		bool update(float deltaTime);
		bool resize(uint32_t width, uint32_t height);

	private:
		void recalculateProjection();
		void recalculateView();
		void recalculateRayDirections();

		glm::vec3 position_{ 0.0f, 0.0f, 5.0f };
		glm::vec3 forwardDir_{ 0.0f, 0.0f, -1.0f };

		glm::mat4 view_{ 1.0f };
		glm::mat4 inverseView_{ 1.0f };
		glm::mat4 projection_{ 1.0f };
		glm::mat4 inverseProjection_{ 1.0f };

		float fov_ = 60.0f;
		float fovMin_ = 10.0f, fovMax_ = 90.0f;
		float nearClip_ = 0.1f, farClip_ = 100.0f;

		float pitch_ = 0.0f, yaw_ = -90.0f;
		float pitchMin_ = -89.0f, pitchMax_ = 89.0f;

		float mouseSensitivity_ = 0.05f;
		float speed_ = 2.0f;

		glm::vec2 lastMousePos_{ 0.0f, 0.0f };

		std::vector<glm::vec3> rayDirections_;
		bool moved_ = false;

		uint32_t viewportWidth_ = 0, viewportHeight_ = 0;
	};

}