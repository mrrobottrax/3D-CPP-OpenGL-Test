#version 460

uniform vec2 onePixelDistance;
uniform float baseGridSize;

smooth in vec2 worldPos;

out vec4 outputColor;

float grid(vec2 pos, float size)
{
	// TODO: Is the early exit here worth it?
	// Or should we use a mask
	if (size < onePixelDistance.x * 10)
		return 0;

	vec2 grid = fract(pos / size);

	// Render grid shifted over 1 pixel
	pos -= onePixelDistance;
	vec2 grid2 = fract(pos / size);

	// Only show grid when cell size is greater than 10 pixels
	//float pass = step(onePixelDistance.x * 10, size);
	//return step(0, grid2.x - grid.x + grid2.y - grid.y) * pass;

	return step(0, grid2.x - grid.x + grid2.y - grid.y);
}

void main()
{
	float grids = grid(worldPos, baseGridSize);
	float gridm = grid(worldPos, baseGridSize * 2);
	float gridl = grid(worldPos, baseGridSize * 4);
	float gridxl = grid(worldPos, baseGridSize * 8);

	float grida = grids + gridm + gridl + gridxl;
	grida *= 0.15;

	outputColor = vec4(grida, grida, grida, 1);
}