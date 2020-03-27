#pragma once

namespace CU
{
	template<typename T>
	class Matrix4x4;
}
namespace vr
{
	struct HmdMatrix34_t;
	struct HmdMatrix44_t;
}

void VRMatrixToCUMatrix(const vr::HmdMatrix34_t& aVRMatrix, CU::Matrix4x4<float>& aOutCUMatrix);
void VRMatrixToCUMatrix(const vr::HmdMatrix44_t& aVRMatrix, CU::Matrix4x4<float>& aOutCUMatrix);
void VRMatrixToCUMatrixProjection(const vr::HmdMatrix44_t& aVRMatrix, CU::Matrix4x4<float>& aOutCUMatrix);