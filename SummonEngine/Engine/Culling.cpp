#include "stdafx.h"
#include "Culling.h"
#include "Model.h"
#include "ModelInstance.h"
#include <Intersection.hpp>
#include <Camera.hpp>
#include <Vector3.hpp>
#include <DebugTools.h>

void InsertionSortForwardModels(CU::GrowingArray<ModelInstance*, int>& aUnsortedArray, const CU::Vector3f aCameraPosition)
{
	int pos = 0;
	float distance;
	ModelInstance* temp;

	for (int i = 1; i < aUnsortedArray.Size(); i++)
	{
		pos = i - 1;
		temp = aUnsortedArray[i];
		distance = (temp->GetPosition() - aCameraPosition).LengthSqr();

		while (pos >= 0 && distance > (aUnsortedArray[pos]->GetPosition() - aCameraPosition).LengthSqr())//closest model at end
		{
			aUnsortedArray[pos + 1] = aUnsortedArray[pos];
			pos--;
		}

		aUnsortedArray[pos + 1] = temp;
	}
}

void CullModelList(const CU::GrowingArray<ModelInstance*, int>& aCullList, CullingCollection* outCulledCollection,
	RenderSettings& aSettings, const CU::Camera* aCamera, const CU::Camera* aMapCamera, const float aMapRange)
{
	if (aCamera == nullptr || outCulledCollection == nullptr)
	{
		CU::Log("Invalid parameters for CullModels", CU::LogType::Error);
		return;
	}

	const int cullListSize = aCullList.Size();
	outCulledCollection->myModelsInstanced.Init(cullListSize);
	outCulledCollection->myModelsDeferred.Init(cullListSize);
	outCulledCollection->myModelsForward.Init(cullListSize);
	outCulledCollection->myModelsWireframe.Init(cullListSize);
	outCulledCollection->myModelsMap.Init(cullListSize);

	const CU::Vector3f cameraPosition = aCamera->GetPosition();
	CU::Vector3f mapFocus;
	if (aMapCamera != nullptr)
	{
		mapFocus = aMapCamera->GetPosition();
	}

	CU::Vector3f toModel;
	CU::Vector3f toModelMap;
	float toModelLengthSqr = 0.0f;
	float toModelMapLengthSqr = 0.0f;
	const float drawDistanceSqr = aSettings.DrawDistance * aSettings.DrawDistance;
	bool addToView = true;
	for (int modelIndex = 0; modelIndex < cullListSize; modelIndex++)
	{
		const bool skipMap = aMapCamera == nullptr || !aCullList[modelIndex]->ShouldRenderOnMap();
		toModel = aCullList[modelIndex]->GetPosition() - cameraPosition;
		toModelLengthSqr = toModel.LengthSqr();
		if (!skipMap)
		{
			toModelMap = aCullList[modelIndex]->GetPosition() - mapFocus;
			toModelMap.y = 0.0f;
			toModelMapLengthSqr = toModelMap.LengthSqr();
		}
		if (toModelLengthSqr > drawDistanceSqr * 1.5f && (skipMap || toModelMapLengthSqr > aMapRange * 1.5f))
		{
			aCullList[modelIndex]->Release();
		}
		else if (toModelLengthSqr < drawDistanceSqr * 1.25f || (!skipMap && toModelMapLengthSqr < aMapRange * 1.25f))
		{
			if (!aCullList[modelIndex]->HasCalledLoad())
			{
				aCullList[modelIndex]->Load();
			}
		}

		if (!aCullList[modelIndex]->HasModel() || (toModelLengthSqr > drawDistanceSqr && (skipMap || toModelMapLengthSqr > aMapRange)))
		{
			continue;
		}
		else
		{
			const CU::Intersection::Sphere modelCullSphere(aCullList[modelIndex]->GetPosition(), aCullList[modelIndex]->GetEncapsulationRange() * aCullList[modelIndex]->GetScale().Length());
			const bool insideViewFrustum = aCamera->InsideFrustum(modelCullSphere);
			bool insideMapFrustum = false;
			if (!skipMap)
			{
				insideMapFrustum = aMapCamera->InsideFrustum(modelCullSphere);
			}
			if (!insideViewFrustum && !insideMapFrustum)
			{
				continue;
			}
			addToView = insideViewFrustum && toModelLengthSqr <= drawDistanceSqr;
		}

		if (addToView)
		{
			switch (aCullList[modelIndex]->GetRenderMode())
			{
			case ModelRenderMode::DeferredInstanced:
				outCulledCollection->myModelsInstanced.Add(aCullList[modelIndex]);
				break;
			case ModelRenderMode::Deferred:
				outCulledCollection->myModelsDeferred.Add(aCullList[modelIndex]);
				break;
			case ModelRenderMode::Forward:
				outCulledCollection->myModelsForward.Add(aCullList[modelIndex]);
				break;
			case ModelRenderMode::Wireframe:
				outCulledCollection->myModelsWireframe.Add(aCullList[modelIndex]);
				break;
			default:
				break;
			}
		}
		if (!skipMap && toModelMapLengthSqr <= aMapRange)
		{
			outCulledCollection->myModelsMap.Add(aCullList[modelIndex]);
		}
	}
	if (outCulledCollection->myModelsForward.Size() > 1)
	{
		InsertionSortForwardModels(outCulledCollection->myModelsForward, cameraPosition);
	}

	//ModelInstance::InsertionSortInstanced(outCulledCollection->myModelsInstanced, outCulledCollection->myModelInstancedStartIndexList);
	//ModelInstance::InsertionSortInstanced(outCulledCollection->myModelsMap, outCulledCollection->myModelMapStartIndexList);
	ModelInstance::InsertionSortInstanced(outCulledCollection->myModelsInstanced);
	ModelInstance::InsertionSortInstanced(outCulledCollection->myModelsMap);

	outCulledCollection->myIsCulled = true;
}