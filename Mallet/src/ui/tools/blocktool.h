#pragma once

class BlockTool
{
public:
	static inline bool creatingBlock = false;

	static inline glm::vec3 blockStart {0, 0, 0};
	static inline glm::vec3 blockEnd {0, 0, 0};

	static inline const float blockPreviewTemplate[] = {
		// Bottom
		-1, -1, -1,
		-1, -1,  1,
		-1, -1,  1,
		 1, -1,  1,
		 1, -1,  1,
		 1, -1, -1,
		 1, -1, -1,
		-1, -1, -1,

		// Top
		-1,  1, -1,
		-1,  1,  1,
		-1,  1,  1,
		 1,  1,  1,
		 1,  1,  1,
		 1,  1, -1,
		 1,  1, -1,
		-1,  1, -1,

		// Middle
		-1, -1, -1,
		-1,  1, -1,
		-1, -1,  1,
		-1,  1,  1,
		 1, -1, -1,
		 1,  1, -1,
		 1, -1,  1,
		 1,  1,  1,
	};

	static inline float blockPreview[sizeof(blockPreviewTemplate) / sizeof(float)];
};