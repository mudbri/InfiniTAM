// Copyright 2014-2017 Oxford University Innovation Limited and the authors of InfiniTAM

#include "ITMMeshingEngine_CPU.h"
#include "../Shared/ITMMeshingEngine_Shared.h"
#include <unordered_set>

using namespace ITMLib;

template<class TVoxel>
void ITMMeshingEngine_CPU<TVoxel, ITMVoxelBlockHash>::MeshScene(ITMMesh *mesh, const ITMScene<TVoxel, ITMVoxelBlockHash> *scene, std::vector< Vector3i >& voxelsIter)
{
	ITMMesh::Triangle *triangles = mesh->triangles->GetData(MEMORYDEVICE_CPU);
	const TVoxel *localVBA = scene->localVBA.GetVoxelBlocks();
	const ITMHashEntry *hashTable = scene->index.GetEntries();

	int noTriangles = 0, noMaxTriangles = mesh->noMaxTriangles, noTotalEntries = scene->index.noTotalEntries;
	float factor = scene->sceneParams->voxelSize;

	// Not clearing mesh triangles. This means that the new angle that does not have data for a particular voxel will have old triangles. This might be incorrect
	// mesh->triangles->Clear();

	int numMisses = 0;
	int numHits = 0;

	int numPotentialVoxels = voxelsIter.size();
	for (int count = 0; count < numPotentialVoxels; count++) {
		Vector3f vertList[12];
		int cubeIndex = buildVertList(vertList, voxelsIter[count], Vector3i(0, 0, 0), localVBA, hashTable);
		
	if (cubeIndex < 0) {
		numMisses++;
		continue;
	} 

		numHits++;
		for (int i = 0; triangleTable[cubeIndex][i] != -1; i += 3)
		{
			triangles[noTriangles].p0 = vertList[triangleTable[cubeIndex][i]] * factor;
			triangles[noTriangles].p1 = vertList[triangleTable[cubeIndex][i + 1]] * factor;
			triangles[noTriangles].p2 = vertList[triangleTable[cubeIndex][i + 2]] * factor;

			if (noTriangles < noMaxTriangles - 1) noTriangles++;
		}
	}

	// for (int entryId = 0; entryId < noTotalEntries; entryId++)
	// {
	// 	Vector3i globalPos;
	// 	const ITMHashEntry &currentHashEntry = hashTable[entryId];

	// 	if (currentHashEntry.ptr < 0) {
	// 		numMisses++;
	// 		continue;
	// 	}
	// 	globalPos = currentHashEntry.pos.toInt() * SDF_BLOCK_SIZE;

	// 	for (int z = 0; z < SDF_BLOCK_SIZE; z++) for (int y = 0; y < SDF_BLOCK_SIZE; y++) for (int x = 0; x < SDF_BLOCK_SIZE; x++)
	// 	{
	// 		Vector3f vertList[12];
	// 		int cubeIndex = buildVertList(vertList, globalPos, Vector3i(x, y, z), localVBA, hashTable);
			
	// 		if (cubeIndex < 0) {
	// 			numMisses++;
	// 			continue;
	// 		} 
	// 		numHits++;
	// 		for (int i = 0; triangleTable[cubeIndex][i] != -1; i += 3)
	// 		{
	// 			triangles[noTriangles].p0 = vertList[triangleTable[cubeIndex][i]] * factor;
	// 			triangles[noTriangles].p1 = vertList[triangleTable[cubeIndex][i + 1]] * factor;
	// 			triangles[noTriangles].p2 = vertList[triangleTable[cubeIndex][i + 2]] * factor;

	// 			if (noTriangles < noMaxTriangles - 1) noTriangles++;
	// 		}
	// 	}
	// }
	printf("Num misses:%d\n", numMisses);
	printf("Num hits:%d\n", numHits);
	printf("Num possible voxels:%d\n", voxelsIter.size());
	printf("noTotalEntries:%d\n", noTotalEntries);
	mesh->noTotalTriangles = noTriangles;
}