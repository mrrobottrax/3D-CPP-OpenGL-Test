#version 330
smooth in vec3 outNormal;
out vec4 outputColor;
void main()
{
	vec3 newNormal = normalize(outNormal);

	outputColor = vec4(newNormal.x, newNormal.y, newNormal.z, 1);
}