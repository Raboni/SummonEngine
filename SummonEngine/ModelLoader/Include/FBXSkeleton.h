#pragma once
#include <GrowingArray.hpp>
#include <SizableArray.h>
#include <HashMap.hpp>
#include <Matrix4x4.hpp>
#include <string>

class FBXBone
{
public:
	FBXBone();

	std::string myName;
	CU::Matrix4x4f myOffset;
	CU::Matrix4x4f myLocalTransform;
	CU::Matrix4x4f myGlobalTransform;
	CU::SizableArray<FBXBone*, 16> myChildren;
	FBXBone* myParent;
};

class FBXSkeleton
{
public:
	FBXSkeleton();
	~FBXSkeleton() = default;

	void Update();

	FBXBone* GetBone(const std::string& aName) const;
	int GetBoneCount() const;

private:
	friend class FBXLoaderCustom;

	static void CalculateBoneToWorldTransform(FBXBone* aChild);

	void UpdateBone(FBXBone* aBone);

private:
	CU::HashMap<std::string, FBXBone*> myBonesByName;
	CU::HashMap<std::string, unsigned int> myBonesToIndex;
	CU::GrowingArray<FBXBone> myBones;
	CU::GrowingArray<FBXBone*> myMeshBones;
	FBXBone* myRoot;
};