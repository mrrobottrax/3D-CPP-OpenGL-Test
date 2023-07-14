#version 460
layout(location = 0) uniform mat4 positionMatrix;
layout(location = 1) uniform mat4 perspectiveMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

smooth out vec4 color;

uniform vec3 sunDir;
uniform float sunIntensity;

uniform float ambientIntensity;

uniform mat3 normalMatrix;
uniform vec4 diffuseColor;

void main()
{
	gl_Position = perspectiveMatrix * (positionMatrix * vec4(position, 1.0));
	vec3 camSpaceNormal = normalize(normalMatrix * normal);

	float cosAngIncidence = dot(camSpaceNormal, sunDir);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	color = (sunIntensity * cosAngIncidence * diffuseColor) + (diffuseColor * ambientIntensity);
}