#version 330
smooth in vec3 outNormal;
out vec4 outputColor;
void main()
{
	vec3 newNormal = normalize(outNormal);

	outputColor = vec4(abs(newNormal.x), abs(newNormal.y), abs(newNormal.z), 1);
}