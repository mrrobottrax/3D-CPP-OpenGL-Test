#pragma once
#include <memory/entityarchetype.h>
#include <components/idcomponent.h>
#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/scalecomponent.h>
#include <meshwork/malletmeshcomponent.h>
#include <components/aabb.h>
#include <components/meshcomponent.h>

inline Component malletMeshComponents[] = {
	Component().Init<IdComponent>(),
	Component().Init<PositionComponent>(),
	Component().Init<RotationComponent>(),
	Component().Init<ScaleComponent>(),
	Component().Init<MalletMeshComponent>(),
	Component().Init<MeshComponent>(),
	Component().Init<AABB>(),
};
inline EntityArchetype malletMesh = EntityArchetype(7, malletMeshComponents);