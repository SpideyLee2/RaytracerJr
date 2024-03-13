#include "Application.h"

int main() {
	Application* app = Application::start();

	while (app->shouldRun) {
		app->run();
	}

	app->shutdown();

	return 0;
}