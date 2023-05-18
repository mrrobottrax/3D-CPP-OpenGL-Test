#include <pch.h>

#include <timeManager.h>

Cvar timescale = { "timescale", 1 };

TimeManager::TimeManager() : dDeltaTime(), deltaTime(), lastTime(), unscaledDeltaTime(), dUnscaledDeltaTime(),
	fixedDeltaTime((float)dFixedDeltaTime), scaledFixedDeltaTime(), dScaledFixedDeltaTime()
{
	console.AddCvar(timescale);
}

void TimeManager::Update()
{
	dDeltaTime = (glfwGetTime() - lastTime);
	deltaTime = (float)dDeltaTime;
	dUnscaledDeltaTime = dDeltaTime;
	unscaledDeltaTime = deltaTime;
	lastTime = glfwGetTime();

	deltaTime *= timescale.value;
	dDeltaTime *= timescale.value;

	dScaledFixedDeltaTime = dFixedDeltaTime * timescale.value;
	scaledFixedDeltaTime = (float)dScaledFixedDeltaTime;
}

float TimeManager::GetFixedDeltaTime()
{
	return scaledFixedDeltaTime;
}

float TimeManager::GetUnscaledFixedDeltaTime()
{
	return fixedDeltaTime;
}

double TimeManager::GetFixedDeltaTimeDouble()
{
	return dScaledFixedDeltaTime;
}

double TimeManager::GetUnscaledFixedDeltaTimeDouble()
{
	return dFixedDeltaTime;
}

float TimeManager::GetDeltaTime()
{
	return deltaTime;
}

float TimeManager::GetUnscaledDeltaTime()
{
	return unscaledDeltaTime;
}

double TimeManager::GetDeltaTimeDouble()
{
	return dDeltaTime;
}

double TimeManager::GetUnscaledDeltaTimeDouble()
{
	return dUnscaledDeltaTime;
}
