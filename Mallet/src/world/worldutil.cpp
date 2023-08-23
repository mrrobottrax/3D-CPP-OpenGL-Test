#include "malletpch.h"
#include "worldutil.h"

#include "worldinfo.h"

#include <memory/entitymanager.h>
#include <meshwork/malletmeshcomponent.h>

void WorldUtil::UnloadWorld()
{
	// Delete meshes
	MalletMesh::DeleteAllMeshes();

	// Delete all mesh entities
	std::vector<ChunkArchetypeElement*> archetypes = entityManager.FindChunkArchetypesWithComponent(Component().Init<MalletMeshComponent>());
	if (!archetypes.empty())
	{
		for (auto archetypeIt = archetypes.begin(); archetypeIt != archetypes.end(); ++archetypeIt)
		{
			entityManager.DeleteChunkArchetype(*archetypeIt);
		}
	}

	worldInfo.comment = "";
}