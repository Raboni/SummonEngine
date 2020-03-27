#pragma once
#include "ModelRenderCommand.h"
#include "PointLightRenderCommand.h"
#include "ParticleEmitterRenderCommand.h"
#include "SpriteRenderCommand.h"
#include "LineRenderCommand.h"
#include "MapRenderCommand.h"
#include "EnviromentLight.h"
#include "TextInstance.h"
#include "Navmesh.h"
#include <Camera.hpp>
#include <GrowingArray.hpp>

struct RenderBuffer
{
	RenderBuffer()
	{
		myModelsDeferred.Init(32);
		myModelsInstanced.Init(32);
		myModelsForward.Init(32);
		myModelsMap.Init(32);
		myModelsWireframe.Init(32);
		myModelInstancedStartIndexList.Init(8);
		myModelMapStartIndexList.Init(8);
		myModelPointLights.Init(1);
		myPointLights.Init(8);
		myNavmeshes.Init(1);
		myParticleEmitters.Init(8);
		mySprites.Init(16);
		myTexts.Init(32);
		myLines.Init(32);
		myUseViewCamera = false;
		myUseMapCamera = false;
		myRenderFustrum = false;
	}
	void Reset()
	{
		myMap.mySprite = nullptr;
		myMap.myZoom = 1.0f;
		mySkybox.myModel = nullptr;
		/*myModelsDeferred = CU::GrowingArray<ModelRenderCommand>(32);
		myModelsForward = CU::GrowingArray<ModelRenderCommand>(32);
		myModelDeferredStartIndexList = CU::GrowingArray<int>(8);
		myModelPointLights = CU::GrowingArray<CU::GrowingArray<PointLightRenderCommand>>(1);
		myPointLights = CU::GrowingArray<PointLightRenderCommand>(8);
		myParticleEmitters = CU::GrowingArray<ParticleEmitterRenderCommand>(8);
		mySprites = CU::GrowingArray<SpriteRenderCommand>(16);
		myTexts = CU::GrowingArray<TextInstance>(32);
		myLines = CU::GrowingArray<LineRenderCommand>(32);
		myNavmeshes = CU::GrowingArray<Nav::Navmesh>(1);*/
		myModelsDeferred.RemoveAll();
		myModelsInstanced.RemoveAll();
		myModelsForward.RemoveAll();
		myModelsMap.RemoveAll();
		myModelsWireframe.RemoveAll();
		myModelInstancedStartIndexList.RemoveAll();
		myModelMapStartIndexList.RemoveAll();
		myModelPointLights.RemoveAll();
		myPointLights.RemoveAll();
		myNavmeshes.RemoveAll();
		myParticleEmitters.RemoveAll();
		mySprites.RemoveAll();
		myTexts.RemoveAll();
		myLines.RemoveAll();
		myUseViewCamera = false;
		myUseMapCamera = false;
		myRenderFustrum = false;
	}
	CU::Camera myMainCamera;
	CU::Camera myViewCamera;
	CU::Camera myMapCamera;
	CU::Camera myVRCameraL;
	CU::Camera myVRCameraR;
	CU::Matrix4x4f myLeftEyeDisplacement;
	CU::Matrix4x4f myRightEyeDisplacement;
	EnviromentLight myEnviromentLight;
	MapRenderCommand myMap;
	ModelRenderCommand mySkybox;
	CU::GrowingArray<ModelRenderCommand> myModelsDeferred;
	CU::GrowingArray<ModelRenderCommand> myModelsInstanced;
	CU::GrowingArray<ModelRenderCommand> myModelsForward;
	CU::GrowingArray<ModelRenderCommand> myModelsMap;
	CU::GrowingArray<ModelRenderCommand> myModelsWireframe;
	CU::GrowingArray<int> myModelInstancedStartIndexList;
	CU::GrowingArray<int> myModelMapStartIndexList;
	CU::GrowingArray<CU::GrowingArray<PointLightRenderCommand>> myModelPointLights;
	CU::GrowingArray<PointLightRenderCommand> myPointLights;
	CU::GrowingArray<Nav::Navmesh> myNavmeshes;
	CU::GrowingArray<ParticleEmitterRenderCommand> myParticleEmitters;
	CU::GrowingArray<SpriteRenderCommand> mySprites;
	CU::GrowingArray<TextInstance> myTexts;
	CU::GrowingArray<LineRenderCommand> myLines;
	bool myUseViewCamera;
	bool myUseMapCamera;
	bool myRenderFustrum;
};