#pragma once
#include "Matrix4x4.hpp"
#include "Matrix3x3.hpp"
#include "Quaternion.h"
#include "Vector3.hpp"

namespace CU
{
	typedef Quaternion<float> Quaternionf;
	class Transform
	{
	public:
		Transform();
		Transform(const Vector3f& aPosition, const Quaternionf& aRotation);
		~Transform() = default;

		bool operator==(const Transform& aTransform) const;
		bool operator!=(const Transform& aTransform) const;

		void Rotate(const Quaternionf& aRotation);
		void Rotate(const Vector3f aRotationInRadians);
		void Rotate(const Vector3f& aAxis, const float aRadianAngle);
		void Translate(const Vector3f& aTranslation);

		void SetMatrix(const Matrix4x4f& aMatrix);
		void SetRotation(const Quaternionf& aRotation);
		void SetRotation(const Vector3f& aRotationInRadians);
		void SetPosition(const Vector3f& aPosition);

		const Matrix4x4f& GetMatrix() const;
		const Matrix4x4f& GetOrientation() const;
		const Quaternionf& GetRotation() const;
		const Vector3f& GetPosition() const;
		const Vector3f& GetDirection() const;

	private:
		Matrix4x4f myMatrix;
		Matrix4x4f myOrientationMatrix;
		Quaternionf myQuaternion;
		Vector3f myPosition;
		Vector3f myDirection;
	};
}