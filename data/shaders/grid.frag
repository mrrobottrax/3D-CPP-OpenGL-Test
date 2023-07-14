#version 460

uniform vec3 fillColor;
uniform float gridSize;
uniform vec2 offset;

out vec4 outColor;

float grid(vec2 fragCoord, float size)
{
	vec2 p = fragCoord + offset;
	
	vec2 a1 = mod(p - 0.5, size);
	vec2 a2 = mod(p + 0.5, size);
	vec2 a = a2 - a1;
	
	float g = min(a.x, a.y);
	return step(g, 0);
}

void main()
{
	vec2 fragCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);

	outColor = vec4(fillColor * grid(fragCoord, gridSize), 1);
}