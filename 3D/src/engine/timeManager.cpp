#include <pch.h>

#include <engine/timeManager.h>

namespace TimeManager {
	double dDeltaTime;
	float deltaTime;
	double lastTime;

	void update()
	{
		dDeltaTime = glfwGetTime() - lastTime;
		deltaTime = (float)dDeltaTime;
		lastTime = glfwGetTime();
	}
}
