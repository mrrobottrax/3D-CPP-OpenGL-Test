#version 460
layout(location = 0) uniform mat4 positionMatrix;
layout(location = 1) uniform mat4 perspectiveMatrix;

in vec3 position;
in vec3 vertColor;

smooth out vec4 color;

void main()
{
	gl_Position = perspectiveMatrix * (positionMatrix * vec4(position, 1.0));
	color = vec4(vertColor, 1);
}