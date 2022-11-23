#include <main.h>

#include <algorithm>
#include <string>
#include <vector>

#include <glutil.cpp>
#include <systemManager.h>
#include <renderSystem.h>
#include <entitySystem.h>

using namespace std;

// Show memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

void init()
{
	// Init OpenGL
	initializeWindow();

	// Init systems
	systemManager::registerSystems();
}

int main()
{
	init();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		systemManager::updateSystems();
		draw();

		glfwSwapBuffers(window);
	}

	systemManager::deleteAllSystems();
	glfwTerminate();

	// Show memory leaks
	_CrtDumpMemoryLeaks();

	exit(EXIT_SUCCESS);
}

void draw()
{
	// Clear the screen
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}