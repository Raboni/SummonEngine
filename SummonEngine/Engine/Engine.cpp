#include "stdafx.h"
#include "Engine.h"
#include "ParticleEmitterLoader.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ModelInstance.h"
#include "SpriteInstance.h"
#include "Culling.h"
#include "Threadpool.h"
#include "SpriteUnits.h"
#include <PostMaster.hpp>
#include <Input.hpp>
#include <chrono>

Engine::Engine() : myFramework(myRenderSettings), myRenderer(myRenderSettings)
{
	CU::Input::Create();
	EnginePostMaster::CreateInstance();
	myActiveScene = 0;
	myLastRenderDuration = 0.0f;
	myLastCullingDuration = 0.0f;
	myUseVR = false;

	EngineProxy::ourEngine = this;
}
Engine::~Engine()
{
	EnginePostMaster::DeleteInstance();
	CU::Input::Destroy();
}

bool Engine::Init(WindowHandler::WindowData& aWinData, const bool aUseVR)
{
	EnginePostMaster::GetInstance()->Subscribe(EngineMessages::ResolutionChanged, &myWinHandler);
	EnginePostMaster::GetInstance()->Subscribe(EngineMessages::ResolutionChanged, this);

	VRManager* vrManager = nullptr;
	myUseVR = aUseVR;
	if (myUseVR && !myVRManager.Init())
	{
		myUseVR = false;
	}
	if (myUseVR)
	{
		vrManager = &myVRManager;
		if (myRenderSettings.VRMirrorEye)
		{
			aWinData.myWidth = myVRManager.GetEyeRenderTargetSize().x;
			aWinData.myHeight = myVRManager.GetEyeRenderTargetSize().y;
		}
	}
	if (!myWinHandler.Init(aWinData))
	{
		return false;
	}
	if (!myFramework.Init(myWinHandler))
	{
		return false;
	}
	if (!myRenderer.Init(this, &myFramework, &myWinHandler, vrManager))
	{
		return false;
	}

	return true;
}

void Engine::BeginFrame()
{
	SwitchAndCleanBuffers();
	myFramework.BeginFrame();

	CU::Input::Get()->UpdateXbox();
}
void Engine::EndFrame()
{
	myRenderSettings = myRenderSettingsBuffer;
	myFramework.EndFrame();
	EnginePostMaster::GetInstance()->Dispatch();
}

void Engine::Render()
{
	const std::chrono::high_resolution_clock::time_point timerStart = std::chrono::high_resolution_clock::now();
	myRenderer.Render();
	const std::chrono::duration<float, std::milli> timeSpan = (std::chrono::high_resolution_clock::now() - timerStart) / 1000;
	myLastRenderDuration = timeSpan.count();
}
void Engine::Update()
{
	if (myUseVR)
	{
		myVRManager.Update();
	}
	ModelLoader::GetInstance()->UpdateCustomMeshes();
	CreateRenderCommands(myRenderSettings);
}
void Engine::UpdateAnimation(const float aDeltaTime)
{
	aDeltaTime;
}

void Engine::SetActiveScene(const unsigned int aSceneIndex)
{
	myActiveScene = aSceneIndex;
}
Scene* Engine::GetActiveScene()
{
	return &myScene[myActiveScene];
}
Scene* Engine::GetScene(const unsigned int aSceneIndex)
{
	return &myScene[aSceneIndex];
}

HWND Engine::GetWindow()
{
	return myWinHandler.GetWindow();
}
CU::Vector2i Engine::GetWindowSize()
{
	return myWinHandler.GetWindowSize<int>();
}

void Engine::OpenConsole(const CU::Vector2i& aPosition, const CU::Vector2i& aSize, const std::string& aTitle)
{
	#ifndef _RETAIL
	myWinHandler.OpenConsole(aPosition, aSize, aTitle);
	#endif
}
void Engine::CloseConsole()
{
	#ifndef _RETAIL
	myWinHandler.CloseConsole();
	#endif
}

