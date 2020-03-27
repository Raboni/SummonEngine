#pragma once
#include "Plane.hpp"
#include "GrowingArray.hpp"
#include "Intersection.hpp"
#include <vector>

namespace CU
{
	class PlaneVolume
	{
	public:
		PlaneVolume();
		~PlaneVolume();
		PlaneVolume(const std::vector<Plane>& aPlaneList);
		PlaneVolume(const GrowingArray<Plane>& aPlaneList);
		Plane& operator[](unsigned short aIndex);
		void AddPlane(const Plane& aPlane);
		bool Inside(const Vector3f& aPosition) const;
		bool Inside(const Intersection::Sphere& aSphere) const;

	private:
		GrowingArray<Plane> myPlanes;
	};
}