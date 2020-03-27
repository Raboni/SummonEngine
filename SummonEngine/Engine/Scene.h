#pragma once
#include "RenderSettings.h"
#include "PointLightInstance.h"
#include "ModelInstance.h"
#include "MapRenderCommand.h"
#include "Navmesh.h"
#include "VRCamera.h"
#include <GrowingArray.hpp>
#include <Camera.hpp>
#include <atomic>

class ParticleEmitterInstance;
class SpriteInstance;
class EnviromentLight;

class Scene
{
public:
	Scene();
	Scene(const Scene& aScene);
	~Scene();
	Scene& operator=(const Scene& aScene);

	void WaitForLoading();
	void AddInstance(ModelInstance* aModel);
	void AddInstance(PointLightInstance* aLight);
	void AddInstance(ParticleEmitterInstance* aParticleEmitter);
	void AddInstance(SpriteInstance* aSprite);
	void AddInstance(Nav::Navmesh* aNavmesh);
	void RemoveInstance(ModelInstance* aModel);
	void RemoveInstance(PointLightInstance* aLight);
	void RemoveInstance(ParticleEmitterInstance* aParticleEmitter);
	void RemoveInstance(SpriteInstance* aSprite);
	void RemoveInstance(Nav::Navmesh* aNavmesh);
	void SetEnviromentLight(EnviromentLight* aLight);
	void SetMainCamera(CU::Camera* aCamera);
	void SetViewCamera(CU::Camera* aCamera);
	void SetVRCameraLeft(CU::Camera* aCamera);
	void SetVRCameraRight(CU::Camera* aCamera);
	void SetMap(SpriteInstance* aSprite);
	void SetMap(CU::Camera* aCamera);
	void SetMapPositionScreen(const CU::Vector2f& aPosition);
	void SetMapSizeScreen(const CU::Vector2f& aSize);
	void SetMapPositionWorld(const CU::Vector2f& aPosition);
	void SetMapSizeWorld(const CU::Vector2f& aSize);
	void SetMapFocus(const CU::Vector2f& aFocus);
	void SetMapRotation(const float aRotation);
	void SetMapZoom(const float aZoom);

	CU::GrowingArray<ModelInstance*, int> GetModelsCulled(RenderSettings& aSettings, const CU::Camera* aCamera, const bool aUseDeferred = true) const;
	CU::GrowingArray<ModelInstance*, int> GetModelsCulledMap(RenderSettings& aSettings, const CU::Camera* aCamera, const float aMapRange) const;
	const CU::GrowingArray<ModelInstance*>& GetModels() const;
	CU::GrowingArray<PointLightInstance*, int> GetLightsCulled(RenderSettings& aSettings, const CU::Camera* aCamera) const;
	CU::GrowingArray<PointLightInstance*, int> GetLightsCulled(ModelInstance* aModel) const;
	CU::GrowingArray<PointLightInstance, int> GetLightsCulled(ModelInstance* aModel, const CU::GrowingArray<PointLightInstance>& aLightArray) const;
	CU::GrowingArray<ParticleEmitterInstance*, int> GetParticleEmittersCulled(RenderSettings& aSettings, const CU::Camera* aCamera) const;
	const CU::GrowingArray<SpriteInstance*>& GetSprites() const;
	CU::Camera* GetCullCamera() const;
	CU::Camera* GetViewCamera() const;
	CU::Camera* GetMapCamera() const;
	CU::Camera* GetVRCameraLeft() const;
	CU::Camera* GetVRCameraRight() const;
	EnviromentLight* GetEnviromentLight() const;

private:
	friend class Engine;

	MapRenderCommand myMap;
	ModelInstance mySkybox;
	VRCamera myVRCamera;
	CU::GrowingArray<ModelInstance*> myModels;
	CU::GrowingArray<PointLightInstance*> myLights;
	CU::GrowingArray<ParticleEmitterInstance*> myParticleEmitters;
	CU::GrowingArray<SpriteInstance*> mySprites;
	CU::GrowingArray<Nav::Navmesh*> myNavmeshes;
	CU::Camera* myMainCamera;
	CU::Camera* myViewCamera;
	CU::Camera* myMapCamera;
	EnviromentLight* myEnviromentLight;
};