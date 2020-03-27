#pragma once
#include <GrowingArray.hpp>
#include <Matrix.hpp>

constexpr unsigned int PointLightListSize = 8;

struct FrameBufferData
{
	CU::Matrix4x4f myToCameraMatrix;
	CU::Matrix4x4f myToProjectionMatrix;
	CU::Vector4f myCameraPosition;
};
struct ObjectBufferData
{
	CU::Matrix4x4f myToWorldMatrix;
	CU::Vector4f myScale;
	CU::Vector4f myColor;
};
struct AnimObjectBufferData
{
	CU::Matrix4x4f myToWorldMatrix;
	CU::Vector4f myScale;
	CU::Vector4f myColor;
	unsigned int myAnimationOffset;
	unsigned int myAnimationDeltaOffset;
	unsigned int myFrameOffset;
	float myBlendPercent;
};
struct EnviromentLightBufferData
{
	CU::Vector4f myDirectionalLightDirection;
	CU::Vector4f myDirectionalLightColor;
	unsigned int myEnviromentLightMipCount;
	unsigned int reserved[3];
};
struct PointLightBufferData
{
	CU::Vector4f myPosition;
	CU::Vector3f myColor;
	float myRange;
};
struct PointLightListBufferData
{
	PointLightBufferData myPointLights[PointLightListSize];
	unsigned int myCount;
	unsigned int reserved[3];
};
struct BoneBufferData
{
	CU::Matrix4x4f myBoneTransforms[100];
};