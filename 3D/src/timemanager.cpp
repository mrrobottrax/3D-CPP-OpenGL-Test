#include <pch.h>

#include <timeManager.h>

void TimeManager::Update()
{
	dDeltaTime = glfwGetTime() - lastTime;
	deltaTime = (float)dDeltaTime;
	lastTime = glfwGetTime();
}
float TimeManager::GetDeltaTime()
{
	return deltaTime;
}
double TimeManager::GetDeltaTimeDouble()
{
	return dDeltaTime;
}
