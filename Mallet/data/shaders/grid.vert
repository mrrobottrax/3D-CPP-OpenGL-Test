#version 460

uniform mat4 screenToWorldMatrix;

layout(location = 0) in vec3 position;

smooth out vec2 worldPos;

void main()
{
	gl_Position = vec4(position, 1.0);

	vec4 world = screenToWorldMatrix * vec4(position, 1.0);
	worldPos = vec2(world.x, world.y);
}