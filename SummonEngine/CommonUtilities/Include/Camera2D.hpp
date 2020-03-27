#pragma once
#include "GrowingArray.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

namespace CU
{
	struct Point2D
	{
		Point2D() = default;
		Point2D(unsigned int aID, const Vector2f& aPosition, int aLayer = 0)
		{
			myPosition = aPosition;
			myID = aID;
			myLayer = aLayer;
		}

		Vector2f myProjectedPosition;
		Vector2f myProjectedSize;
		Vector2f myPosition;
		unsigned int myID;
		int myLayer;
		bool myOnScreen;
	};

	class Camera2D
	{
	public:
		Camera2D();
		~Camera2D();

		Matrix4x4<float> GetSpace();

		void MoveRight();
		void MoveLeft();
		void MoveUp();
		void MoveDown();
		void ZoomIn();
		void ZoomOut();
		void Translate(const Vector3f& aTranslation);

		void RotateTiltRight();
		void RotateTiltLeft();
		void RotateTilt(float aRotationInRadians);
		void RotateReset();

		virtual void ProjectToScreen(Point2D& aPoint);
		virtual void ProjectArrayToScreen(GrowingArray<Point2D>& aArray);
		Vector2f GetPosition();
		float GetZoom();
		float GetRotation();

	protected:
		Matrix4x4<float> myCameraSpace;
		float myRotation;
		bool myUseSafeRotation;

	private:
		Matrix4x4<float> myCameraMoveRight;
		Matrix4x4<float> myCameraMoveLeft;
		Matrix4x4<float> myCameraMoveForward;
		Matrix4x4<float> myCameraMoveBack;
		Matrix4x4<float> myCameraMoveUp;
		Matrix4x4<float> myCameraMoveDown;

		Matrix4x4<float> myCameraRotateTiltRight;
		Matrix4x4<float> myCameraRotateTiltLeft;
	};
}