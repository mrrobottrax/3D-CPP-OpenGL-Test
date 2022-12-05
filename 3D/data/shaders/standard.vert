#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
smooth out vec4 color;
uniform mat4 matrix;
void main()
{
	gl_Position = matrix * position;
	color = vec4(normal.x, normal.y, normal.z, 1.0);
}