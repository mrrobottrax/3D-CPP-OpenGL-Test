#version 460

smooth in vec2 worldPos;

out vec4 outputColor;

void main()
{
	outputColor = vec4(worldPos.x, worldPos.y, 0, 1);
}