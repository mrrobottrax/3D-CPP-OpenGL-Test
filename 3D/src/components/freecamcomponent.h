#pragma once

struct FreecamComponent
{
	bool enabled;
	bool panOnly;
	bool panning;

	float speed;
	float acceleration;
	float friction;

	FreecamComponent(float speed, float acceleration, float friction, bool enabled = true, bool panOnly = false) :
		speed(speed), acceleration(acceleration), friction(friction), enabled(enabled), panOnly(panOnly), panning(false)
	{
	}
};