#pragma once

#include <input/console.h>

extern Cvar timescale;

class TimeManager {
public:
	TimeManager();

private:
	const double dFixedDeltaTime = 1 / 60.0f;
	double dScaledFixedDeltaTime;
	float fixedDeltaTime;
	float scaledFixedDeltaTime;

	float deltaTime;
	double dDeltaTime;

	float unscaledDeltaTime;
	double dUnscaledDeltaTime;

	double lastTime;

public:
	void Update();

	float GetFixedDeltaTime();
	double GetFixedDeltaTimeDouble();

	float GetUnscaledFixedDeltaTime();
	double GetUnscaledFixedDeltaTimeDouble();

	float GetDeltaTime();
	double GetDeltaTimeDouble();

	float GetUnscaledDeltaTime();
	double GetUnscaledDeltaTimeDouble();

}; inline TimeManager timeManager;