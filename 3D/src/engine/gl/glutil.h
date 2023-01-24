#pragma once

#include <engine/gl/gl.h>

#include <engine/renderSystem.h>
#include <engine/systemManager.h>
#include <engine/inputManager.h>

extern GLFWwindow* window;
extern const char* glsl_version;

void error_callback(int, const char*);
void key_callback(GLFWwindow*, int, int, int, int);
void window_size_callback(GLFWwindow*, int, int);
void initializeWindow();

GLuint CreateShader(GLenum, const std::string&);
GLuint CreateProgram(const std::vector<GLuint>&);