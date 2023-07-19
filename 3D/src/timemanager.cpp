#include "pch.h"
#include "timemanager.h"

Cvar timescale = { "timescale", 1 };

TimeManager::TimeManager() : dDeltaTime(), deltaTime(), lastTime(), unscaledDeltaTime(), dUnscaledDeltaTime(),
	fixedDeltaTime((float)dFixedDeltaTime)
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
}
