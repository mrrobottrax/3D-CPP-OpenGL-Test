#include <pch.h>

#include <main.h>
#include <gl/glutil.h>

// Show memory leaks
#define _CRTDBG_MAP_ALLOC

int main()
{   
	InitializeWindow();
	//InitializeOpenGL();

	/*
	InitializeWindow();
	InitializeOpenGL();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);
	}
	*/

	glfwTerminate();

	// Show memory leaks
	_CrtDumpMemoryLeaks();

	exit(EXIT_SUCCESS);
}