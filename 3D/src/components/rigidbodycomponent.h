#pragma once

enum ColliderType
{
	Hull
};

struct RigidBodyComponent
{
	bool isStatic;
	ColliderType colliderType;
	float gravityScale = 1;
};