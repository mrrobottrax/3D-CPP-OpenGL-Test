#pragma once

extern GLFWwindow* pMainWindow;
extern const char* glsl_version;

extern GLuint vao;

extern GLuint sharedPositionMatrixUnif;
extern GLuint sharedPerspectiveMatrixUnif;

extern GLuint standardShaderProgram;
extern GLuint normalMatrix;
extern GLuint sunDirUnif;
extern GLuint sunIntensityUnif;
extern GLuint ambientIntensityUnif;
extern GLuint colorUnif;

extern GLuint wireframeShaderProgram;

void ErrorCallback(int, const char*);
void KeyCallback(GLFWwindow*, int, int, int, int);
void DefaultWindowSizeCallback(GLFWwindow*, int, int);
void InitializeWindow();
void InitializeOpenGL();

GLuint CreateShader(GLenum, const std::string&);
GLuint CreateProgram(const std::vector<GLuint>&);