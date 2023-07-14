#version 460

uniform vec3 fillColor;

out vec4 outColor;

void main()
{
	outColor = vec4(fillColor, 1);
}