#pragma once
#include "Vector3.hpp"

namespace CU
{
	class Plane
	{
	public:
		Plane();
		~Plane() = default;
		Plane(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2);
		Plane(const Vector3f& aPoint0, const Vector3f& aNormal);
		void InitWith3Points(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2);
		void InitWithPointAndNormal(const Vector3f& aPoint, const Vector3f& aNormal);
		bool Inside(const Vector3f& aPosition) const;

	private:
		friend class PlaneVolume;

		Vector3f myPoint;
		Vector3f myNormal;
	};
}