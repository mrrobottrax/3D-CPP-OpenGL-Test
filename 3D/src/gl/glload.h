#pragma once
#define GLFW_INCLUDE_NONE

#define USE_GLAD

#ifdef USE_GLEW
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif // USE_GLEW

#ifdef USE_GLAD
	#include <glad/glad.h>
#endif // USE_GLEW

#include <GLFW/glfw3.h>