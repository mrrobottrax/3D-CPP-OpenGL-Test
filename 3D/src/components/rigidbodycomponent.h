#pragma once

enum ColliderType
{
	Hull
};

enum FrictionCombine
{
	Average,
	Min,
	Max
};

struct RigidBodyComponent
{
	bool isStatic;
	ColliderType colliderType;
	float gravityScale = 1;
	float frictionCoefficient = 0.6f;
	FrictionCombine frictionCombine = Average;

	float sleepTimer = 0.2f;

	inline const bool IsSleeping() const
	{
		return sleepTimer <= 0;
	};
};