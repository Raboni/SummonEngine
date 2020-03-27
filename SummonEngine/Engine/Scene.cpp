#include "stdafx.h"
#include "Scene.h"
#include "ModelLoader.h"
#include "SpriteLoader.h"
#include "ModelInstance.h"
#include "EnviromentLight.h"
#include "ParticleEmitterInstance.h"
#include "SpriteInstance.h"
#include "TextInstance.h"
#include "Culling.h"
#undef min
#undef max
#include <Intersection.hpp>

void InsertionSortForwardLights(CU::GrowingArray<PointLightInstance*, int>& aUnsortedArray, const CU::Vector3f& aModelPosition)
{
	int pos = 0;
	float distance;
	PointLightInstance* temp;

	for (int i = 1; i < aUnsortedArray.Size(); i++)
	{
		pos = i - 1;
		temp = aUnsortedArray[i];
		distance = (temp->GetPosition() - aModelPosition).LengthSqr();

		while (pos >= 0 && distance < (aUnsortedArray[pos]->GetPosition() - aModelPosition).LengthSqr())//closest light at start
		{
			aUnsortedArray[pos + 1] = aUnsortedArray[pos];
			pos--;
		}

		aUnsortedArray[pos + 1] = temp;
	}
}
void InsertionSortForwardLights(CU::GrowingArray<PointLightInstance, int>& aUnsortedArray, const CU::Vector3f& aModelPosition)
{
	int pos = 0;
	float distance;
	PointLightInstance* temp;

	for (int i = 1; i < aUnsortedArray.Size(); i++)
	{
		pos = i - 1;
		temp = &aUnsortedArray[i];
		distance = (temp->GetPosition() - aModelPosition).LengthSqr();

		while (pos >= 0 && distance < (aUnsortedArray[pos].GetPosition() - aModelPosition).LengthSqr())//closest light at start
		{
			aUnsortedArray[pos + 1] = aUnsortedArray[pos];
			pos--;
		}

		aUnsortedArray[pos + 1] = *temp;
	}
}

/*CU::Vector3f ClosestPointOnSphere(const CU::Intersection::Sphere& aSphere, const CU::Intersection::Line3D& aLine)
{
	float dot = aLine.direction.Dot(aSphere.center - aLine.point);
	CU::Vector3f closestPointOnLine = aLine.point + (aLine.direction * dot);
	CU::Vector3f distance = closestPointOnLine - aSphere.center;
	if (distance.LengthSqr() > aSphere.radius * aSphere.radius)
	{
		return (aSphere.center + distance.GetNormalized() * aSphere.radius);//fix needed
	}
	else
	{
		return closestPointOnLine;
	}
}*/

Scene::Scene()
{
	myModels.Init(32);
	myLights.Init(4);
	myParticleEmitters.Init(4);
	mySprites.Init(16);
	myNavmeshes.Init(1);

	myMainCamera = nullptr;
	myViewCamera = nullptr;
	myMapCamera = nullptr;
	myEnviromentLight = nullptr;

	mySkybox.Init("Skybox");
	mySkybox.SetRenderMode(ModelRenderMode::Forward);
}
Scene::Scene(const Scene& aScene)
{
	myMap = aScene.myMap;
	myVRCamera = aScene.myVRCamera;

	myModels = aScene.myModels;
	myLights = aScene.myLights;
	myParticleEmitters = aScene.myParticleEmitters;
	mySprites = aScene.mySprites;
	myNavmeshes = aScene.myNavmeshes;

	myMainCamera = aScene.myMainCamera;
	myViewCamera = aScene.myViewCamera;
	myMapCamera = aScene.myMapCamera;
	myEnviromentLight = aScene.myEnviromentLight;

	if (myEnviromentLight != nullptr && !mySkybox.HasCalledLoad())
	{
		mySkybox.Load();
	}
}
Scene::~Scene()
{
}

