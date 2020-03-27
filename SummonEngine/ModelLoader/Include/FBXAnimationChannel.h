#pragma once
#include <GrowingArray.hpp>
#include <Matrix4x4.hpp>
#include <Vector3.hpp>
#include <Quaternion.h>
#include <string>

namespace FBXLoader
{
	struct VectorKey
	{
		double myTime;
		CU::Vector3f myVector;
	};
	struct QuatKey
	{
		double myTime;
		CU::Quaternionf myQuat;
	};

	class FBXAnimationChannel
	{
	public:
		CU::GrowingArray<VectorKey, unsigned int> myPositionKeys;
		CU::GrowingArray<QuatKey, unsigned int> myRotationKeys;
		CU::GrowingArray<VectorKey, unsigned int> myScalingKeys;
		std::string myName;
		int myBoneIndex = -1;
	};
}