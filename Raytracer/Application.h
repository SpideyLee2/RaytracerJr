#pragma once

#include "Renderer.h"
#include "Window.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Scene.h"

using namespace mtn;

struct AppSettings {
	int screenWidth = 800;
	int screenHeight = 600;
	int refreshRate = 60;
};

class Application {
public:
	Application(const Application&) =		delete;
	Application(Application&&) =			delete;
	void operator=(const Application&) =	delete;
	void operator=(Application&&) =			delete;

	static Application* start();
	void run();
	void shutdown();
	
	AppSettings appSettings;

	Window* window;
	Renderer* renderer;
	Input inputHandler;
	Camera camera;
	Scene scene;

	bool shouldRun;

	float deltaTime, gameTime;
	int gameTimeFactor;

	unsigned int frameCount = 0;
	unsigned int fps = 0;

private:
	Application(const AppSettings& appSettings = AppSettings());
	~Application();

	void windowInit();
	void sceneInit();
	void calcDeltaTime();

	// GLFW callback functions
	friend void keyCallbackFun(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
	friend void windowSizeCallbackFun(GLFWwindow* glfwWindow, int width, int height);
	friend void windowPosCallbackFun(GLFWwindow* window, int xpos, int ypos);

	void onWindowResize(int width, int height);

	inline static Application* spInstance_ = nullptr;

	float currFrameTime_, lastFrameTime_;
};