void Engine::SetUseVR(const bool aActive)
{
	if (myUseVR != aActive)
	{
		myUseVR = aActive;
		if (myUseVR)
		{
			if (!myVRManager.Init())
			{
				myUseVR = false;
				return;
			}
			if (myRenderSettings.VRMirrorEye)
			{
				myWinHandler.SetWindowSize(myVRManager.GetEyeRenderTargetSize().x, myVRManager.GetEyeRenderTargetSize().y);
			}
			myRenderer.InitVR(&myVRManager);
		}
		else
		{
			myVRManager.Quit();
			myRenderer.QuitVR();
		}
	}
}
bool Engine::GetUseVR()
{
	return myUseVR;
}

RenderSettings& Engine::GetRenderSettings()
{
	return myRenderSettingsBuffer;
}

float Engine::GetLastRenderDuration() const
{
	return myLastRenderDuration;
}
float Engine::GetLastCullingDuration() const
{
	return myLastCullingDuration;
}

RenderBuffer& Engine::GetRenderBuffer()
{
	if (myRenderBufferA)
	{
		return myBufferA;
	}
	else
	{
		return myBufferB;
	}
}

void Engine::Draw(const LineRenderCommand& aLine)
{
	RenderBuffer* buffer;
	if (myRenderBufferA)
	{
		buffer = &myBufferB;
	}
	else
	{
		buffer = &myBufferA;
	}
	buffer->myLines.Add(aLine);
}
void Engine::Draw(SpriteInstance* aSprite)
{
	if (aSprite == nullptr)
	{
		return;
	}

	RenderBuffer* buffer;
	if (myRenderBufferA)
	{
		buffer = &myBufferB;
	}
	else
	{
		buffer = &myBufferA;
	}
	int spriteIndex = -1;
	for (int spriteBufferIndex = 0; spriteBufferIndex < buffer->mySprites.Size(); spriteBufferIndex++)
	{
		if (buffer->mySprites[spriteBufferIndex].mySprite == aSprite->GetSprite())
		{
			spriteIndex = spriteBufferIndex;
			break;
		}
	}
	if (spriteIndex != -1)
	{
		buffer->mySprites[spriteIndex].myData.Add(aSprite->GetData());
	}
	else
	{
		buffer->mySprites.Add(aSprite);
	}
}
void Engine::Draw(TextInstance* aText)
{
	if (aText == nullptr)
	{
		return;
	}

	RenderBuffer* buffer;
	if (myRenderBufferA)
	{
		buffer = &myBufferB;
	}
	else
	{
		buffer = &myBufferA;
	}
	buffer->myTexts.Add(*aText);
}

