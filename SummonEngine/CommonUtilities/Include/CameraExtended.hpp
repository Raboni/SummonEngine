#pragma once
#include "Camera.hpp"
#include "CameraObject.hpp"

namespace CU
{
	struct Light
	{
		Light()
		{
			myColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
			myAmbience = Vector4f(0.1f, 0.1f, 0.1f, 1.0f);
		}
		Light(const Vector3f& aPosition, const Vector4f& aColor, const Vector4f& aAmbience)
		{
			myPosition = aPosition;
			myColor = aColor;
			myAmbience = aAmbience;
		}

		Vector3f myPosition;
		Vector4f myColor;
		Vector4f myAmbience;
	};

	class CameraExtended : public Camera
	{
	public:
		CameraExtended();
		~CameraExtended() = default;

		void AddLight(const Light& aLight);
		GrowingArray<Light>& GetLightArray();

		GrowingArray<Point> ProjectObjectListToScreen(GrowingArray<CameraObject>& aObjectList, bool aUseLight = false);
		void ProjectObjectToScreen(CameraObject& aObject, bool aUseLight = false);
		void ProjectToScreen(Point& aPoint, bool aUseLight = false);
		void ProjectArrayToScreen(GrowingArray<Point>& aArray, bool aUseLight = false);

	protected:
		void InsertionSortPoint(GrowingArray<Point>& aUnsortedArray);

		GrowingArray<Light> myLights;
	};
}