Scene& Scene::operator=(const Scene& aScene)
{
	myMap = aScene.myMap;
	myVRCamera = aScene.myVRCamera;

	myModels = aScene.myModels;
	myLights = aScene.myLights;
	myParticleEmitters = aScene.myParticleEmitters;
	mySprites = aScene.mySprites;
	myNavmeshes = aScene.myNavmeshes;

	myMainCamera = aScene.myMainCamera;
	myViewCamera = aScene.myViewCamera;
	myEnviromentLight = aScene.myEnviromentLight;

	if (myEnviromentLight != nullptr && !mySkybox.HasCalledLoad())
	{
		mySkybox.Load();
	}
	return *this;
}

void Scene::WaitForLoading()
{
	while (ModelLoader::GetInstance()->IsLoading())
	{
		std::this_thread::yield();
	}
}

void Scene::AddInstance(ModelInstance* aModel)
{
	myModels.Add(aModel);
}
void Scene::AddInstance(PointLightInstance* aLight)
{
	myLights.Add(aLight);
}
void Scene::AddInstance(ParticleEmitterInstance* aParticleEmitter)
{
	myParticleEmitters.Add(aParticleEmitter);
}
void Scene::AddInstance(SpriteInstance* aSprite)
{
	mySprites.Add(aSprite);
}
void Scene::AddInstance(Nav::Navmesh* aNavmesh)
{
	myNavmeshes.Add(aNavmesh);
}

void Scene::RemoveInstance(ModelInstance* aModel)
{
	if (myModels.Size() == 0)
	{
		return;
	}
	myModels.Remove(aModel);
}
void Scene::RemoveInstance(PointLightInstance* aLight)
{
	if (myLights.Size() == 0)
	{
		return;
	}
	myLights.RemoveCyclic(aLight);
}
void Scene::RemoveInstance(ParticleEmitterInstance* aParticleEmitter)
{
	if (myParticleEmitters.Size() == 0)
	{
		return;
	}
	myParticleEmitters.RemoveCyclic(aParticleEmitter);
}
void Scene::RemoveInstance(SpriteInstance* aSprite)
{
	if (mySprites.Size() == 0)
	{
		return;
	}
	mySprites.RemoveCyclic(aSprite);
}
void Scene::RemoveInstance(Nav::Navmesh* aNavmesh)
{
	if (myNavmeshes.Size() == 0)
	{
		return;
	}
	myNavmeshes.RemoveCyclic(aNavmesh);
}

void Scene::SetEnviromentLight(EnviromentLight* aLight)
{
	myEnviromentLight = aLight;
	if (!mySkybox.HasCalledLoad())
	{
		mySkybox.Load();
	}
}
void Scene::SetMainCamera(CU::Camera* aCamera)
{
	myMainCamera = aCamera;
}
void Scene::SetViewCamera(CU::Camera* aCamera)
{
	myViewCamera = aCamera;
}
void Scene::SetVRCameraLeft(CU::Camera* aCamera)
{
	myVRCamera.SetCameraLeft(aCamera);
}
void Scene::SetVRCameraRight(CU::Camera* aCamera)
{
	myVRCamera.SetCameraRight(aCamera);
}

void Scene::SetMap(SpriteInstance* aSprite)
{
	myMap.mySprite = aSprite->GetSprite();
}
void Scene::SetMap(CU::Camera* aCamera)
{
	myMapCamera = aCamera;
}
void Scene::SetMapPositionScreen(const CU::Vector2f& aPosition)
{
	myMap.myPositionScreen = aPosition;
}
void Scene::SetMapSizeScreen(const CU::Vector2f& aSize)
{
	myMap.mySizeScreen = aSize;
}
void Scene::SetMapPositionWorld(const CU::Vector2f& aPosition)
{
	myMap.myPositionWorld = aPosition;
}
void Scene::SetMapSizeWorld(const CU::Vector2f& aSize)
{
	myMap.mySizeWorld = aSize;
}
void Scene::SetMapFocus(const CU::Vector2f& aFocus)
{
	myMap.myFocus = aFocus;
}
void Scene::SetMapRotation(const float aRotation)
{
	myMap.myRotation = aRotation;
}
void Scene::SetMapZoom(const float aZoom)
{
	myMap.myZoom = aZoom;
}

