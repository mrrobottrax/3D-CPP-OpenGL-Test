#pragma once

class TimeManager {
	float deltaTime;
	double dDeltaTime;
	double lastTime;

public:
	void Update();

	float GetDeltaTime();
	double GetDeltaTimeDouble();
}; inline TimeManager timeManager;