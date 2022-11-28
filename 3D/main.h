#pragma once

#include <gl.h>

int main();

extern GLuint shaderProgram;
extern GLuint positionBufferObject;
extern GLuint elementBufferObject;
extern GLuint vao;
extern GLuint offsetUniform;
extern GLuint colorUniform;
extern GLuint matrixUniform;

const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"uniform vec4 color;\n"
	"smooth out vec4 theColor;\n"
	"uniform vec3 offset;\n"
	"uniform mat4 perspectiveMatrix;\n"
	"void main()\n"
	"{\n"
	"	vec4 cameraPos = position + vec4(offset.x, offset.y, offset.z, 0.0);\n"
	"	gl_Position = perspectiveMatrix * cameraPos;\n"
	"	theColor = color;\n"
	"}"
);

const std::string strFragmentShader(
	"#version 330\n"
	"smooth in vec4 theColor;\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"	outputColor = theColor;\n"
	"}"
);