CU::GrowingArray<ModelInstance*, int> Scene::GetModelsCulled(RenderSettings& aSettings, const CU::Camera* aCamera, const bool aUseDeferred) const
{
	if (aCamera == nullptr)
	{
		return myModels;
	}

	CU::GrowingArray<ModelInstance*, int> culledList(myModels);
	const CU::Vector3f cameraPosition = aCamera->GetPosition();
	//CU::Intersection::Line3D cameraViewLine(cameraPosition, aCamera->GetDirection());

	CU::Vector3f toModel;
	float drawDistanceSqr = aSettings.DrawDistance * aSettings.DrawDistance;
	for (int modelIndex = 0; modelIndex < culledList.Size(); modelIndex++)
	{
		if (culledList[modelIndex]->IsDeferred() != aUseDeferred)
		{
			culledList.RemoveCyclicAtIndex(modelIndex--);
			continue;
		}

		toModel = culledList[modelIndex]->GetPosition() - cameraPosition;
		if (toModel.LengthSqr() > drawDistanceSqr * 1.5f)
		{
			culledList[modelIndex]->Release();
		}
		else if (toModel.LengthSqr() < drawDistanceSqr * 1.25f)
		{
			if (!culledList[modelIndex]->HasCalledLoad())
			{
				culledList[modelIndex]->Load();
			}
		}
		
		if (toModel.LengthSqr() > drawDistanceSqr || !culledList[modelIndex]->HasModel())
		{
			culledList.RemoveCyclicAtIndex(modelIndex--);
		}
		else
		{
			const CU::Intersection::Sphere modelCullSphere(culledList[modelIndex]->GetPosition(), culledList[modelIndex]->GetModel()->GetEncapsulationRange() * culledList[modelIndex]->GetScale().Length());
			const bool outsideFrustum = !aCamera->InsideFrustum(modelCullSphere);
			if (outsideFrustum)
			{
				culledList.RemoveCyclicAtIndex(modelIndex--);
			}
		}
	}
	if (!aUseDeferred)
	{
		InsertionSortForwardModels(culledList, cameraPosition);
	}
	
	return culledList;
}
CU::GrowingArray<ModelInstance*, int> Scene::GetModelsCulledMap(RenderSettings& aSettings, const CU::Camera* aCamera, const float aMapRange) const
{
	aSettings;
	CU::GrowingArray<ModelInstance*, int> culledList(myModels);
	const CU::Vector3f cameraPosition = aCamera->GetPosition();
	//CU::Intersection::Line3D cameraViewLine(cameraPosition, aCamera->GetDirection());

	CU::Vector3f toModel;
	for (int modelIndex = 0; modelIndex < culledList.Size(); modelIndex++)
	{
		toModel = culledList[modelIndex]->GetPosition() - cameraPosition;
		toModel.y = 0.0f;
		if (toModel.LengthSqr() > aMapRange * 1.5f)
		{
			//culledList[modelIndex]->Release();
		}
		else if (toModel.LengthSqr() < aMapRange * 1.25f)
		{
			if (!culledList[modelIndex]->HasCalledLoad())
			{
				culledList[modelIndex]->Load();
			}
		}

		if (toModel.LengthSqr() > aMapRange || !culledList[modelIndex]->HasModel())
		{
			culledList.RemoveCyclicAtIndex(modelIndex--);
		}
		else
		{
			const CU::Intersection::Sphere modelCullSphere(culledList[modelIndex]->GetPosition(), culledList[modelIndex]->GetModel()->GetEncapsulationRange() * culledList[modelIndex]->GetScale().Length());
			const bool outsideFrustum = !aCamera->InsideFrustum(modelCullSphere);
			if (outsideFrustum)
			{
				culledList.RemoveCyclicAtIndex(modelIndex--);
			}
		}
	}

	return culledList;
}
const CU::GrowingArray<ModelInstance*>& Scene::GetModels() const
{
	return myModels;
}

