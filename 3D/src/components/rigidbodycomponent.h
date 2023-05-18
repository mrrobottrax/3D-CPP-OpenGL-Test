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
	float frictionCoefficient = 0.5f;
	FrictionCombine frictionCombine = Average;
};