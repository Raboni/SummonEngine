#pragma once
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Quaternion.h"

namespace CU
{

	namespace Intersection
	{
		struct Sphere
		{
			Sphere(const Vector3f& aCenter, const float aRadius)
				: center(aCenter)
				, radius(aRadius)
			{}

			Vector3f center;
			float radius;
		};

		struct Circle
		{
			Circle(const Vector2f& aCenter, const float aRadius)
				: center(aCenter)
				, radius(aRadius)
			{}

			Vector2f center;
			float radius;
		};

		struct AABB3D
		{
			AABB3D(const Vector3f& aMin, const Vector3f& aMax)
				: min(aMin)
				, max(aMax)
			{}

			Vector3f min;
			Vector3f max;
		};

		struct AABB2D
		{
			AABB2D(const Vector2f& aPosition, const Vector2f& aSize)
			{
				myPosition = aPosition;
				myHalfSize = aSize * 0.5f;
			}

			Vector2f myPosition;
			Vector2f myHalfSize;
		};

		struct Line3D
		{
			Line3D(const Vector3f& aPoint, const Vector3f& aDir)
			{
				point = aPoint;
				direction = aDir;
			}

			Vector3f point;
			Vector3f direction;
		};
		struct LineRay3D
		{
			LineRay3D(const Vector3f& aStart, const Vector3f& aEnd)
			{
				point = aStart;
				direction = aEnd - aStart;
				range = direction.Length();
				direction.Normalize();
			}
			LineRay3D(const Vector3f& aPoint, const Vector3f& aDirection, const float aRange)
			{
				point = aPoint;
				direction = aDirection;
				range = aRange;
			}

			Vector3f point;
			Vector3f direction;
			float range;
		};

		struct Line2D
		{
			Line2D(const Vector2f& aPoint, const Vector2f& aDir)
				: point(aPoint)
				, direction(aDir)
			{
			}

			Vector2f point;
			Vector2f direction;
		};
		struct LineRay2D
		{
			LineRay2D(const Vector2f& aStart, const Vector2f& aEnd)
				: start(aStart)
				, end(aEnd)
			{
			}

			Vector2f start;
			Vector2f end;
		};

		struct Plane
		{
			Plane(const Vector3f& aPoint, const Vector3f& aNormal)
			{
				point = aPoint;
				normal = aNormal;
			}
			Plane(const Vector3f& aPosition, const Vector3f& aPoint2, const Vector3f& aPoint3)
			{
				point = aPosition;
				normal = (aPoint2 - aPosition).Cross(aPoint3 - aPosition);
				normal.Normalize();
			}

			Vector3f point;
			Vector3f normal;
		};

		bool PointInsideSphere(const Sphere& aSphere, const Vector3f aPoint);
		bool PointInsideSphere(const Circle& aCircle, const Vector2f aPoint);
		bool PointInsideAABB(const AABB3D& aAABB, const Vector3f aPoint);
		bool PointInsideAABB(const AABB2D& aAABB, const Vector2f aPoint);
		bool IntersectionPlaneLine(const Plane& aPlane, const Line3D& aLine, Vector3f& aOutIntersectionPoint);
		bool IntersectionPlaneLine(const Plane& aPlane, const LineRay3D& aLine, Vector3f& aOutIntersectionPoint);
		bool IntersectionAABBLine(const AABB3D& aAABB, const LineRay3D& aLine);
		bool IntersectionAABBLine(const AABB3D& aAABB, const Line3D& aLine);
		bool IntersectionAABBLineRotated(const Vector3f& aAABBPoint, const Vector3f& aAABBHalfSize, const LineRay3D& aLine, const Quaternionf& aRotation);
		bool IntersectionSphereLine(const Sphere& aSphere, const Line3D& aLine);
		bool IntersectionSphereLine(const Sphere& aSphere, const LineRay3D& aLine);
		bool IntersectionSpherePlane(const Sphere& aSphere, const Plane& aPlane);
		bool IntersectionAABBLine(const AABB2D& aAABB, const LineRay2D& aLine);
		bool IntersectionAABBLine(const AABB2D& aAABB, const Line2D& aLine);
		bool IntersectionSphereLine(const Circle& aCircle, const Line2D& aLine);
		bool IntersectionSphereLine(const Circle& aCircle, const LineRay2D& aLine);
		bool IntersectionAABBSphere(const AABB2D& aAABB, const Circle& aCircle);
		bool IntersectionAABBSphere(const AABB2D& aAABB, const Circle& aCircle, Vector2f& aOutIntersectionPoint);
		bool IntersectionAABBAABB(const AABB2D& aAABB0, const AABB2D& aAABB1);
		bool IntersectionSphereSphere(const Sphere& aSphere0, const Sphere& aSphere1);
		bool IntersectionSphereSphere(const Circle& aCircle0, const Circle& aCircle1);
	};
}

