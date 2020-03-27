#pragma once
#include "GrowingArray.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "PlaneVolume.hpp"
#include "Intersection.hpp"

namespace CU
{
	class Camera
	{
	public:
		Camera();
		~Camera();
		Camera& operator=(const Camera& aCamera);

		const Matrix4x4f& GetSpace() const;
		const Matrix4x4f& GetProjectionMatrix() const;
		const Matrix4x4f GetViewMatrix() const;

		void Move(const Vector3f& aMovement);
		void Rotate(const Vector3f& aRotationInRadians);

		void SetProjectionMatrix(const Matrix4x4f& aProjection);
		void SetTransformMatrix(const Matrix4x4f& aTransform);
		void SetPosition(const Vector3f& aPosition);
		void SetRotation(const Vector3f& aRotationInRadians);
		void SetFoV(const float aRadianFoV);
		void SetAspectRatio(const float aAspectRatio);
		void SetNearPlane(const float aNearPlane);
		void SetFarPlane(const float aFarPlane);
		void SetOrthographic(const bool aUseOrthographic);

		virtual Vector4f ProjectToScreen(const Vector4f& aVector);
		virtual GrowingArray<Vector4f> ProjectArrayToScreen(const GrowingArray<Vector4f>& aArray);
		const Vector3f& GetPosition() const;
		const Vector3f& GetDirection() const;

		CU::Intersection::LineRay3D GetRay(float aRange = 0.0f, const CU::Vector2f& aOffset = CU::Vector2f::Zero);
		bool InsideFrustum(const Vector3f& aPoint) const;
		bool InsideFrustum(Intersection::Sphere aSphere) const;

	protected:
		void UpdateProjection();

		Transform myTransform;
		Matrix4x4f myProjectionMatrix;
		Matrix4x4f myProjectionMatrixInverse;
		PlaneVolume myFrustum;

		float myFoV;
		float myAspectRatio;
		float myNear;
		float myFar;
		bool myUseOrthographic;
	};
}