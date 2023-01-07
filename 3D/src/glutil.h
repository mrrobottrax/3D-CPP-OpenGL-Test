#pragma once
#include <gl.h>

#include <iostream>
#include <renderSystem.h>
#include <systemManager.h>
#include <vector>
#include <inputManager.h>

extern GLFWwindow* window;

void error_callback(int, const char*);
void key_callback(GLFWwindow*, int, int, int, int);
void window_size_callback(GLFWwindow*, int, int);
void initializeWindow();

GLuint CreateShader(GLenum, const std::string&);
GLuint CreateProgram(const std::vector<GLuint>&);