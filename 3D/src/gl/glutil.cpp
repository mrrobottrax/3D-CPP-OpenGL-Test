#include <pch.h>
#include <gl/glutil.h>

#include <gl/shaderLoader.h>
#include <managers.h>
#include <systems/renderSystem.h>

GLFWwindow* mainWindow;

GLuint vao;

GLuint sharedPositionMatrixUnif;
GLuint sharedPerspectiveMatrixUnif;

GLuint standardShaderProgram;
GLuint normalMatrix;
GLuint sunDirUnif;
GLuint sunIntensityUnif;
GLuint ambientIntensityUnif;
GLuint colorUnif;

GLuint wireframeShaderProgram;

void ErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	inputManager->KeyCallback(key, scancode, action, mods);
}

void DefaultWindowSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

const char* glsl_version = "#version 460";

// Create a window and initialize GLEW
void InitializeWindow()
{
	if (!glfwInit())
	{
		// Initialization failed
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback(ErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	mainWindow = glfwCreateWindow(1200, 600, "3D Test", NULL, NULL);
	if (!mainWindow)
	{
		// Window or OpenGL context creation failed
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(mainWindow, KeyCallback);
	glfwSetWindowSizeCallback(mainWindow, DefaultWindowSizeCallback);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(mainWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwMakeContextCurrent(mainWindow);
	glfwSwapInterval(0);    // Vsync

	#ifdef USE_GLEW
		// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions 
		glewExperimental = GL_TRUE;

		// Initialize GLEW to setup the OpenGL Function pointers 
		if (GLEW_OK != glewInit())
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			exit(EXIT_FAILURE);
		}
	#endif // USE_GLEW

	#ifdef USE_GLAD
			// Initialize GLAD to setup the OpenGL Function pointers 
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				std::cout << "Failed to initialize GLAD" << std::endl;
				exit(EXIT_FAILURE);
			}
	#endif // USE_GLAD

	int width, height;
	glfwGetFramebufferSize(mainWindow, &width, &height);
	glViewport(0, 0, width, height);
}

void InitializeOpenGL()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	std::vector<GLuint> shaderList;

	// Standard shader
	{
		const char* strVertShader = shaderLoader::loadShader("data/shaders/normal.vert");
		const char* strFragShader = shaderLoader::loadShader("data/shaders/normal.frag");
		shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertShader));
		shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragShader));
		delete[] strVertShader;
		delete[] strFragShader;

		standardShaderProgram = CreateProgram(shaderList);
		std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

		sharedPerspectiveMatrixUnif = glGetUniformLocation(standardShaderProgram, "perspectiveMatrix");
		sharedPositionMatrixUnif = glGetUniformLocation(standardShaderProgram, "positionMatrix");
		normalMatrix = glGetUniformLocation(standardShaderProgram, "normalMatrix");
		sunDirUnif = glGetUniformLocation(standardShaderProgram, "sunDir");
		sunIntensityUnif = glGetUniformLocation(standardShaderProgram, "sunIntensity");
		ambientIntensityUnif = glGetUniformLocation(standardShaderProgram, "ambientIntensity");
		colorUnif = glGetUniformLocation(standardShaderProgram, "diffuseColor");
	}

	// Wireframe shader
	{
		const char* strVertShader = shaderLoader::loadShader("data/shaders/wireframe.vert");
		const char* strFragShader = shaderLoader::loadShader("data/shaders/wireframe.frag");
		shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertShader));
		shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragShader));
		delete[] strVertShader;
		delete[] strFragShader;

		wireframeShaderProgram = CreateProgram(shaderList);
		std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

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