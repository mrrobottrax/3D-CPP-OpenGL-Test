#pragma once

namespace shaderLoader {
	const inline char defaultVertexShader[] = "#version 460\n\nlayout(location = 0) in vec3 position;\n\nvoid main()\n{\n\tgl_Position = vec4(position, 1.0);\n}";
	const inline char defaultFragmentShader[] = "#version 460\n\nout vec4 outColor;\n\nvoid main()\n{\n\toutColor = vec4(1, 0, 1, 1);\n}";

	const char* LoadShader(const char*);
}