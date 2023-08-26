#pragma once

inline GLFWwindow* pMainWindow;
constexpr char* glsl_version = "#version 460";

inline GLuint vao;

// Shared uniforms
inline GLuint sharedPositionMatrixUnif;
inline GLuint sharedPerspectiveMatrixUnif;

// Standard shader
inline GLuint standardShaderProgram;
inline GLuint normalMatrix;
inline GLuint sunDirUnif;
inline GLuint sunIntensityUnif;
inline GLuint ambientIntensityUnif;
inline GLuint colorUnif;

inline GLuint wireframeShaderProgram;

inline GLuint solidShaderProgram;
inline GLuint solidColorUnif;

void ErrorCallback(int, const char*);
void KeyCallback(GLFWwindow*, int, int, int, int);
void DefaultWindowSizeCallback(GLFWwindow*, int, int);
void InitializeWindow();
void InitializeOpenGL();

GLuint CreateShader(GLenum, const std::string&);
GLuint CreateProgram(const std::vector<GLuint>&);

const inline float quadArray[] = {
	-1, -1, 1,
	 1,  1, 1,
	-1,  1, 1,

	 1,  1, 1,
	-1, -1, 1,
	 1, -1, 1,
};