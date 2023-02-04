#version 460
layout(location = 0) uniform mat4 positionMatrix;
layout(location = 1) uniform mat4 perspectiveMatrix;

in vec3 position;
in vec3 normal;

smooth out vec3 outNormal;

uniform vec3 sunDir;
uniform float sunIntensity;

uniform float ambientIntensity;

uniform mat3 normalMatrix;
uniform vec4 diffuseColor;

void main()
{
	gl_Position = perspectiveMatrix * (positionMatrix * vec4(position, 1.0));

	outNormal = normalMatrix * normal;
}