#pragma once

#include "Window.h"
#include "Drawable.h"
#include "ShaderPool.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include "glad.h"
#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <memory>

// This is going to be a huge class that controls all of the rendering of the engine.
// EnTT will do most of the heavy lifting in regards to keep this somewhat clean, but it will be
// flooded with GL functions, more than likely.

namespace mtn {

	struct RendererSettings {
		bool accumulate = true;
		bool gammaCorrect = true;
		bool multithread = true;
		bool skylight = true;
	};

	class Renderer {
	public:
		Renderer() = delete;
		Renderer(Window* const window);

		void startFrame(float dt);
		void render(Scene* scene, Camera* camera);
		void endFrame();
		
		void setWindow(Window* const window);
		void setWireframeMode(bool b);

		inline void resetFrameIndex() { frameIndex_ = 1; }

		bool wireframeOn = false;

	private:
		void initGlad();
		void initImGui();

		void onRender();
		void renderImage();
		void onResize(uint32_t width, uint32_t height);

		// Like RayGen in DirectX and Vulkan
		glm::vec4 perPixel(uint32_t x, uint32_t y);

		HitData traceRay(const Ray& ray);
		HitData closestHit(const Ray& ray, float hitDistance, int objIdx);
		HitData miss(const Ray& ray);

		float schlickReflectance(float cosine, float refIdx);

		float deltaTime_ = 0.0f;

		bool shouldRender_ = false;

		Window* pWindow_ = nullptr;
		glm::vec3 clearColor_{ 0.2f, 0.4f, 0.4f };

		glm::mat4 view_;
		glm::mat4 projection_;

		ImGuiIO* imguiIO_ = nullptr;

		std::unique_ptr<Texture2D> pFinalImage_ = nullptr;
		std::shared_ptr<uint32_t[]> pImageData_ = nullptr;

		bool accumulate_ = true;
		std::unique_ptr<glm::vec4[]> pAccumulatedImageData_ = nullptr;

		std::vector<uint32_t> imageHorizontalItr_, imageVerticalItr_;

		glm::vec3 skyLight{ 0.6f, 0.75f, 1.0f };
		glm::vec3 skyLightBrightness{ 1.0f };

		uint32_t frameIndex_ = 1;
		RendererSettings settings_;

		Camera* pCamera_ = nullptr;
		Scene* pScene_ = nullptr;

		uint32_t viewportWidth_ = 0;
		uint32_t viewportHeight_ = 0;
	};

}