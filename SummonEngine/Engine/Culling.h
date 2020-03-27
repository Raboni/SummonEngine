#pragma once
#include "RenderSettings.h"
#include <GrowingArray.hpp>
#include <atomic>

class ModelInstance;
namespace CU
{
	class Camera;
}

struct CullingCollection
{
	CU::GrowingArray<ModelInstance*, int> myModelsInstanced;
	CU::GrowingArray<ModelInstance*, int> myModelsDeferred;
	CU::GrowingArray<ModelInstance*, int> myModelsForward;
	CU::GrowingArray<ModelInstance*, int> myModelsWireframe;
	CU::GrowingArray<ModelInstance*, int> myModelsMap;
	CU::GrowingArray<int> myModelInstancedStartIndexList;
	CU::GrowingArray<int> myModelMapStartIndexList;
	std::atomic<bool> myIsCulled = false;
};

void InsertionSortForwardModels(CU::GrowingArray<ModelInstance*, int>& aUnsortedArray, const CU::Vector3f aCameraPosition);

void CullModelList(const CU::GrowingArray<ModelInstance*, int>& aCullList, CullingCollection* outCulledCollection,
	RenderSettings& aSettings, const CU::Camera* aCamera, const CU::Camera* aMapCamera, const float aMapRange);