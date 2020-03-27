#pragma once
#include "GrowingArray.hpp"
#include "Vector.hpp"

namespace CU
{
	struct Point
	{
		Point() = default;
		Point(const Vector3f& aPosition, const Vector3f& aNormal, const Vector4f& aColor)
		{
			myPosition = aPosition;
			myNormal = aNormal;
			myColor = aColor;
		}

		Vector3f myPosition;
		Vector3f myNormal;
		Vector4f myColor;
		Vector3f myProjectedPosition;
		Vector2f myProjectedSize;
		Vector4f myProjectedColor;
	};
	struct Material
	{
		Material()
		{
			mySpecularColor = CU::Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f);
			myEmissiveColor = CU::Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f);
			myGloss = 15.0f;
		}

		Vector4f mySpecularColor;
		Vector4f myDiffuseColor;
		Vector4f myEmissiveColor;
		float myGloss;
	};

	class CameraObject
	{
	public:
		CameraObject();
		CameraObject(const GrowingArray<Point>& aPointArray);
		~CameraObject() = default;

		void SetMaterial(const Material& aMaterial);
		void SetPoints(const GrowingArray<Point>& aPointArray);
		void AddPoint(const Point& aPoint);

		GrowingArray<Point>& GetPoints();
		Material GetMaterial();
		Vector3f GetPosition();
		Vector2f GetPointSize();
		bool GetUseEmissive();
		bool GetUseAmbience();
		bool GetUseDiffuse();
		bool GetUseSpecular();
		void SetPointSize(const Vector2f& aPointSize);
		void SetUseEmissive(bool aBool);
		void SetUseAmbience(bool aBool);
		void SetUseDiffuse(bool aBool);
		void SetUseSpecular(bool aBool);

		void CreateCube(const CU::Vector3f& aPosition, float aSize);
		void CreateSphere(const CU::Vector3f& aPosition, float aSize);
		void CreateDonut(const CU::Vector3f& aPosition, float aSize);

	private:
		GrowingArray<Point> myPoints;
		Vector3f myPosition;
		Material myMaterial;
		Vector2f myPointSize;

		bool myUseEmissive = false;
		bool myUseAmbience = true;
		bool myUseDiffuse = true;
		bool myUseSpecular = true;
	};
}