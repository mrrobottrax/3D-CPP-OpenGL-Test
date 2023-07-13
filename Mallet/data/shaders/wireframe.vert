#version 460
layout(location = 0) uniform mat4 positionMatrix;
layout(location = 1) uniform mat4 perspectiveMatrix;

layout(location = 0) in vec3 position;

smooth out vec3 color;

void main()
{
	gl_Position = perspectiveMatrix * (positionMatrix * vec4(position, 1.0));
	color = vec3(0, 0.4, 0.4);
}