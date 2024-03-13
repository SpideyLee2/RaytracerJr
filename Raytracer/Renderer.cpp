#include "Renderer.h"

#include "Logger.h"
#include "Shader.h"
#include "Random.h"

#include "glm/gtc/type_ptr.hpp"
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <iostream>
#include <execution>

namespace mtn {

	namespace utils {

		uint32_t rgbaToColor32(const glm::vec4& color) {
			return (uint8_t)(color.r * 255.0f) |
				(uint8_t)(color.g * 255.0f) << 8 |
				(uint8_t)(color.b * 255.0f) << 16 |
				(uint8_t)(color.a * 255.0f) << 24;
		}

		inline glm::vec3 correctGamma(const glm::vec3& color) {
			return glm::pow(color, glm::vec3(1.0f / 2.2f));
		}

		bool nearZero(const glm::vec3& dir) {
			float nz = (float)1e-8;
			return fabs(dir.x) < nz && fabs(dir.y) < nz && fabs(dir.z) < nz;
		}

	}

	void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
							  GLsizei length, const GLchar* message,
							  const void* userParam);

	Renderer::Renderer(Window* const window) 
		: pWindow_(window), clearColor_(0.05f, 0.05f, 0.05f) {

		initGlad();
		initImGui();
	}

	void Renderer::startFrame(float dt) {
		deltaTime_ = dt;

		glfwPollEvents();

		glClearColor(clearColor_.r, clearColor_.g, clearColor_.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Renderer::render(Scene* scene, Camera* camera) {
		// Makes the primary window dockable
		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		pCamera_ = camera;
		pScene_ = scene;

		onRender();

		//ImGui::ShowDemoWindow();

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// Platform functions may change the current OpenGL context, so we save/restore it to make 
		// it easier to paste this code elsewhere.
		if (imguiIO_->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			//GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			//glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Renderer::endFrame() {
		glfwSwapBuffers(pWindow_->handle());
	}

	void Renderer::setWindow(Window* const window) {
		pWindow_ = window;
	}

	void Renderer::setWireframeMode(bool b) {
		wireframeOn = b;
		wireframeOn ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
					: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::initGlad() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			Logger::trace("Failed to load Glad");
			glfwTerminate();
			exit(-1);
		}
	}

	void Renderer::initImGui() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		imguiIO_ = &ImGui::GetIO(); (void)imguiIO_;
		imguiIO_->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		imguiIO_->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		imguiIO_->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (imguiIO_->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(pWindow_->handle(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void Renderer::onRender() {
		Logger::trace("Renderer::onRenderUi()");

		ImGui::Begin("Settings");

		ImGui::Text("Last Frame Time: %fms (%d fps)", deltaTime_ * 1000, (int)(1 / deltaTime_));

		//shouldRender_ = false;

		if (ImGui::Button("Render")) {
			Logger::debug("Beginning rendering");
			shouldRender_ = true;
		}
		if (shouldRender_) {
			renderImage();
		}

		ImGui::Checkbox("Accumulate", &settings_.accumulate);
		ImGui::Checkbox("Gamma Correct", &settings_.gammaCorrect);
		ImGui::Checkbox("Multithread", &settings_.multithread);
		ImGui::Checkbox("Skylight", &settings_.skylight);

		if (ImGui::Button("Reset")) {
			Logger::debug("Resetting accumulated image data");
			resetFrameIndex();
		}

		ImGui::End(); // Scene

		ImGui::Begin("Scene");

		ImGui::Text("Sphere Settings");

		static unsigned int currSphereIdx = 0;
		const auto& idList = pScene_->getIdStrList();
		if (ImGui::BeginCombo("Sphere Idx", idList[currSphereIdx].c_str())) {
			for (unsigned int n = 0; n < idList.size(); n++) {
				const bool isSelected = (currSphereIdx == n);
				if (ImGui::Selectable(idList[n].c_str(), isSelected)) {
					currSphereIdx = n;
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		Sphere& sphere = pScene_->spheres[currSphereIdx];
		ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f);
		ImGui::DragFloat("Radius", &sphere.radius, 0.1f);

		const auto& matList = pScene_->getMatStrList();
		if (ImGui::BeginCombo("Material Idx", matList[sphere.matIdx].c_str())) {
			for (unsigned int n = 0; n < matList.size(); n++) {
				const bool isSelected = (sphere.matIdx == n);
				if (ImGui::Selectable(matList[n].c_str(), isSelected)) {
					sphere.matIdx = n;
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();
		ImGui::Text("Material Settings");

		Material& material = pScene_->materials[sphere.matIdx];
		int materialType = (int)material.matType;
		ImGui::SliderInt("Material Type", &materialType, 0, 3);
		material.matType = MaterialType(materialType);
		ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
		ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.emissionColor));
		ImGui::DragFloat("Emission Strength", &material.emissionStrength, 0.01f, 0.0f, FLT_MAX);
		ImGui::DragFloat("Metallicness", &material.metallicness, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("Refractive Index", &material.refractiveIndex, 0.001f, 1.0f, 3.0f);

		ImGui::End(); // Scene

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar);

		viewportWidth_ = (uint32_t)ImGui::GetContentRegionAvail().x;
		viewportHeight_ = (uint32_t)ImGui::GetContentRegionAvail().y;

		if (pFinalImage_) {
			ImGui::Image((void*)(GLuint*)pFinalImage_->getId(),
						 ImVec2((float)pFinalImage_->getWidth(), (float)pFinalImage_->getHeight()),
						 ImVec2(0, 1), ImVec2(1, 0)); // Reverse x-axis and y-axis
		}

		ImGui::End(); // Viewport
		ImGui::PopStyleVar();
	}

	void Renderer::renderImage() {
		Logger::trace("Renderer::render()");

		onResize(viewportWidth_, viewportHeight_);

		if (frameIndex_ == 1) {
			// Sets all values in the accumulated image data to 0
			memset(pAccumulatedImageData_.get(), 0,
				   pFinalImage_->getWidth() * pFinalImage_->getHeight() * sizeof(glm::vec4));
		}

		// for_each is just a really easy way to parallelize work for each pixel
		// std::execution::par_unseq = parallel, unsequential processing
		if (settings_.multithread) {
			std::for_each(std::execution::par_unseq, imageVerticalItr_.begin(), imageVerticalItr_.end(),
				[this](uint32_t y) {
				std::for_each(std::execution::par_unseq, imageHorizontalItr_.begin(), imageHorizontalItr_.end(),
					[this, y](uint32_t x) {
					glm::vec4 pixelColor = perPixel(x, y);

					pAccumulatedImageData_[x + (y * pFinalImage_->getWidth())] += pixelColor;
					glm::vec4 accumulatedColor = pAccumulatedImageData_[x + y * pFinalImage_->getWidth()];
					accumulatedColor /= (float)frameIndex_;

					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));

					pImageData_[x + (y * pFinalImage_->getWidth())] = utils::rgbaToColor32(accumulatedColor);
				});
			});
		}
		else {
			for (uint32_t y = 0; y < pFinalImage_->getHeight(); ++y) {
				for (uint32_t x = 0; x < pFinalImage_->getWidth(); ++x) {
					glm::vec4 pixelColor = perPixel(x, y);

					pAccumulatedImageData_[x + (y * pFinalImage_->getWidth())] += pixelColor;
					glm::vec4 accumulatedColor = pAccumulatedImageData_[x + y * pFinalImage_->getWidth()];
					accumulatedColor /= (float)frameIndex_;

					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));

					pImageData_[x + (y * pFinalImage_->getWidth())] = utils::rgbaToColor32(accumulatedColor);
				}
			}
		}


		pFinalImage_->setData(pImageData_);

		// Update frame index depending on whether accumulation is enabled
		if (settings_.accumulate) {
			++frameIndex_;
		}
		else {
			resetFrameIndex();
		}
	}

	void Renderer::onResize(uint32_t width, uint32_t height) {
		bool resized = false;
		if (pFinalImage_) {
			if (width == pFinalImage_->getWidth() && height == pFinalImage_->getHeight()) {
				return;
			}
			resized = pFinalImage_->resize(width, height);
		}
		else {
			pFinalImage_ = std::make_unique<Texture2D>(width, height);
		}

		pImageData_ = std::shared_ptr<uint32_t[]>(new uint32_t[width * height]);
		pAccumulatedImageData_ = std::unique_ptr<glm::vec4[]>(new glm::vec4[width * height]);

		pCamera_->resize(width, height);

		imageHorizontalItr_.resize(width);
		imageVerticalItr_.resize(height);

		// Initialize horizontal and vertical pixel iterators
		for (uint32_t i = 0; i < width; ++i) {
			imageHorizontalItr_[i] = i;
		}
		for (uint32_t i = 0; i < height; ++i) {
			imageVerticalItr_[i] = i;
		}
	}

	glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y) {
		// Initial ray starting at the camera's center, directed based on the pixel index
		Ray ray;
		ray.origin = pCamera_->getPosition();
		ray.dir = pCamera_->getRayDirections()[x + y * pFinalImage_->getWidth()];

		glm::vec3 totalLight(0.0f);
		glm::vec3 contribution(1.0f);

		// Seed could probably be better, but it gets the job done
		uint32_t seed = (x + y * pFinalImage_->getWidth()) * frameIndex_;

		const int NUM_BOUNCES = 16;
		for (int i = 0; i < NUM_BOUNCES; i++) {
			seed += i;

			HitData hitData = traceRay(ray);

			// If we miss all objects in the scene, the sky color is added to the pixel color and
			// we break out of the bounce loop
			if (hitData.hitDistance < 0.0f) {
				if (settings_.skylight) {
					totalLight += skyLight * contribution;
				}
				break;
			}

			const Sphere& sphere = pScene_->spheres[hitData.objIdx];
			const Material& material = pScene_->materials[sphere.matIdx];

			// Small offset of pos along hit sphere's normal depending on the material to prevent
			// Note: We can't hit the inside of spheres currently unless the material is dielectric, 
			// so this solution is passable.
			if (material.matType != MaterialType::DIELECTRIC) {
				ray.origin = hitData.worldPos - ray.dir * 1e-3f;
			}

			if (material.matType == MaterialType::LAMBERTIAN) {
				// Absorbs all the light of the material's albedo.
				contribution *= material.albedo;

				// Randomly scatter from the hit normal
				glm::vec3 scattered = hitData.worldNormal + Random::inUnitSphere(seed);

				if (utils::nearZero(scattered)) {
					ray.dir = hitData.worldNormal;
				}

				ray.dir = glm::normalize(scattered);
			}
			else if (material.matType == MaterialType::METALLIC) {
				glm::vec3 reflected = glm::reflect(ray.dir, hitData.worldNormal);
				// Randomize the direction of the reflected ray based on the material's metallicness
				ray.dir = glm::normalize(reflected + material.metallicness * Random::inUnitSphere(seed));

				// Absorb all light if the ray scatters below the surface
				if (glm::dot(ray.dir, hitData.worldNormal) > 0.0f) {
					contribution *= material.albedo;
				}
			}

			else if (material.matType == MaterialType::DIELECTRIC) {
				// Dielectric materials absorb no light
				//contribution *= glm::vec3{ 1.0f };

				// Refractive index is inversed when hitting the outside of a sphere
				float refractionRatio = glm::dot(-ray.dir, hitData.worldNormal) < 0.0f
					? material.refractiveIndex				// Hit the back face
					: (1.0f / material.refractiveIndex);	// Hit the front face

				// Total Internal Reflection:
				// If the ray is moving from a medium with a higher refractive index to one that's
				// lower, then the ray cannot refract, since there would be no solution to Snell's Law.
				float cosTheta = std::min(glm::dot(-ray.dir, hitData.worldNormal), 1.0f);
				float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
				bool canRefract = refractionRatio * sinTheta <= 1.0f;

				if (canRefract && schlickReflectance(cosTheta, refractionRatio) <= Random::rFloat(seed)) {
					// Refract using Snell's Law
					ray.dir = glm::normalize(glm::refract(ray.dir, hitData.worldNormal, refractionRatio));
					ray.origin = hitData.worldPos + ray.dir * 1e-3f;
				}
				else {
					ray.dir = glm::normalize(glm::reflect(ray.dir, hitData.worldNormal));
				}
			}

			totalLight += contribution * material.getEmission();
		}

		return glm::vec4(utils::correctGamma(totalLight), 1.0f);
	}

	/*
	* Ray intersection formula (for a circle at (0,0), for simplicity):
	* Simply the equation for a point on a ray (a_xy + b_xy*t) plugged into the quadratic
	* equation (x^2 + y^2 + z^2 - r^2 = 0)
	*
	* (b_x^2 + b_y^2)t^2 + (2(a_x*b_x + a_y*b_y))t + (a_x^2 + a_y^2 - r^2) = 0
	* at^2 + 2bt + c = 0
	*
	* a = ray origin
	* b = ray direction
	* r = radius
	* t = hit distance
	*/

	HitData Renderer::traceRay(const Ray& ray) {
		int closestSphereIdx = -1;

		float closestHitDistance = std::numeric_limits<float>::max();

		for (size_t i = 0; i < pScene_->spheres.size(); ++i) {
			const Sphere& sphere = pScene_->spheres[i];
			// Shifting the origin effectively moves the sphere into position
			glm::vec3 origin = ray.origin - sphere.pos;

			float a = glm::dot(ray.dir, ray.dir);
			float halfB = glm::dot(origin, ray.dir);
			float c = glm::dot(origin, origin) - (sphere.radius * sphere.radius);

			// Use the discriminant to check if there is an intersection
			float discriminant = (halfB * halfB) - (a * c);

			if (discriminant < 0) {
				continue;
			}

			float closestT = (-halfB - sqrt(discriminant)) / a;
			// closestT > 0 prevents redrawing spheres that don't actually exist
			if (closestT > 1e-8f && closestT < closestHitDistance) {
				closestSphereIdx = (int)i;
				closestHitDistance = closestT;
				continue;
			}
			//closestT = (-halfB + sqrt(discriminant)) / a;
			//if (closestT > 1e-8f && closestT < closestHitDistance) {
			//	closestSphereIdx = (int)i;
			//	closestHitDistance = closestT;
			//	continue;
			//}
		} // end loop

		if (closestSphereIdx < 0) {
			return miss(ray);
		}

		return closestHit(ray, closestHitDistance, closestSphereIdx);
	}

	HitData Renderer::closestHit(const Ray& ray, float hitDistance, int objIdx) {
		const Sphere& sphere = pScene_->spheres[objIdx];

		glm::vec3 hitPos = ray.origin + ray.dir * hitDistance; // a + bt

		HitData hitData;
		hitData.hitDistance = hitDistance;
		hitData.worldPos = hitPos;
		hitData.worldNormal = glm::normalize(hitPos - sphere.pos);
		hitData.objIdx = objIdx;
		
		return hitData;
	}

	HitData Renderer::miss(const Ray& ray) {
		HitData missData;
		missData.hitDistance = -1.0f;
		return missData;
	}

	// Schlick Reflectance approximation for reflectivity or refractive surfaces at steep viewing angles
	float Renderer::schlickReflectance(float cosine, float refIdx) {
		float r0 = (1.0f - refIdx) / (1.0f + refIdx);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * (float)std::pow((1.0f - cosine), 5);
	}

	void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
							  GLsizei length, const GLchar* message,
							  const void* userParam) {
		const char* const srcStr = [source]() {
			switch (source) {
				case GL_DEBUG_SOURCE_API: return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
				case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
				case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
				case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
				case GL_DEBUG_SOURCE_OTHER: return "OTHER";
				default: return "?";
			}
		}();

		const char* const typeStr = [type]() {
			switch (type) {
				case GL_DEBUG_TYPE_ERROR: return "ERROR";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED BEHAVIOR";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED BEHAVIOR";
				case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
				case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
				case GL_DEBUG_TYPE_MARKER: return "MARKER";
				case GL_DEBUG_TYPE_OTHER: return "OTHER";
				default: return "?";
			}
		}();

		const char* const severityStr = [severity]() {
			switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
				case GL_DEBUG_SEVERITY_LOW: return "LOW";
				case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
				case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
				default: return "?";
			}
		}();

		std::cout << srcStr << "-" << typeStr << "-" << severityStr << " (" << id << "): "
			<< message << '\n';
	}

}