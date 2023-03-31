#pragma once

enum ColliderType
{
	Hull
};

struct RigidBodyComponent
{
	ColliderType colliderType;
};