#include "Application.h"

#include "Logger.h"
#include "Random.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#pragma region Function_Decls
void keyCallbackFun(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
void windowSizeCallbackFun(GLFWwindow* window, int width, int height);
void windowPosCallbackFun(GLFWwindow* window, int xpos, int ypos);
#pragma endregion

Application::Application(const AppSettings& appSettings) 
	: appSettings(appSettings), shouldRun(true), gameTimeFactor(static_cast<int>(1e6)) {

	Logger::trace("Application::Application(const AppSettings&)");

	window = new Window();
	inputHandler = Input(window);
	windowInit();
	renderer = new Renderer(window);
	camera = Camera();
	sceneInit();
}

Application* Application::start() {
	Logger::init();

	Logger::info("Starting application.");
	if (spInstance_ == nullptr) {
		spInstance_ = new Application();
	}
	else {
		Logger::warn("Application already exists! Using existing application instance.");
	}

	return spInstance_;
}

void Application::run() {
	//Logger::trace("Application::run()");

	++frameCount;
	//Logger::trace("Frame Count: {}", frameCount);

	calcDeltaTime();

	// Calculate FPS every 50 frames
	// Check if == 1 so first frame has an fps calc
	if (frameCount % 50 == 1) {
		fps = static_cast<unsigned int>(1 / deltaTime);
	}

	if (camera.update(deltaTime)) {
		renderer->resetFrameIndex();
	}

	renderer->startFrame(deltaTime);
	renderer->render(&scene, &camera);
	renderer->endFrame();

	shouldRun = !window->shouldClose();
}

void Application::shutdown() {
	Logger::trace("Application::shutdown()");
		
	// TODO: Clean up rendering elements

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Cleanup GLFW
	window->destroy();
	glfwTerminate();
}

Application::~Application() {
	Logger::trace("Application::~Application()");
	delete spInstance_;
	spInstance_ = nullptr;
}

void Application::windowInit() {
	Logger::trace("Application::windowInit()");

	if (window->handle() == nullptr) {
		Logger::fatal("Window handle is a nullptr!");
		exit(1);
	}
	//glfwSetWindowUserPointer(window->handle(), spInstance_);
	//window->setFramebufferSizeCallback(framebufferSizeCallbackFun);

	// Set GLFW callbacks
	window->setKeyCallback(keyCallbackFun);
	window->setWindowSizeCallback(windowSizeCallbackFun);
	window->setWindowPosCallback(windowPosCallbackFun);

	// Turn vsync on by default
	window->setVsync(true);
}

void Application::sceneInit() {
	{
		Material& m = scene.materials.emplace_back();
		m.name = "THE SUN";
		m.albedo = { 1.0f, 1.0f, 1.0f };
		m.emissionColor = m.albedo;
		m.emissionStrength = 50.0f;
	}
	{
		Material& m = scene.materials.emplace_back();
		m.name = "Lambertian Blue";
		m.albedo = { 0.2f, 0.3f, 1.0f };
	}
	{
		Material& m = scene.materials.emplace_back();
		m.name = "Green Light";
		m.albedo = { 0.3f, 0.8f, 0.2f };
		m.emissionColor = m.albedo;
		m.emissionStrength = 30.0f;
	}
	{
		Material& m = scene.materials.emplace_back();
		m.name = "Metallic Fuschia";
		m.albedo = { 1.0f, 0.0f, 0.6f };
		m.matType = MaterialType::METALLIC;
		m.metallicness = 0.25f;
	}
	{
		Material& m = scene.materials.emplace_back();
		m.name = "Clear Glass";
		m.matType = MaterialType::DIELECTRIC;
		m.refractiveIndex = Material::RI_GLASS;
	}

	// THE SUN
	{
		Sphere& s = scene.spheres.emplace_back();
		s.pos = { 100.0f, 50.0f, -100.0f };
		s.radius = 20.0f;
		s.matIdx = 0;
	}
	// Ground sphere
	{
		Sphere& s = scene.spheres.emplace_back();
		s.pos = { 0.0f, -10001.0f, 0.0f };
		s.radius = 10000.0f;
		s.matIdx = 1;
	}
	// Other spheres
	{
		Sphere& s = scene.spheres.emplace_back();
		s.pos = { 2.5f, 0.0f, 0.0f };
		s.radius = 1.0f;
		s.matIdx = 2;
	}
	{
		Sphere& s = scene.spheres.emplace_back();
		s.pos = { 0.0f, 0.0f, 0.0f };
		s.radius = 1.0f;
		s.matIdx = 3;
	}
	{
		Sphere& s = scene.spheres.emplace_back();
		s.pos = { -2.5f, 0.0f, 0.0f };
		s.radius = 1.0f;
		s.matIdx = 4;
	}

	// Create randomized spheres with randomized materials
	const uint32_t NUM_RAND_SPHERES = 0;
	for (uint32_t i = 0; i < NUM_RAND_SPHERES; ++i) {
		Material& m = scene.materials.emplace_back();
		Sphere& s = scene.spheres.emplace_back();
		uint32_t seed = i;

		m.albedo = Random::vec3(0.0f, 1.0f);
		m.emissionColor = m.albedo;

		glm::vec2 randPos{ Random::vec2(-1.0f, 1.0f) * 10.0f };
		s.radius = (Random::rFloat(seed) * 0.15f) + 0.05f; // min size 0.05
		s.pos = glm::vec3(randPos.x, s.radius - 1.0f, randPos.y);
		s.matIdx = (uint8_t)scene.materials.size() - 1;

		float rf = Random::rFloat(seed);
		if (rf < 0.7f) {
			// 70% chance a sphere has a lambertian material
			m.matType = MaterialType::LAMBERTIAN;
		}
		else if (rf < 0.8f) {
			// 10% chance a sphere is emissive
			m.matType = MaterialType::LAMBERTIAN;
			m.emissionStrength = (10.0f * Random::rFloat(seed)) + 10.0f; // min 10.0 strength
		}
		else if (rf < 0.9f) {
			// 10% chance a sphere is metallic
			m.matType = MaterialType::METALLIC;
			m.metallicness = Random::rFloat(seed);
		}
		else {
			// 10% chance a sphere is dielectric
			m.matType = MaterialType::DIELECTRIC;
			m.refractiveIndex = Material::RI_GLASS;
		}
	}

	scene.generateIdStrList();
	scene.generateMatStrList();
}

// Delta time and game time calculations
void Application::calcDeltaTime() {
	currFrameTime_ = static_cast<float>(glfwGetTime());

	deltaTime = currFrameTime_ - lastFrameTime_;
	gameTime = deltaTime * gameTimeFactor;

	lastFrameTime_ = currFrameTime_;
}

void keyCallbackFun(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
	Logger::trace("keyCallbackFun()");

	Application* app = Application::spInstance_;

	// Close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		app->shouldRun = false;
	}

	// Fullscreen
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
		if (!app->window->isFullscreen) {
			// Sets the window to display in fullscreen on the primary monitor
			const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			app->appSettings.screenWidth = vidMode->width;
			app->appSettings.screenHeight = vidMode->height;
			app->appSettings.refreshRate = vidMode->refreshRate;
			glfwSetWindowMonitor(glfwWindow, NULL, 0, 0, app->appSettings.screenWidth,
									app->appSettings.screenHeight, app->appSettings.refreshRate);
		}
		else {
			// Sets window to be displayed at 800x600 on the primary monitor
			const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			app->appSettings.screenWidth = 800;
			app->appSettings.screenHeight = 600;
			app->appSettings.refreshRate = vidMode->refreshRate;
			// Resize window and move top-left corner away from edge of screen to prevent 
			// hiding the window bar
			glfwSetWindowMonitor(glfwWindow, NULL, 50, 50, app->appSettings.screenWidth,
									app->appSettings.screenHeight, app->appSettings.refreshRate);
		}
		app->onWindowResize(app->appSettings.screenWidth, app->appSettings.screenHeight);

		app->window->isFullscreen = !app->window->isFullscreen;
	}

	// Enable/Disable primitive edge drawing
	if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
		app->renderer->setWireframeMode(!app->renderer->wireframeOn);
	}
}

void windowSizeCallbackFun(GLFWwindow* window, int width, int height) {
	Logger::trace("windowSizeCallbackFun()");

	Application* app = Application::spInstance_;

	app->onWindowResize(width, height);
}

void windowPosCallbackFun(GLFWwindow* window, int xpos, int ypos) {
	Logger::trace("windowPosCallbackFun()");

	Application* app = Application::spInstance_;

	// Helps keep deltaTime and gameTime from accumulating while the window is moving, since 
	// the application is technically paused.
	app->lastFrameTime_ = static_cast<float>(glfwGetTime());
}

void Application::onWindowResize(int width, int height) {
	window->windowSettings.width = width;
	window->windowSettings.height = height;

	camera.resize(width, height);
}