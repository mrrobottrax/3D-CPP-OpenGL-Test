#pragma once
#include <memory/entityarchetype.h>
#include <components/idcomponent.h>
#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/scalecomponent.h>
#include <components/meshcomponent.h>

Component malletMeshComponents[] = {
	Component().Init<IdComponent>(),
	Component().Init<PositionComponent>(),
	Component().Init<RotationComponent>(),
	Component().Init<ScaleComponent>(),
	Component().Init<MeshComponent>(),
};
inline EntityArchetype malletMesh = EntityArchetype(5, malletMeshComponents);