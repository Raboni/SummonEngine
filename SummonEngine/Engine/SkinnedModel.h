#pragma once
#include "Model.h"
#include <GrowingArray.hpp>
#include <Matrix4x4.hpp>

class Skeleton;
class Animation;

class SkinnedModel : public Model
{
public:
	CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransforms(const int aAnimationIndex, const float aTime);
	const CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransforms(const int aAnimationIndex, const float aTime) const;
	const CU::GrowingArray<CU::Matrix4x4f> GetBlendedBoneTransforms(const int aAnimationIndex, const float aTime) const;
	CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransforms(const int aAnimationIndex, const int aFrame);
	const CU::GrowingArray<CU::Matrix4x4f>& GetBoneTransforms(const int aAnimationIndex, const int aFrame) const;

	Animation* GetAnimation(const int aAnimationIndex);
	float GetAnimationDuration(const int aAnimationIndex) const;
	int GetAnimationIndex(const std::string& aName) const;
	int GetAnimationCount() const;

	unsigned int GetAnimationOffset(const int aAnimationIndex) const;
	unsigned int GetAnimationDeltaOffset(const int aAnimationIndex) const;
	unsigned int GetFrameOffset(const int aFrame) const;
	unsigned int GetFrameOffset(const int aAnimationIndex, const float aTime) const;
	unsigned int GetFrameOffset(const int aAnimationIndex, const float aTime, float& outBlendPercent) const;
	int GetSkeletonBoneCount() const;

protected:
	friend class Engine;
	friend class Scene;
	friend class ModelLoader;
	friend class ModelInstance;
	friend struct ModelRenderCommand;
	friend class CU::HashMap<std::string, SkinnedModel>;

	SkinnedModel();
	~SkinnedModel() = default;

protected:
	static CU::GrowingArray<CU::Matrix4x4f> ourEmptyTransformList;
	Skeleton* mySkeleton;
};