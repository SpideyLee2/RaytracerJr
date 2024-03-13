#pragma once

#include <random>

#include <glm/glm.hpp>

namespace mtn {

	class Random {
	public:
		template <class T>
		inline static T rInt(T a = std::numeric_limits<T>::min(), T b = std::numeric_limits<T>::max()) {
			return std::uniform_int_distribution<T>(a, b)(rng_);
		}
		
		static int rInt(uint32_t& seed) {
			seed = pcgHash(seed);
			return seed / (int)std::numeric_limits<uint32_t>::max();
		}

		static float rFloat(uint32_t& seed) {
			seed = pcgHash(seed);
			return seed / (float)std::numeric_limits<uint32_t>::max();
		}

		inline static glm::vec2 vec2(float a, float b) {
			return glm::vec2(std::uniform_real_distribution<float>(a, b)(rng_),
							 std::uniform_real_distribution<float>(a, b)(rng_));
		}

		inline static glm::vec3 vec3(float a, float b) {
			return glm::vec3(std::uniform_real_distribution<float>(a, b)(rng_),
							 std::uniform_real_distribution<float>(a, b)(rng_),
							 std::uniform_real_distribution<float>(a, b)(rng_));
		}

		inline static glm::vec4 vec4(float a, float b) {
			return glm::vec4(std::uniform_real_distribution<float>(a, b)(rng_),
							 std::uniform_real_distribution<float>(a, b)(rng_),
							 std::uniform_real_distribution<float>(a, b)(rng_),
							 std::uniform_real_distribution<float>(a, b)(rng_));
		}

		static glm::vec3 inUnitSphere(uint32_t& seed) {
			return glm::normalize(glm::vec3(rFloat(seed) * 2.0f - 1.0f,
											rFloat(seed) * 2.0f - 1.0f,
											rFloat(seed) * 2.0f - 1.0f));
		}

		static glm::vec3 inUnitSphereSlow() {
			return glm::normalize(vec3(-1.0f, 1.0f));
		}

	private:
		// PCG Hash is super fast and great for graphics operations
		static uint32_t pcgHash(uint32_t seed) {
			uint32_t state = seed * 747796405u + 2891336453u;
			uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
			return (word >> 22u) * word;
		}

		inline static std::random_device rd_{};
		// thread_local allows each core to have its own instance of the mt engine in its cache.
		// This prevents locking up multithreaded processes when they need to increment the state of
		// the mt engine
		inline static thread_local std::mt19937 rng_{ rd_() };
	};

}