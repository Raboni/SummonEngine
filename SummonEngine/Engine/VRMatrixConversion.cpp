#include "stdafx.h"
#include "VRMatrixConversion.h"
#include "..\OpenVR\Include\openvr.h"
#include <Matrix.hpp>

void VRMatrixToCUMatrix(const vr::HmdMatrix34_t& aVRMatrix, CU::Matrix4x4<float>& aOutCUMatrix)
{
	memcpy(&aOutCUMatrix[0], &aVRMatrix.m[0], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[4], &aVRMatrix.m[1], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[8], &aVRMatrix.m[2], sizeof(float) * 4);
	aOutCUMatrix[12] = 0.0f;
	aOutCUMatrix[13] = 0.0f;
	aOutCUMatrix[14] = 0.0f;
	aOutCUMatrix[15] = 1.0f;
	aOutCUMatrix = CU::Matrix4x4f::Transpose(aOutCUMatrix);

	aOutCUMatrix[2] *= -1;
	aOutCUMatrix[6] *= -1;
	aOutCUMatrix[8] *= -1;
	aOutCUMatrix[9] *= -1;
	aOutCUMatrix[14] *= -1;
}

void VRMatrixToCUMatrix(const vr::HmdMatrix44_t& aVRMatrix, CU::Matrix4x4<float>& aOutCUMatrix)
{
	memcpy(&aOutCUMatrix[0], &aVRMatrix.m[0], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[4], &aVRMatrix.m[1], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[8], &aVRMatrix.m[2], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[12], &aVRMatrix.m[3], sizeof(float) * 4);
	aOutCUMatrix = CU::Matrix4x4f::Transpose(aOutCUMatrix);

	aOutCUMatrix[2] *= -1;
	aOutCUMatrix[6] *= -1;
	aOutCUMatrix[8] *= -1;
	aOutCUMatrix[9] *= -1;
	aOutCUMatrix[14] *= -1;
}

void VRMatrixToCUMatrixProjection(const vr::HmdMatrix44_t& aVRMatrix, CU::Matrix4x4<float>& aOutCUMatrix)
{
	memcpy(&aOutCUMatrix[0], &aVRMatrix.m[0], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[4], &aVRMatrix.m[1], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[8], &aVRMatrix.m[2], sizeof(float) * 4);
	memcpy(&aOutCUMatrix[12], &aVRMatrix.m[3], sizeof(float) * 4);
	aOutCUMatrix = CU::Matrix4x4f::Transpose(aOutCUMatrix);

	aOutCUMatrix[8] *= -1;
	aOutCUMatrix[9] *= -1;
	aOutCUMatrix[10] *= -1;
	aOutCUMatrix[11] *= -1;
}