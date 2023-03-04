#version 460

layout (location = 0) in vec3 position;

smooth out vec4 color;

void main()
{
	gl_Position = vec4(position, 1.0);
	color = vec4(1, 1, 1, 1);
}