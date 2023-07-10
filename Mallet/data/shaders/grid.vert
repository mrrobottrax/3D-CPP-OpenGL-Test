#version 460

uniform mat4 screenToWorldMatrix;

layout(location = 0) in vec3 position;

smooth out vec2 xy;

void main()
{
	gl_Position = vec4(position, 1.0);
	vec4 pos = screenToWorldMatrix * gl_Position;
	xy = vec2(pos.x, pos.y);
}