void Engine::CreateRenderCommands(RenderSettings& aSettings)
{
	RenderBuffer* buffer;
	if (myRenderBufferA)
	{
		buffer = &myBufferB;
	}
	else
	{
		buffer = &myBufferA;
	}

	//memcpy(&buffer->LeftEyeDisplacement, &myLeftEyeDisplacement, sizeof(CU::Matrix4x4f));
	//memcpy(&buffer->RightEyeDisplacement, &myRightEyeDisplacement, sizeof(CU::Matrix4x4f));

	CU::Camera* cullCamera = myScene[myActiveScene].myMainCamera;
	CU::Camera* mapCamera = myScene[myActiveScene].myMapCamera;
	if (myScene[myActiveScene].myMainCamera != nullptr)
	{
		buffer->myMainCamera = *myScene[myActiveScene].myMainCamera;
	}
	buffer->myUseViewCamera = (myScene[myActiveScene].myViewCamera != nullptr);
	buffer->myUseMapCamera = (myScene[myActiveScene].myMapCamera != nullptr);
	if (buffer->myUseViewCamera)
	{
		buffer->myViewCamera = *myScene[myActiveScene].myViewCamera;
	}
	if (buffer->myUseMapCamera)
	{
		buffer->myMapCamera = *myScene[myActiveScene].myMapCamera;
	}
	if (myScene[myActiveScene].myVRCamera.IsValid())
	{
		buffer->myVRCameraL = *myScene[myActiveScene].myVRCamera.GetCameraLeft();
		buffer->myVRCameraR = *myScene[myActiveScene].myVRCamera.GetCameraRight();
	}
	buffer->myRenderFustrum = myRenderFustrum;
	buffer->myEnviromentLight = *myScene[myActiveScene].myEnviromentLight;
	buffer->myMap = myScene[myActiveScene].myMap;
	buffer->mySkybox = myScene[myActiveScene].mySkybox;

	std::chrono::high_resolution_clock::time_point timerStart = std::chrono::high_resolution_clock::now();
	CullingCollection culledCollection;
	if (aSettings.ThreadCulling)
	{
		CU::GrowingArray<ModelInstance*, int>& modelList = myScene[myActiveScene].myModels;
		if (modelList.Size() > 64)
		{
			CullingCollection cullingCollection[8];
			CU::GrowingArray<ModelInstance*, int> cullingLists[8];
			const int cullingListSize = modelList.Size() / 8;
			for (int collectionIndex = 1; collectionIndex < 8; collectionIndex++)
			{
				cullingLists[collectionIndex] = CU::GrowingArray<ModelInstance*, int>(&modelList[cullingListSize * collectionIndex], cullingListSize);
				Threadpool::GetInstance()->AddTask(std::bind(&CullModelList, cullingLists[collectionIndex], &cullingCollection[collectionIndex], aSettings, cullCamera, mapCamera, aSettings.MapRange), false);
			}
			cullingLists[0] = CU::GrowingArray<ModelInstance*, int>(modelList.GetPointer(), cullingListSize);
			CullModelList(cullingLists[0], &cullingCollection[0], aSettings, cullCamera, mapCamera, aSettings.MapRange);

			if (!cullingCollection[0].myIsCulled)
			{
				//shit is fucked
			}
			culledCollection.myModelsInstanced = cullingCollection[0].myModelsInstanced;
			culledCollection.myModelsDeferred = cullingCollection[0].myModelsDeferred;
			culledCollection.myModelsForward = cullingCollection[0].myModelsForward;
			culledCollection.myModelsWireframe = cullingCollection[0].myModelsWireframe;
			culledCollection.myModelsMap = cullingCollection[0].myModelsMap;
			//culledCollection.myModelInstancedStartIndexList = cullingCollection[0].myModelInstancedStartIndexList;
			//culledCollection.myModelMapStartIndexList = cullingCollection[0].myModelMapStartIndexList;
			culledCollection.myIsCulled = true;
			for (int collectionIndex = 1; collectionIndex < 8; collectionIndex++)
			{
				while (!cullingCollection[collectionIndex].myIsCulled)
				{
					std::this_thread::yield();
				}

				for (int index = 0; index < cullingCollection[collectionIndex].myModelInstancedStartIndexList.Size(); index++)
				{
					cullingCollection[collectionIndex].myModelInstancedStartIndexList[index] += culledCollection.myModelsInstanced.Size();
				}
				for (int index = 0; index < cullingCollection[collectionIndex].myModelMapStartIndexList.Size(); index++)
				{
					cullingCollection[collectionIndex].myModelMapStartIndexList[index] += culledCollection.myModelsMap.Size();
				}

				culledCollection.myModelsInstanced += cullingCollection[collectionIndex].myModelsInstanced;
				culledCollection.myModelsDeferred += cullingCollection[collectionIndex].myModelsDeferred;
				culledCollection.myModelsForward += cullingCollection[collectionIndex].myModelsForward;
				culledCollection.myModelsWireframe += cullingCollection[collectionIndex].myModelsWireframe;
				culledCollection.myModelsMap += cullingCollection[collectionIndex].myModelsMap;
				//culledCollection.myModelInstancedStartIndexList += cullingCollection[collectionIndex].myModelInstancedStartIndexList;
				//culledCollection.myModelMapStartIndexList += cullingCollection[collectionIndex].myModelMapStartIndexList;
			}
		}
		else
		{
			CullModelList(modelList, &culledCollection, aSettings, cullCamera, mapCamera, aSettings.MapRange);
		}
	}
	else
	{
		CullModelList(myScene[myActiveScene].myModels, &culledCollection, aSettings, cullCamera, mapCamera, aSettings.MapRange);
	}
	ModelInstance::InsertionSortInstanced(culledCollection.myModelsInstanced, culledCollection.myModelInstancedStartIndexList);
	ModelInstance::InsertionSortInstanced(culledCollection.myModelsMap, culledCollection.myModelMapStartIndexList);
	for (int modelIndex = 0; modelIndex < culledCollection.myModelsDeferred.Size(); modelIndex++)
	{
		culledCollection.myModelsDeferred[modelIndex]->GetModel()->AddUser();
		buffer->myModelsDeferred.Add(culledCollection.myModelsDeferred[modelIndex]);
	}
	for (int modelIndex = 0; modelIndex < culledCollection.myModelsInstanced.Size(); modelIndex++)
	{
		culledCollection.myModelsInstanced[modelIndex]->GetModel()->AddUser();
		buffer->myModelsInstanced.Add(culledCollection.myModelsInstanced[modelIndex]);
	}
	for (int modelIndex = 0; modelIndex < culledCollection.myModelsForward.Size(); modelIndex++)
	{
		culledCollection.myModelsForward[modelIndex]->GetModel()->AddUser();
		buffer->myModelsForward.Add(culledCollection.myModelsForward[modelIndex]);
	}
	for (int modelIndex = 0; modelIndex < culledCollection.myModelsWireframe.Size(); modelIndex++)
	{
		culledCollection.myModelsWireframe[modelIndex]->GetModel()->AddUser();
		buffer->myModelsWireframe.Add(culledCollection.myModelsWireframe[modelIndex]);
	}
	for (int modelIndex = 0; modelIndex < culledCollection.myModelsMap.Size(); modelIndex++)
	{
		culledCollection.myModelsMap[modelIndex]->GetModel()->AddUser();
		buffer->myModelsMap.Add(culledCollection.myModelsMap[modelIndex]);
	}
	buffer->myModelInstancedStartIndexList = culledCollection.myModelInstancedStartIndexList;
	buffer->myModelMapStartIndexList = culledCollection.myModelMapStartIndexList;

	CU::GrowingArray<PointLightInstance*, int> pointLights = myScene[myActiveScene].GetLightsCulled(aSettings, cullCamera);
	for (int lightIndex = 0; lightIndex < pointLights.Size(); lightIndex++)
	{
		buffer->myPointLights.Add(pointLights[lightIndex]);
	}
	CU::GrowingArray<PointLightInstance*, int> modelsPointLights;
	buffer->myModelPointLights.Reserve(culledCollection.myModelsForward.Size());
	buffer->myModelPointLights.Resize(culledCollection.myModelsForward.Size());
	for (int modelIndex = 0; modelIndex < culledCollection.myModelsForward.Size(); modelIndex++)
	{
		modelsPointLights = myScene[myActiveScene].GetLightsCulled(culledCollection.myModelsForward[modelIndex]);
		buffer->myModelPointLights[modelIndex].Init(modelsPointLights.Size());
		buffer->myModelPointLights[modelIndex].Resize(modelsPointLights.Size());
		for (int lightIndex = 0; lightIndex < modelsPointLights.Size(); lightIndex++)
		{
			buffer->myModelPointLights[modelIndex][lightIndex] = modelsPointLights[lightIndex];
		}
	}

	ParticleEmitterInstance* emitterInstance = nullptr;
	int emitterIndex = -1;
	for (int particleIndex = 0; particleIndex < myScene[myActiveScene].myParticleEmitters.Size(); particleIndex++)
	{
		emitterInstance = myScene[myActiveScene].myParticleEmitters[particleIndex];
		ParticleEmitterLoader::GetInstance()->UpdateParticleEmitterBuffer(emitterInstance->GetEmitter());
		emitterIndex = -1;
		for (int particleBufferIndex = 0; particleBufferIndex < buffer->myParticleEmitters.Size(); particleBufferIndex++)
		{
			if (buffer->myParticleEmitters[particleBufferIndex].myEmitter == emitterInstance->GetEmitter())
			{
				emitterIndex = particleBufferIndex;
				break;
			}
		}
		if (emitterIndex != -1)
		{
			buffer->myParticleEmitters[emitterIndex].myParticleList += emitterInstance->GetParticleList();
		}
		else
		{
			buffer->myParticleEmitters.Add(emitterInstance);
		}
	}
	CU::Vector3f cameraPos;
	if (myScene[myActiveScene].myMainCamera != nullptr)
	{
		cameraPos = myScene[myActiveScene].myMainCamera->GetPosition();
	}
	for (int particleBufferIndex = 0; particleBufferIndex < buffer->myParticleEmitters.Size(); particleBufferIndex++)
	{
		CU::GrowingArray<ParticleData, int>& particleList = buffer->myParticleEmitters[particleBufferIndex].myParticleList;
		for (int particleIndex = 0; particleIndex < particleList.Size(); particleIndex++)
		{
			particleList[particleIndex].myDistanceToCameraSqr = (CU::Vector3f(particleList[particleIndex].myPosition) - cameraPos).LengthSqr();
		}
		ParticleEmitterInstance::InsertionSortParticle(particleList);
	}
	const std::chrono::duration<float, std::milli> timeSpan = (std::chrono::high_resolution_clock::now() - timerStart) / 1000;
	myLastCullingDuration = timeSpan.count();

	SpriteInstance* spriteInstance = nullptr;
	int spriteIndex = -1;
	for (int instanceIndex = 0; instanceIndex < myScene[myActiveScene].mySprites.Size(); instanceIndex++)
	{
		spriteInstance = myScene[myActiveScene].mySprites[instanceIndex];
		spriteIndex = -1;
		for (int spriteBufferIndex = 0; spriteBufferIndex < buffer->mySprites.Size(); spriteBufferIndex++)
		{
			if (buffer->mySprites[spriteBufferIndex].mySprite == spriteInstance->GetSprite())
			{
				spriteIndex = spriteBufferIndex;
				break;
			}
		}
		if (spriteIndex != -1)
		{
			buffer->mySprites[spriteIndex].myData.Add(spriteInstance->GetData());
		}
		else
		{
			buffer->mySprites.Add(spriteInstance);
			spriteInstance->GetSprite()->AddUser();
		}
	}
	/*for (unsigned short spriteIndex = 0; spriteIndex < mySprites.Size(); spriteIndex++)
	{
		buffer->Sprites.Add(mySprites[spriteIndex]);
	}*/

	for (int navmeshIndex = 0; navmeshIndex < myScene[myActiveScene].myNavmeshes.Size(); navmeshIndex++)
	{
		buffer->myNavmeshes.Add(*myScene[myActiveScene].myNavmeshes[navmeshIndex]);
	}
}

