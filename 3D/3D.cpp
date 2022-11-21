#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const float verts[] = {
	-1, -1, -1, 1,
	-1, -1,  1, 1,
	-1,  1, -1, 1,
	-1,  1,  1, 1,
	 1, -1, -1, 1,
	 1, -1,  1, 1,
	 1,  1, -1, 1,
	 1,  1,  1, 1,
};

const GLshort indices[] = {
	4, 2, 0,
	4, 6, 2,
	4, 5, 7,
	4, 7, 6,
	0, 2, 3,
	1, 0, 3,
	1, 3, 5,
	5, 3, 7,
	0, 1, 5,
	0, 5, 4,
	3, 2, 7,
	7, 2, 6,
};

const std::string strVertexShader(
    "#version 330\n"
    "layout(location = 0) in vec4 position;\n"
    "uniform vec4 color;\n"
    "uniform vec3 offset;\n"
    "uniform mat4 perspectiveMatrix;\n"
    "smooth out vec4 theColor;\n"
    "void main()\n"
    "{\n"
    "   vec4 cameraPos = position + vec4(offset.x, offset.y, offset.z, 0);\n"
    "   gl_Position = perspectiveMatrix * cameraPos;\n"
    "   theColor = color;\n"
    "}\n"
);

const std::string strFragmentShader(
    "#version 330\n"
    "smooth in vec4 theColor;\n"
    "out vec4 outputColor;\n"
    "void main()\n"
    "{\n"
    "   outputColor = theColor;\n"
    "}\n"
);

GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char* strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char* strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint>& shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

GLuint shaderProgram;

GLuint colorUnif;
GLuint offsetUnif;
GLuint perspectiveMatrixUnif;

float frustumScale = 1.0f;
float nearPlane = 0.3f, farPlane = 1000.0f;
float perspectiveMatrix[16];

void initializeShaderProgram()
{
    std::vector<GLuint> shaderList;

    shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
    shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

    shaderProgram = CreateProgram(shaderList);

	colorUnif = glGetUniformLocation(shaderProgram, "color");
	offsetUnif = glGetUniformLocation(shaderProgram, "offset");
	perspectiveMatrixUnif = glGetUniformLocation(shaderProgram, "perspectiveMatrix");

    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

GLuint positionBufferObject;
GLuint indexBufferObject;

void InitializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);
	glGenBuffers(1, &indexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void setPerspMatrixAspect(int width, int height)
{
	perspectiveMatrix[0] = frustumScale / (width / (float)height);
	perspectiveMatrix[5] = frustumScale;

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix);
	glUseProgram(0);
}

void window_size_callback(GLFWwindow* window, int height, int width)
{
	setPerspMatrixAspect(width, height);
}

GLFWwindow* initializeWindow()
{
	if (!glfwInit())
	{
		// Initialization failed
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(600, 600, "3D Test", NULL, NULL);
	if (!window)
	{
		// Window or OpenGL context creation failed
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);    // Vsync

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions 
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers 
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	return window;
}

GLuint vao;

GLFWwindow* init()
{
	GLFWwindow* window = initializeWindow();

    initializeShaderProgram();
	InitializeVertexBuffer();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Set up perpsective matrix
	memset(perspectiveMatrix, 0, sizeof(float) * 16);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	setPerspMatrixAspect(width, height);

	perspectiveMatrix[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
	perspectiveMatrix[14] = (2 * farPlane * nearPlane) / (nearPlane - farPlane);
	perspectiveMatrix[11] = -1.0f;

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix);
	glUseProgram(0);

	return window;
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Cull backfaces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glUseProgram(shaderProgram);

	glUniform4f(colorUnif, 1, 1, 1, 1);
	glUniform3f(offsetUnif, 0, 0, -6);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}

int main()
{
    GLFWwindow* window = init();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		display();

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}