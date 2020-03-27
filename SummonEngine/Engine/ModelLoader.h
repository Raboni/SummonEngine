#pragma once
#include "ModelInstance.h"
#include "Model.h"
#include "SkinnedModel.h"
#include "CustomModel.h"
#include "Skeleton.h"
#include "Animation.h"
#include "FBXLoaderCustom.h"

#include <GrowingArray.hpp>
#include <GrowingQueue.hpp>
#include <HashMap.hpp>
#include <SizableArray.h>

#include <thread>
#include <atomic>

class FBXMesh;
class FBXAnimation;
struct ID3D11Device;

constexpr int MaxAnimationCount = 64;

struct LoadCommand
{
	LoadCommand()
	{
		myModel = nullptr;
		myIsAnimation = false;
		myIsCustom = false;
	}
	LoadCommand(const std::string& aPath, Model** aModel, const bool aIsAnimation = false, const bool aIsCustom = false)
	{
		myPath = aPath;
		myModel = aModel;
		myIsAnimation = aIsAnimation;
		myIsCustom = aIsCustom;
	}
	LoadCommand(const std::string& aPath, const std::string& aSkeletonPath, Model** aModel)
	{
		myPath = aPath;
		mySkeletonPath = aSkeletonPath;
		myModel = aModel;
		myIsAnimation = false;
		myIsCustom = false;
	}

	std::string myPath;
	std::string mySkeletonPath;
	Model** myModel;
	bool myIsAnimation;
	bool myIsCustom;
};

class ModelLoader
{
public:
	static void CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxModelAmount);
	static ModelLoader* GetInstance();
	static void DestroyInstance();

	Model* GetModel(const std::string& aPath, Model** aModel);
	SkinnedModel* GetSkinnedModel(const std::string& aPath, const std::string& aSkeletonPath, SkinnedModel** aModel);
	CustomModel* GetCustomModel(const std::string& aName, CustomModel** aModel);
	void AddAnimations(const std::string& aPath, SkinnedModel** aModel);
	const CU::SizableArray<Animation, MaxAnimationCount>& GetAnimations() const;
	void UpdateCustomMeshes();
	void UnloadModel(const std::string& aPath);
	void UnloadModel(const std::string& aPath, const std::string& aSkeletonPath);
	bool IsLoading() const;

private:
	enum class CubeType
	{
		Normal,
		Debug,
		Skybox
	};

private:
	ModelLoader(ID3D11Device* aDevice, const unsigned int aMaxModelAmount);
	~ModelLoader();
	void LoadThreaded();
	Model* LoadCube(const CubeType aType = CubeType::Normal);
	Model* LoadPlane();
	void LoadModel(const std::string& aPath, const std::string& aSkeletonPath, Model** aOutModel, const bool aCustom = false);
	void LoadAnimations(const std::string& aPath, SkinnedModel* aModel);
	void LoadAnimations(const std::string& aPath, Skeleton* aSkeleton);
	void LoadAnimations(const CU::GrowingArray<FBXAnimation>& aAnimationArray, Skeleton* aSkeleton);
	Animation& CreateAnimationEntry(const FBXAnimation& aAnimation);
	float CalculateEncapsulationRange(const FBXMesh* aMesh) const;
	float CalculateEncapsulationRange(const CustomMesh* aMesh) const;

private:
	static ModelLoader* ourInstance;

	FBXLoaderCustom myFBXLoader;
	CU::HashMap<std::string, Model> myModelMap;
	CU::HashMap<std::string, SkinnedModel> mySkinnedModelMap;
	CU::HashMap<std::string, CustomModel> myCustomModelMap;
	CU::HashMap<std::string, Skeleton> mySkeletonMap;
	CU::SizableArray<Animation, MaxAnimationCount> myAnimationList;
	CU::GrowingQueue<LoadCommand> myLoadCommands;
	std::thread myLoadingThread;
	ID3D11Device* myDevice;
	std::atomic<bool> myIsLoading;
	bool myStopThread;
};