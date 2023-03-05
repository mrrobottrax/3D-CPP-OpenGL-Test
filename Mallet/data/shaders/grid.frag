#version 460

uniform vec2 onePixelDistance;
uniform float baseGridSize;

smooth in vec2 worldPos;

out vec4 outputColor;

float grid (vec2 st, float x)
{
	vec2 grid = fract(st * x);
	float b = grid.x * grid.y;

	st += onePixelDistance;
	grid = fract(st * x);
	float b2 = grid.x * grid.y;

	return step(0, b - b2);
}

void main()
{
	float grids = grid(worldPos, 1 / baseGridSize);
	float gridm = grid(worldPos, 0.5 / baseGridSize);
	float gridl = grid(worldPos, 0.25 / baseGridSize);

	float grida = grids + gridm + gridl;
	grida *= 0.25;

	outputColor = vec4(grida, grida, grida, 1);
}