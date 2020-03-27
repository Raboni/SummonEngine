#include "stdafx.h"
#include "VRCamera.h"
#include "Camera.hpp"

VRCamera::VRCamera()
{
	myCameraLeft = nullptr;
	myCameraRight = nullptr;
}
VRCamera::VRCamera(CU::Camera* aLeftCamera, CU::Camera* aRightCamera)
{
	myCameraLeft = aLeftCamera;
	myCameraRight = aRightCamera;
}
VRCamera::~VRCamera()
{
}

void VRCamera::SetCameraLeft(CU::Camera* aCamera)
{
	myCameraLeft = aCamera;
}
void VRCamera::SetCameraRight(CU::Camera* aCamera)
{
	myCameraRight = aCamera;
}

void VRCamera::SetOffsetLeft(const CU::Vector3f& aOffset)
{
	myOffsetLeft = aOffset;
}
void VRCamera::SetOffsetRight(const CU::Vector3f& aOffset)
{
	myOffsetRight = aOffset;
}

void VRCamera::SetProjectionMatrixLeft(const CU::Matrix4x4f& aProjection)
{
	myCameraLeft->SetProjectionMatrix(aProjection);
}
void VRCamera::SetProjectionMatrixRight(const CU::Matrix4x4f& aProjection)
{
	myCameraRight->SetProjectionMatrix(aProjection);
}

void VRCamera::SetTransformMatrix(const CU::Matrix4x4f& aTransform)
{
	CU::Matrix4x4f fixedTransform;
	CU::Vector3f eyePos;

	memcpy(&fixedTransform, &aTransform, sizeof(CU::Matrix4x4f));
	myPosition = CU::Vector3f(fixedTransform[12], fixedTransform[13], fixedTransform[14]);
	eyePos = myPosition + CU::Vector3f(CU::Vector4f(myOffsetLeft, 0.0f) * fixedTransform);
	fixedTransform[12] += eyePos.x;
	fixedTransform[13] += eyePos.y;
	fixedTransform[14] += eyePos.z;
	myCameraLeft->SetTransformMatrix(fixedTransform);

	memcpy(&fixedTransform, &aTransform, sizeof(CU::Matrix4x4f));
	eyePos = myPosition + CU::Vector3f(CU::Vector4f(myOffsetRight, 0.0f) * fixedTransform);
	fixedTransform[12] += eyePos.x;
	fixedTransform[13] += eyePos.y;
	fixedTransform[14] += eyePos.z;
	myCameraRight->SetTransformMatrix(fixedTransform);
}

const CU::Vector3f& VRCamera::GetPosition() const
{
	return myPosition;
}

CU::Camera* VRCamera::GetCameraLeft() const
{
	return myCameraLeft;
}
CU::Camera* VRCamera::GetCameraRight() const
{
	return myCameraRight;
}

bool VRCamera::IsValid() const
{
	return myCameraLeft != nullptr && myCameraRight != nullptr;
}