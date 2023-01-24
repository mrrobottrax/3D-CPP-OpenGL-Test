#include <pch.h>

#include <timeManager.h>

namespace TimeManager {
	double dDeltaTime;
	float deltaTime;
	double lastTime;

	void Update()
	{
		dDeltaTime = glfwGetTime() - lastTime;
		deltaTime = (float)dDeltaTime;
		lastTime = glfwGetTime();
	}
}
