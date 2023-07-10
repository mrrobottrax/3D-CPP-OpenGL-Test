#version 460

uniform float baseGridSize;
uniform float unitScreenSize;

smooth in vec2 xy;

out vec4 outputColor;

float grid(vec2 pos, float size)
{
	vec2 grid = abs(fract(pos / size) * 2 - 1);
	float t = max(grid.x, grid.y);
	float gridLineThickness = 4 / unitScreenSize / size;

	// Don't render grids smaller than 10px
	float mask = step(10, size * unitScreenSize);
	return smoothstep(1 - gridLineThickness, 1, t) * mask;
}

void main()
{
	float grids = grid(xy, baseGridSize);
	float gridm = grid(xy, baseGridSize * 2);
	float gridl = grid(xy, baseGridSize * 4);
	float gridxl = grid(xy, baseGridSize * 8);

	float grida = grids + gridm + gridl + gridxl;
	grida *= 0.15;

	outputColor = vec4(grida, grida, grida, 1);
}