CU::GrowingArray<PointLightInstance*, int> Scene::GetLightsCulled(RenderSettings& aSettings, const CU::Camera* aCamera) const
{
	if (aCamera == nullptr)
	{
		return myLights;
	}

	CU::GrowingArray<PointLightInstance*, int> culledList(myLights);
	const CU::Vector3f cameraPosition = aCamera->GetPosition();

	CU::Vector3f distance;
	const float drawDistanceSqr = (aSettings.DrawDistance * aSettings.DrawDistance) * 1.2f;
	for (int lightIndex = 0; lightIndex < culledList.Size(); lightIndex++)
	{
		distance = culledList[lightIndex]->GetPosition() - cameraPosition;
		if (distance.LengthSqr() > drawDistanceSqr)
		{
			culledList.RemoveCyclicAtIndex(lightIndex--);
		}
	}

	return culledList;
}
CU::GrowingArray<PointLightInstance*, int> Scene::GetLightsCulled(ModelInstance* aModel) const
{
	if (aModel == nullptr)
	{
		return CU::GrowingArray<PointLightInstance*, int>(2);
	}

	CU::GrowingArray<PointLightInstance*, int> culledList(myLights);
	const CU::Vector3f modelPosition = aModel->GetPosition();
	CU::Vector3f distance;
	float rangeSqr;
	for (int lightIndex = 0; lightIndex < culledList.Size(); lightIndex++)
	{
		distance = culledList[lightIndex]->GetPosition() - modelPosition;
		rangeSqr = culledList[lightIndex]->GetRange();
		rangeSqr *= rangeSqr;
		if (distance.LengthSqr() > rangeSqr)
		{
			culledList.RemoveCyclicAtIndex(lightIndex--);
		}
	}
	InsertionSortForwardLights(culledList, modelPosition);

	return culledList;
}

CU::GrowingArray<PointLightInstance, int> Scene::GetLightsCulled(ModelInstance* aModel, const CU::GrowingArray<PointLightInstance, int>& aLightArray) const
{
	if (aModel == nullptr)
	{
		return CU::GrowingArray<PointLightInstance, int>(2);
	}

	CU::GrowingArray<PointLightInstance, int> culledList(aLightArray);
	const CU::Vector3f modelPosition = aModel->GetPosition();
	CU::Vector3f distance;
	float rangeSqr;
	for (int lightIndex = 0; lightIndex < culledList.Size(); lightIndex++)
	{
		distance = culledList[lightIndex].GetPosition() - modelPosition;
		rangeSqr = culledList[lightIndex].GetRange();
		rangeSqr *= rangeSqr;
		if (distance.LengthSqr() > rangeSqr)
		{
			culledList.RemoveCyclicAtIndex(lightIndex--);
		}
	}
	InsertionSortForwardLights(culledList, modelPosition);

	return culledList;
}

CU::GrowingArray<ParticleEmitterInstance*, int> Scene::GetParticleEmittersCulled(RenderSettings& aSettings, const CU::Camera* aCamera) const
{
	if (aCamera == nullptr)
	{
		return myParticleEmitters;
	}

	CU::GrowingArray<ParticleEmitterInstance*, int> culledList(myParticleEmitters);
	const CU::Vector3f cameraPosition = aCamera->GetPosition();
	CU::Vector3f distance;
	const float drawDistanceSqr = aSettings.DrawDistance * aSettings.DrawDistance;
	for (int modelIndex = 0; modelIndex < culledList.Size(); modelIndex++)
	{
		distance = culledList[modelIndex]->GetPosition() - cameraPosition;
		if (distance.LengthSqr() > drawDistanceSqr)
		{
			culledList.RemoveCyclicAtIndex(modelIndex--);
		}
	}

	return culledList;
}
const CU::GrowingArray<SpriteInstance*>& Scene::GetSprites() const
{
	return mySprites;
}
CU::Camera* Scene::GetCullCamera() const
{
	return myMainCamera;
}
CU::Camera* Scene::GetViewCamera() const
{
	if (myViewCamera != nullptr)
	{
		return myViewCamera;
	}
	return myMainCamera;
}
CU::Camera* Scene::GetMapCamera() const
{
	return myMapCamera;
}
CU::Camera* Scene::GetVRCameraLeft() const
{
	return myVRCamera.GetCameraLeft();
}
CU::Camera* Scene::GetVRCameraRight() const
{
	return myVRCamera.GetCameraRight();
}
EnviromentLight* Scene::GetEnviromentLight() const
{
	return myEnviromentLight;
}
