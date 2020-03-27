#pragma once
#include "VertexStructs.h"
#include "FBXSkeleton.h"
#include "FBXAnimation.h"

#include <GrowingArray.hpp>
#include <Matrix4x4.hpp>

#include <string>
#include <vector>
#include <map>

using namespace FBXLoader;
struct aiScene;
struct aiMesh;
struct aiMaterial;
struct aiNode;

struct BoneInfo
{
	Matrix44f BoneOffset;
	Matrix44f FinalTransformation;
};

// One model can contain multiple meshes
class FBXMesh
{
public:
	FBXMesh() {myShaderType = 0; myVerticies = nullptr; myVertexBufferSize = 0; myVertexCount = 0; myModel = nullptr; }
	~FBXMesh(){}
	std::vector<unsigned int> myIndexes;
	std::vector<FBXMesh*> myChildren;
	class LoaderModel* myModel;
	char* myVerticies;
	unsigned int myShaderType;
	unsigned int myVertexBufferSize;
	int myVertexCount;
};

class LoaderModel
{
public:
	LoaderModel() { myIsLoaded = false; myAnimationDuration = 0.0f; }
	~LoaderModel(){}
	void SetData(const char* aModelPath) { myModelPath = aModelPath; }
	FBXMesh* CreateMesh()
	{
		FBXMesh* model = new FBXMesh();
		myMeshes.push_back(model);
		model->myModel = this;
		return model;
	}

	Matrix44f myGlobalInverseTransform;
	std::map<std::string, unsigned int> myBoneNameToIndex;
	std::vector<BoneInfo> myBoneInfo;
	std::vector<FBXMesh*> myMeshes;
	std::vector<std::string> myTextures;
	std::string myModelPath;
	const aiScene* myScene;
	unsigned int myNumBones;
	float myAnimationDuration;
	bool myIsLoaded;
};


class FBXLoaderCustom
{
public:
	FBXLoaderCustom();
	~FBXLoaderCustom() = default;

	LoaderModel* LoadModel(const std::string& aPath);
	void LoadSkeleton(FBXSkeleton& outSkeleton, CU::GrowingArray<FBXAnimation>& outAnimationList, const std::string& aPath);
	void LoadAnimations(CU::GrowingArray<FBXAnimation>& outAnimationList, FBXSkeleton& aSkeleton, const std::string& aPath);

private:
	void* LoadModelInternal(LoaderModel* aModel);
	int DetermineAndLoadVerticies(aiMesh* aMesh, FBXMesh* aLoaderMesh);
	void LoadMaterials(const aiScene* aScene, LoaderModel* aModel);
	void LoadTexture(const int aType, std::vector<std::string>& aTextureList, aiMaterial* aMaterial);
	void LoadAnimations(CU::GrowingArray<FBXAnimation>& outAnimationList, FBXSkeleton& aSkeleton, const aiScene* aScene);

	FBXBone* CreateBoneTree(FBXSkeleton& outSkeleton, const aiNode* aNode, FBXBone* aParent);
	void InitBones(FBXSkeleton& outSkeleton, const aiScene* aScene);
	void UpdateAnimationTransforms(FBXAnimation& outAnimation, FBXSkeleton& aSkeleton, const float aTime);
};