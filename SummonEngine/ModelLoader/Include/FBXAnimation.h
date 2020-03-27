#pragma once
#include "FBXAnimationChannel.h"
#include <GrowingArray.hpp>
#include <Matrix4x4.hpp>
#include <string>

struct aiAnimation;

class FBXAnimation
{
public:
	FBXAnimation();
	FBXAnimation(const aiAnimation* aAnim);
	~FBXAnimation() = default;

	void CalculateChannelTransform(CU::Matrix4x4f& outMatrix, const int aChannelIndex, const float aTime);
	void Reset();

	const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& GetBoneTransformLists() const;
	const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& GetBoneDeltaLists() const;

	void SetLastTime(const float aTime);
	const std::string& GetChannelName(const int aChannelIndex) const;
	int GetChannelBoneIndex(const int aChannelIndex) const;
	int GetChannelCount() const;

	float GetDuration() const;
	float GetTicksPerSecond() const;
	const std::string& GetName() const;

private:
	friend class FBXLoaderCustom;

	CU::GrowingArray<FBXLoader::FBXAnimationChannel> myChannels;
	CU::GrowingArray<unsigned int> myLastPositionKey;
	CU::GrowingArray<unsigned int> myLastRotationKey;
	CU::GrowingArray<unsigned int> myLastScalingKey;
	CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>> myBoneTransforms;
	CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>> myBoneDeltas;
	std::string myName;
	float myLastTime;
	float myTicksPerSecond;
	float myDuration;
};