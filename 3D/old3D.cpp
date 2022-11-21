#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const float vertexPositions[] = {
     0.25f,  0.25f, 0.75f, 1.0f,
     0.25f, -0.25f, 0.75f, 1.0f,
    -0.25f,  0.25f, 0.75f, 1.0f,

     0.25f, -0.25f, 0.75f, 1.0f,
    -0.25f, -0.25f, 0.75f, 1.0f,
    -0.25f,  0.25f, 0.75f, 1.0f,

     0.25f,  0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,
     0.25f, -0.25f, -0.75f, 1.0f,

     0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,

    -0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f, -0.25f,  0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,

    -0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,

     0.25f,  0.25f,  0.75f, 1.0f,
     0.25f, -0.25f, -0.75f, 1.0f,
     0.25f, -0.25f,  0.75f, 1.0f,

     0.25f,  0.25f,  0.75f, 1.0f,
     0.25f,  0.25f, -0.75f, 1.0f,
     0.25f, -0.25f, -0.75f, 1.0f,

     0.25f,  0.25f, -0.75f, 1.0f,
     0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f,  0.25f,  0.75f, 1.0f,

     0.25f,  0.25f, -0.75f, 1.0f,
    -0.25f,  0.25f,  0.75f, 1.0f,
    -0.25f,  0.25f, -0.75f, 1.0f,

     0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f,  0.75f, 1.0f,
     0.25f, -0.25f,  0.75f, 1.0f,

     0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f, -0.75f, 1.0f,
    -0.25f, -0.25f,  0.75f, 1.0f,




    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,

    0.8f, 0.8f, 0.8f, 1.0f,
    0.8f, 0.8f, 0.8f, 1.0f,
    0.8f, 0.8f, 0.8f, 1.0f,

    0.8f, 0.8f, 0.8f, 1.0f,
    0.8f, 0.8f, 0.8f, 1.0f,
    0.8f, 0.8f, 0.8f, 1.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,

    0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f,

    0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,

    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,

    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
};

const std::string strVertexShader(
    "#version 330\n"
    "layout(location = 0) in vec4 position;\n"
    "layout(location = 1) in vec4 color;\n"
    "uniform vec2 offset;\n"
    "uniform mat4 perspMatrix;\n"
    "smooth out vec4 theColor;\n"
    "void main()\n"
    "{\n"
    "   vec4 cameraPos = position + vec4(offset.x, offset.y, -2, 0);\n"
    "   gl_Position = perspMatrix * cameraPos;\n"
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

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLuint positionBufferObject;
GLuint vao;

GLuint offsetUniform;
GLuint perspMatrixUniform;

GLuint theProgram;

float perspectiveMatrix[16];
float nearPlane = 0.3f, farPlane = 1000.0f, frustumScale = 1.0f;

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

void setPerspMatrixAspect(int width, int height)
{
    perspectiveMatrix[0] = frustumScale / (width / (float)height);
    perspectiveMatrix[5] = frustumScale;

    glUseProgram(theProgram);
    glUniformMatrix4fv(perspMatrixUniform, 1, GL_FALSE, perspectiveMatrix);
    glUseProgram(0);
}

void reshape(GLFWwindow* window, int width, int height)
{
    glfwGetFramebufferSize(window, &width, &height);

    setPerspMatrixAspect(width, height);

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

GLFWwindow* init()
{
    if (!glfwInit())
    {
        // Initialization failed
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);

    // Require at least version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 400, "3D Test", NULL, NULL);
    if (!window)
    {
        // Window or OpenGL context creation failed
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

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

    // Init shader program
    std::vector<GLuint> shaderList;

    shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
    shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

    theProgram = CreateProgram(shaderList);

    offsetUniform = glGetUniformLocation(theProgram, "offset");
    perspMatrixUniform = glGetUniformLocation(theProgram, "perspMatrix");

    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

    // Init vertex buffer
    glGenBuffers(1, &positionBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Cull backfaces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    // Set up uniforms
    memset(perspectiveMatrix, 0, sizeof(float) * 16);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    setPerspMatrixAspect(width, height);

    perspectiveMatrix[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    perspectiveMatrix[14] = (2 * farPlane * nearPlane) / (nearPlane - farPlane);
    perspectiveMatrix[11] = -1.0f;

    glUseProgram(theProgram);
    glUniformMatrix4fv(perspMatrixUniform, 1, GL_FALSE, perspectiveMatrix);
    glUseProgram(0);

    return window;
}

int main()
{
    GLFWwindow* window = init();

    glfwSetWindowSizeCallback(window, reshape);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Clear the colorbuffer 
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(theProgram);
        glUniform2f(offsetUniform, sinf(glfwGetTime()) * 0.7f, cosf(glfwGetTime()) * 0.7f);

        size_t colorData = sizeof(vertexPositions) / 2;
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorData);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glUseProgram(0);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
