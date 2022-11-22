#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>

#include <main.h>
#include <glutil.cpp>

GLFWwindow* init()
{
	GLFWwindow* window = initializeWindow();
	return window;
}

int main()
{
	GLFWwindow* window = init();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void draw()
{
	// Clear the screen
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}