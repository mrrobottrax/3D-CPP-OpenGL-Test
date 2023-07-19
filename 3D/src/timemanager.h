#pragma once
#include <input/console.h>

extern Cvar timescale;

class TimeManager {
public:
	TimeManager();

private:
	const double dFixedDeltaTime = 1 / 60.0f;
	float fixedDeltaTime;

	float deltaTime;
	double dDeltaTime;

	float unscaledDeltaTime;
	double dUnscaledDeltaTime;

	double lastTime;

public:
	void Update();

	float GetFixedDeltaTime()
	{
		return fixedDeltaTime;
	}

	double GetFixedDeltaTimeDouble()
	{
		return dFixedDeltaTime;
	}

	float GetDeltaTime()
	{
		return deltaTime;
	}

	float GetUnscaledDeltaTime()
	{
		return unscaledDeltaTime;
	}

	double GetDeltaTimeDouble()
	{
		return dDeltaTime;
	}

	double GetUnscaledDeltaTimeDouble()
	{
		return dUnscaledDeltaTime;
	}

}; inline TimeManager timeManager;