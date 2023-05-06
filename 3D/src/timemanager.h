#pragma once

#include <input/console.h>

extern Cvar timescale;

class TimeManager {
public:
	TimeManager();

private:
	float deltaTime;
	double dDeltaTime;

	float unscaledDeltaTime;
	double dUnscaledDeltaTime;

	double lastTime;

public:
	void Update();

	float GetDeltaTime();
	double GetDeltaTimeDouble();

	float GetUnscaledDeltaTime();
	double GetUnscaledDeltaTimeDouble();

}; inline TimeManager timeManager;