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

	float GetFixedDeltaTime();
	double GetFixedDeltaTimeDouble();

	float GetDeltaTime();
	double GetDeltaTimeDouble();

	float GetUnscaledDeltaTime();
	double GetUnscaledDeltaTimeDouble();

}; inline TimeManager timeManager;