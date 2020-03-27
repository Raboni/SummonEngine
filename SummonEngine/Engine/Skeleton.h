#pragma once
#include "FBXSkeleton.h"
#include <HashMap.hpp>
#include <SizableArray.h>
#include <string>

class Animation;

class Skeleton
{
public:
	Skeleton();
	~Skeleton() = default;

	int GetAnimationIndex(const std::string& aName) const;
	Animation* GetAnimation(const std::string& aName) const;
	Animation* GetAnimation(const int aIndex) const;

	int GetBoneCount() const;
	int GetAnimationCount() const;

private:
	friend class ModelLoader;

	std::string myPath;
	FBXSkeleton mySkeleton;
	CU::HashMap<std::string, int> myAnimationIndexMap;
	CU::SizableArray<Animation*, 32> myAnimations;
	CU::SizableArray<size_t, 32> myLoadedAnimationPathHashList;
};