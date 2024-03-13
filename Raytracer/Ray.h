#pragma once

#include <glm/glm.hpp>

struct Ray {
	glm::vec3 origin{ 0.0f };
	glm::vec3 dir{ 0.0f };
};

struct HitData {
	float hitDistance = 0;
	glm::vec3 worldPos{ 0.0f };
	glm::vec3 worldNormal{ 0.0f };

	uint32_t objIdx = 0;
};