#pragma once
#include <GrowingArray.hpp>
#include <StaticArray.hpp>
#include <Matrix4x4.hpp>
#include <string>

class Animation
{
public:
	Animation();
	~Animation() = default;

	void SetAnimationOffset(const unsigned int aOffset);
	void SetAnimationSize(const unsigned int aSize);
	void SetAnimationDeltaOffset(const unsigned int aOffset);
	void SetAnimationDeltaSize(const unsigned int aSize);
	unsigned int GetAnimationOffset() const;
	unsigned int GetAnimationSize() const;
	unsigned int GetAnimationDeltaOffset() const;
	unsigned int GetAnimationDeltaSize() const;

	const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& GetBoneTransformLists() const;
	CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransformsAtTime(const float aTime);
	const CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransformsAtTime(const float aTime) const;
	CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransformsAtFrame(const int aFrame);
	const CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransformsAtFrame(const int aFrame) const;

	const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& GetBoneDeltaLists() const;
	const CU::GrowingArray<CU::Matrix4x4f>& GetBoneDeltasAtFrame(const int aFrame) const;

	int GetFrameIndex(const float aTime) const;
	CU::StaticArray<int, 2> GetFrameBlendIndices(float& outBlendPercent, const float aTime) const;
	int GetFrameCount() const;

	float GetDuration() const;
	float GetTicksPerSecond() const;
	const std::string& GetName() const;

private:
	friend class ModelLoader;

	CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>> myBoneTransforms;
	CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>> myBoneDeltas;
	std::string myName;
	float myTicksPerSecond;
	float myDuration;
	unsigned int myAnimationOffset;
	unsigned int myAnimationSize;
	unsigned int myAnimationDeltaOffset;
	unsigned int myAnimationDeltaSize;
	bool myPlayAnimationForward;
};