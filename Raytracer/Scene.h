#pragma once

#include <glm/glm.hpp>

#include <vector>

enum class MaterialType : int {
	NONE = 0,
	LAMBERTIAN,
	METALLIC,
	DIELECTRIC
};

struct Material {
public:
	Material() : id_(nextId_) { ++nextId_; }

	std::string name = "Material";
	MaterialType matType = MaterialType::LAMBERTIAN;
	glm::vec3 albedo{ 1.0f };

	float metallicness = 0.0f;
	glm::vec3 emissionColor{ 1.0f };
	float emissionStrength = 0.0f;
	float refractiveIndex = 1.0f;

	inline const static float RI_AIR = 1.0f;
	inline const static float RI_WATER = 1.333f;
	inline const static float RI_ICE = 1.31f;
	inline const static float RI_GLASS = 1.52f;
	inline const static float RI_DIAMOND = 2.417f;

	glm::vec3 getEmission() const { return emissionColor * emissionStrength; }

	inline uint32_t getId() const { return id_; }

private:
	uint32_t id_ = 0;
	inline static uint32_t nextId_ = 0;
};

struct Sphere {
public:
	Sphere() : id_(nextId_) { ++nextId_; }

	glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
	float radius = 0.5f;
	uint8_t matIdx = 0;

	inline uint32_t getId() const { return id_; }

private:
	uint32_t id_ = 0;
	inline static uint32_t nextId_ = 0;
};

struct Scene {
	std::vector<Sphere> spheres;
	std::vector<Material> materials;

	inline const std::vector<std::string>& getIdStrList() { return idList_; }
	inline const std::vector<std::string>& getMatStrList() { return matList_; }

	void generateIdStrList() {
		idList_.clear();
		idList_.reserve(spheres.size());

		for (const Sphere& sphere : spheres) {
			idList_.push_back(std::to_string(sphere.getId()));
		}
	}

	void generateMatStrList() {
		matList_.clear();
		matList_.reserve(materials.size());

		for (const Material& mat : materials) {
			matList_.push_back(mat.name);
		}
	}

private:
	std::vector<std::string> idList_;
	std::vector<std::string> matList_;
};

