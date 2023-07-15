#pragma once

#include "mallettool.h"
#include <ui/windows/viewport.h>

class BlockTool : public MalletTool
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

	static inline GLuint boxVao;
	static inline GLuint boxPositionBuffer;

	void Draw(Viewport*) override;
	void MouseCallback(Viewport*, GLFWwindow* pWindow, int button, int action, int mods) override;
	void MousePosCallback(Viewport*, GLFWwindow* pWindow, double xPos, double yPos) override;
};