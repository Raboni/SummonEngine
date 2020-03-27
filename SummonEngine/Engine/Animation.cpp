#include "stdafx.h"
#include "Animation.h"

Animation::Animation() : myBoneTransforms(16), myBoneDeltas(16)
{
	myTicksPerSecond = 0.0f;
	myDuration = 0.0f;
	myAnimationOffset = 0;
	myAnimationSize = 0;
	myPlayAnimationForward = true;
}

void Animation::SetAnimationOffset(const unsigned int aOffset)
{
	myAnimationOffset = aOffset;
}
void Animation::SetAnimationSize(const unsigned int aSize)
{
	myAnimationSize = aSize;
}
void Animation::SetAnimationDeltaOffset(const unsigned int aOffset)
{
	myAnimationDeltaOffset = aOffset;
}
void Animation::SetAnimationDeltaSize(const unsigned int aSize)
{
	myAnimationDeltaSize = aSize;
}
unsigned int Animation::GetAnimationOffset() const
{
	return myAnimationOffset;
}
unsigned int Animation::GetAnimationSize() const
{
	return myAnimationSize;
}
unsigned int Animation::GetAnimationDeltaOffset() const
{
	return myAnimationDeltaOffset;
}
unsigned int Animation::GetAnimationDeltaSize() const
{
	return myAnimationDeltaSize;
}

const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& Animation::GetBoneTransformLists() const
{
	return myBoneTransforms;
}
CU::GrowingArray<CU::Matrix4x4f>& Animation::GetBoneTransformsAtTime(const float aTime)
{
	return myBoneTransforms[GetFrameIndex(aTime)];
}
const CU::GrowingArray<CU::Matrix4x4f>& Animation::GetBoneTransformsAtTime(const float aTime) const
{
	return myBoneTransforms[GetFrameIndex(aTime)];
}
CU::GrowingArray<CU::Matrix4x4f>& Animation::GetBoneTransformsAtFrame(const int aFrame)
{
	return myBoneTransforms[aFrame];
}
const CU::GrowingArray<CU::Matrix4x4f>& Animation::GetBoneTransformsAtFrame(const int aFrame) const
{
	return myBoneTransforms[aFrame];
}
const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& Animation::GetBoneDeltaLists() const
{
	return myBoneDeltas;
}
const CU::GrowingArray<CU::Matrix4x4f>& Animation::GetBoneDeltasAtFrame(const int aFrame) const
{
	return myBoneDeltas[aFrame];
}

int Animation::GetFrameIndex(const float aTime) const
{
	const float pTime = aTime * myTicksPerSecond;

	float time = 0.0f;
	if (myDuration > 0.0f)
	{
		time = fmod(pTime, myDuration);
	}

	float percent = time / myDuration;
	if (!myPlayAnimationForward)
	{
		percent = (percent - 1.0f) * -1.0f;
	}
	return static_cast<int>(static_cast<float>(myBoneTransforms.Size() - 1) * percent);
}
CU::StaticArray<int, 2> Animation::GetFrameBlendIndices(float& outBlendPercent, const float aTime) const
{
	float ptime = aTime * myTicksPerSecond;

	float time = 0.0f;
	if (myDuration > 0.0f)
	{
		time = fmod(ptime, myDuration);
	}

	float animationPercent = time / myDuration;
	if (!myPlayAnimationForward)
	{
		animationPercent = (animationPercent - 1.0f) * -1.0f;// this will invert the percent so the animation plays backwards
	}

	const float floatIndex = static_cast<float>(myBoneTransforms.Size() - 1) * animationPercent;
	const int firstIndex = static_cast<int>(floatIndex);
	const float blendPercent = floatIndex - static_cast<float>(firstIndex);
	CU::StaticArray<int, 2> frameIndices;
	frameIndices[0] = firstIndex;
	if (blendPercent <= 0.0f || firstIndex >= myBoneTransforms.Size() - 1)
	{
		frameIndices[1] = firstIndex;
		outBlendPercent = 0.0f;
		return frameIndices;
	}

	frameIndices[1] = firstIndex + 1;
	outBlendPercent = blendPercent;
	return frameIndices;
}
int Animation::GetFrameCount() const
{
	return myBoneTransforms.Size();
}

float Animation::GetDuration() const
{
	return myDuration;
}
float Animation::GetTicksPerSecond() const
{
	return myTicksPerSecond;
}

const std::string& Animation::GetName() const
{
	return myName;
}