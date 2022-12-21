#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

smooth out vec3 outNormal;

uniform vec3 sunDir;
uniform float sunIntensity;

uniform float ambientIntensity;

uniform mat4 positionMatrix;
uniform mat3 normalMatrix;
uniform vec4 diffuseColor;

uniform mat4 perspectiveMatrix;

void main()
{
	gl_Position = perspectiveMatrix * (positionMatrix * vec4(position, 1.0));

	outNormal = normalMatrix * normal;
}