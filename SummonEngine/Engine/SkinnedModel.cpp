#include "stdafx.h"
#include "SkinnedModel.h"
#include "Skeleton.h"
#include "Animation.h"

CU::GrowingArray<CU::Matrix4x4f> SkinnedModel::ourEmptyTransformList;

SkinnedModel::SkinnedModel()
{
	myIsSkinned = true;
	mySkeleton = nullptr;
}

//const AnimationController& SkinnedModel::GetAnimationController() const
//{
//	return myController;
//}

CU::GrowingArray<CU::Matrix4x4f>& SkinnedModel::GetBoneTransforms(const int aAnimationIndex, const float aTime)
{
	if (mySkeleton == nullptr)
	{
		return ourEmptyTransformList;
	}

	Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return ourEmptyTransformList;
	}

	return animation->GetBoneTransformsAtTime(aTime);
}
const CU::GrowingArray<CU::Matrix4x4f>& SkinnedModel::GetBoneTransforms(const int aAnimationIndex, const float aTime) const
{
	if (mySkeleton == nullptr)
	{
		return ourEmptyTransformList;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return ourEmptyTransformList;
	}

	return animation->GetBoneTransformsAtTime(aTime);
}
const CU::GrowingArray<CU::Matrix4x4f> SkinnedModel::GetBlendedBoneTransforms(const int aAnimationIndex, const float aTime) const
{
	if (mySkeleton == nullptr)
	{
		return ourEmptyTransformList;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return ourEmptyTransformList;
	}

	float blendPercent;
	const CU::StaticArray<int, 2> blendFrameIndices(animation->GetFrameBlendIndices(blendPercent, aTime));
	if (blendFrameIndices[0] == blendFrameIndices[1])
	{
		return animation->GetBoneTransformsAtFrame(blendFrameIndices[0]);
	}

	const CU::GrowingArray<CU::Matrix4x4f>& firstBoneList(animation->GetBoneTransformsAtFrame(blendFrameIndices[0]));
	//const CU::GrowingArray<CU::Matrix4x4f>& secondBoneList(animation->GetBoneTransforms(blendFrameIndices[1]));
	/*CU::GrowingArray<CU::Matrix4x4f> blendedBoneList(firstBoneList);
	const int channelCount = animation->GetChannelCount();
	for (int channelIndex = 0; channelIndex < channelCount; channelIndex++)
	{
		const int boneIndex = animation->GetChannelBoneIndex(channelIndex);
		if (boneIndex == -1)
		{
			continue;
		}

		CU::Matrix4x4f deltaMatrix(secondBoneList[boneIndex] - firstBoneList[boneIndex]);
		for (int i = 0; i < 16; i++)
		{
			deltaMatrix[i] *= blendPercent;
		}
		blendedBoneList[boneIndex] = firstBoneList[boneIndex] + deltaMatrix;
	}*/
	/*CU::GrowingArray<CU::Matrix4x4f> blendedBoneList(firstBoneList.Size());
	blendedBoneList.Fill();
	for (int boneIndex = 0; boneIndex < blendedBoneList.Size(); boneIndex++)
	{
		CU::Matrix4x4f deltaMatrix(secondBoneList[boneIndex] - firstBoneList[boneIndex]);
		for (int i = 0; i < 16; i++)
		{
			deltaMatrix[i] *= blendPercent;
		}
		blendedBoneList[boneIndex] = firstBoneList[boneIndex] + deltaMatrix;
	}*/
	CU::GrowingArray<CU::Matrix4x4f> blendedBoneList(animation->GetBoneDeltasAtFrame(blendFrameIndices[0]));
	for (int boneIndex = 0; boneIndex < blendedBoneList.Size(); boneIndex++)
	{
		for (int i = 0; i < 16; i++)
		{
			blendedBoneList[boneIndex][i] *= blendPercent;
		}
		blendedBoneList[boneIndex] += firstBoneList[boneIndex];
	}

	return blendedBoneList;
}

CU::GrowingArray<CU::Matrix4x4f>& SkinnedModel::GetBoneTransforms(const int aAnimationIndex, const int aFrame)
{
	if (mySkeleton == nullptr)
	{
		return ourEmptyTransformList;
	}

	Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return ourEmptyTransformList;
	}

	return animation->GetBoneTransformsAtFrame(aFrame);
}
const CU::GrowingArray<CU::Matrix4x4f>& SkinnedModel::GetBoneTransforms(const int aAnimationIndex, const int aFrame) const
{
	if (mySkeleton == nullptr)
	{
		return ourEmptyTransformList;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return ourEmptyTransformList;
	}

	return animation->GetBoneTransformsAtFrame(aFrame);
}
Animation* SkinnedModel::GetAnimation(const int aAnimationIndex)
{
	if (mySkeleton == nullptr)
	{
		return nullptr;
	}

	return mySkeleton->GetAnimation(aAnimationIndex);
}
float SkinnedModel::GetAnimationDuration(const int aAnimationIndex) const
{
	if (mySkeleton == nullptr)
	{
		return 0.0f;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return 0.0f;
	}

	return animation->GetDuration();
}

int SkinnedModel::GetAnimationIndex(const std::string& aName) const
{
	if (mySkeleton == nullptr)
	{
		return -1;
	}

	return mySkeleton->GetAnimationIndex(aName);
}

int SkinnedModel::GetAnimationCount() const
{
	return mySkeleton->GetAnimationCount();
}

unsigned int SkinnedModel::GetAnimationOffset(const int aAnimationIndex) const
{
	if (mySkeleton == nullptr)
	{
		return 0;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return 0;
	}

	return animation->GetAnimationOffset();
}
unsigned int SkinnedModel::GetAnimationDeltaOffset(const int aAnimationIndex) const
{
	if (mySkeleton == nullptr)
	{
		return 0;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return 0;
	}

	return animation->GetAnimationDeltaOffset();
}
unsigned int SkinnedModel::GetFrameOffset(const int aFrame) const
{
	if (mySkeleton == nullptr)
	{
		return 0;
	}

	return aFrame * mySkeleton->GetBoneCount() * 4;
}
unsigned int SkinnedModel::GetFrameOffset(const int aAnimationIndex, const float aTime) const
{
	if (mySkeleton == nullptr)
	{
		return 0;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return 0;
	}

	return animation->GetFrameIndex(aTime) * mySkeleton->GetBoneCount() * 4;
}
unsigned int SkinnedModel::GetFrameOffset(const int aAnimationIndex, const float aTime, float& outBlendPercent) const
{
	if (mySkeleton == nullptr)
	{
		return 0;
	}

	const Animation* animation = mySkeleton->GetAnimation(aAnimationIndex);
	if (animation == nullptr)
	{
		return 0;
	}

	const CU::StaticArray<int, 2> indices(animation->GetFrameBlendIndices(outBlendPercent, aTime));
	return indices[0] * mySkeleton->GetBoneCount() * 4;
}

int SkinnedModel::GetSkeletonBoneCount() const
{
	if (mySkeleton == nullptr)
	{
		return 0;
	}

	return mySkeleton->GetBoneCount();
}