#include "stdafx.h"
#include "Skeleton.h"
#include "ModelLoader.h"
#include <GrowingArray.hpp>

Skeleton::Skeleton() : myAnimationIndexMap(32)
{
}

int Skeleton::GetAnimationIndex(const std::string& aName) const
{
	const int* indexPtr = myAnimationIndexMap.Get(aName);
	if (indexPtr == nullptr)
	{
		return -1;
	}

	return *indexPtr;
}

Animation* Skeleton::GetAnimation(const std::string& aName) const
{
	const int* indexPtr = myAnimationIndexMap.Get(aName);
	if (indexPtr == nullptr)
	{
		return nullptr;
	}

	return myAnimations[*indexPtr];
}
Animation* Skeleton::GetAnimation(const int aIndex) const
{
	if (aIndex < 0 || aIndex >= myAnimations.Size())
	{
		return nullptr;
	}

	return myAnimations[aIndex];
}

int Skeleton::GetBoneCount() const
{
	return mySkeleton.GetBoneCount();
}

int Skeleton::GetAnimationCount() const
{
	return myAnimations.Size();
}