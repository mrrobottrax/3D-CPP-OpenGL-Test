#pragma once

extern GLFWwindow* mainWindow;
extern const char* glsl_version;

extern GLuint shaderProgram;
extern GLuint vao;
extern GLuint perspectiveMatrix;
extern GLuint positionMatrix;
extern GLuint normalMatrix;
extern GLuint sunDirUnif;
extern GLuint sunIntensityUnif;
extern GLuint ambientIntensityUnif;
extern GLuint colorUnif;

void ErrorCallback(int, const char*);
void KeyCallback(GLFWwindow*, int, int, int, int);
void WindowSizeCallback(GLFWwindow*, int, int);
void InitializeWindow();
void InitializeOpenGL();

GLuint CreateShader(GLenum, const std::string&);
GLuint CreateProgram(const std::vector<GLuint>&);