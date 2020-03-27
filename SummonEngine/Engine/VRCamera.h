#pragma once
#include <Matrix.hpp>
#include <Vector.hpp>

namespace CU
{
	class Camera;
}

class VRCamera
{
public:
	VRCamera();
	VRCamera(CU::Camera* aLeftCamera, CU::Camera* aRightCamera);
	~VRCamera();

	void SetCameraLeft(CU::Camera* aCamera);
	void SetCameraRight(CU::Camera* aCamera);
	void SetOffsetLeft(const CU::Vector3f& aOffset);
	void SetOffsetRight(const CU::Vector3f& aOffset);
	void SetProjectionMatrixLeft(const CU::Matrix4x4f& aProjection);
	void SetProjectionMatrixRight(const CU::Matrix4x4f& aProjection);
	void SetTransformMatrix(const CU::Matrix4x4f& aTransform);

	const CU::Vector3f& GetPosition() const;
	CU::Camera* GetCameraLeft() const;
	CU::Camera* GetCameraRight() const;
	bool IsValid() const;

private:
	CU::Vector3f myOffsetLeft;
	CU::Vector3f myOffsetRight;
	CU::Vector3f myPosition;
	CU::Camera* myCameraLeft;
	CU::Camera* myCameraRight;
};