void Engine::SwitchAndCleanBuffers()
{
	myRenderBufferA = !myRenderBufferA;
	RenderBuffer& bufferToRender = myRenderBufferA ? myBufferA : myBufferB;
	RenderBuffer& bufferToClean = myRenderBufferA ? myBufferB : myBufferA;

	bufferToClean.myMap.mySprite = nullptr;
	bufferToClean.mySkybox.myModel = nullptr;

	for (int modelIndex = 0; modelIndex < bufferToClean.myModelsDeferred.Size(); modelIndex++)
	{
		bufferToClean.myModelsDeferred[modelIndex].myModel->RemoveUser();
	}
	bufferToClean.myModelsDeferred.RemoveAll();
	for (int modelIndex = 0; modelIndex < bufferToClean.myModelsInstanced.Size(); modelIndex++)
	{
		bufferToClean.myModelsInstanced[modelIndex].myModel->RemoveUser();
	}
	bufferToClean.myModelsInstanced.RemoveAll();
	bufferToClean.myModelInstancedStartIndexList.RemoveAll();
	for (int modelIndex = 0; modelIndex < bufferToClean.myModelsForward.Size(); modelIndex++)
	{
		bufferToClean.myModelsForward[modelIndex].myModel->RemoveUser();
	}
	bufferToClean.myModelsForward.RemoveAll();
	for (int modelIndex = 0; modelIndex < bufferToClean.myModelsWireframe.Size(); modelIndex++)
	{
		bufferToClean.myModelsWireframe[modelIndex].myModel->RemoveUser();
	}
	bufferToClean.myModelsWireframe.RemoveAll();
	for (int modelIndex = 0; modelIndex < bufferToClean.myModelsMap.Size(); modelIndex++)
	{
		bufferToClean.myModelsMap[modelIndex].myModel->RemoveUser();
	}
	bufferToClean.myModelsMap.RemoveAll();

	bufferToClean.myPointLights.RemoveAll();
	bufferToClean.myParticleEmitters.RemoveAll();
	for (int spriteIndex = 0; spriteIndex < bufferToClean.mySprites.Size(); spriteIndex++)
	{
		bufferToClean.mySprites[spriteIndex].mySprite->RemoveUser();
	}
	bufferToClean.mySprites.RemoveAll();
	bufferToClean.myTexts.RemoveAll();
	bufferToClean.myLines.RemoveAll();
	bufferToClean.myNavmeshes.RemoveAll();
	bufferToClean.myUseViewCamera = false;
	bufferToClean.myUseMapCamera = false;
	bufferToClean.myRenderFustrum = false;

	if (myDirtyBuffer)
	{
		bufferToRender.myMap.mySprite = nullptr;
		bufferToRender.mySkybox.myModel = nullptr;

		for (int modelIndex = 0; modelIndex < bufferToRender.myModelsDeferred.Size(); modelIndex++)
		{
			bufferToRender.myModelsDeferred[modelIndex].myModel->RemoveUser();
		}
		bufferToRender.myModelsDeferred.RemoveAll();
		for (int modelIndex = 0; modelIndex < bufferToRender.myModelsInstanced.Size(); modelIndex++)
		{
			bufferToRender.myModelsInstanced[modelIndex].myModel->RemoveUser();
		}
		bufferToRender.myModelsInstanced.RemoveAll();
		bufferToRender.myModelInstancedStartIndexList.RemoveAll();
		for (int modelIndex = 0; modelIndex < bufferToRender.myModelsForward.Size(); modelIndex++)
		{
			bufferToRender.myModelsForward[modelIndex].myModel->RemoveUser();
		}
		bufferToRender.myModelsForward.RemoveAll();
		for (int modelIndex = 0; modelIndex < bufferToRender.myModelsWireframe.Size(); modelIndex++)
		{
			bufferToRender.myModelsWireframe[modelIndex].myModel->RemoveUser();
		}
		bufferToRender.myModelsWireframe.RemoveAll();
		for (int modelIndex = 0; modelIndex < bufferToRender.myModelsMap.Size(); modelIndex++)
		{
			bufferToRender.myModelsMap[modelIndex].myModel->RemoveUser();
		}
		bufferToRender.myModelsMap.RemoveAll();

		bufferToRender.myPointLights.RemoveAll();
		bufferToRender.myParticleEmitters.RemoveAll();
		bufferToRender.mySprites.RemoveAll();
		bufferToRender.myTexts.RemoveAll();
		bufferToRender.myLines.RemoveAll();
		bufferToRender.myNavmeshes.RemoveAll();
		bufferToRender.myUseViewCamera = false;
		bufferToRender.myUseMapCamera = false;
		bufferToRender.myRenderFustrum = false;

		myDirtyBuffer = false;
	}
}

void Engine::SetShouldRenderFustrum(const bool aShouldRender)
{
	myRenderFustrum = aShouldRender;
}

void Engine::WaitForLoading()
{
	while (IsLoading())
	{
		std::this_thread::yield();
	}
}
bool Engine::IsLoading() const
{
	return ModelLoader::GetInstance()->IsLoading();
}

void Engine::RecieveMessage(const CU::Message<EngineMessages>& aMessage)
{
	switch (aMessage.GetType())
	{
	case EngineMessages::ResolutionChanged:
		if (myScene[myActiveScene].GetViewCamera() != nullptr)
		{
			myScene[myActiveScene].GetViewCamera()->SetAspectRatio(static_cast<float>(myWinHandler.GetWidth()) / static_cast<float>(myWinHandler.GetHeight()));
		}
		break;
	default:
		break;
	}
}
