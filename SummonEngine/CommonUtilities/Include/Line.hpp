#pragma once
#include "Vector2.hpp"

namespace CU
{
	template<typename T>
	class Line
	{
	public:
		Line();
		~Line() = default;
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);
		void InitWithPointAndNormal(const Vector2<T>& aPoint, const Vector2<T>& aNormal);
		bool Inside(const Vector2<T>& aPosition) const;

	private:
		Vector2<T> myPoint;
		Vector2<T> myNormal;
	};

	template<typename T>
	inline Line<T>::Line()
	{
		myPoint = { 0, 0 };
		myNormal = { 0, 0 };
	}

	template<typename T>
	inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		Vector2<T> direction = aPoint1 - aPoint0;
		direction.Normalize();
		myNormal = { -direction.y, direction.x };
	}

	template<typename T>
	inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		Vector2<T> direction = aPoint1 - aPoint0;
		direction.Normalize();
		myNormal = { -direction.y, direction.x };
	}

	template<typename T>
	inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
	{
		myPoint = aPoint;
		myNormal = { -aDirection.y, aDirection.x };
	}

	template<typename T>
	inline void Line<T>::InitWithPointAndNormal(const Vector2<T>& aPoint, const Vector2<T>& aNormal)
	{
		myPoint = aPoint;
		myNormal = aNormal.GetNormalized();
	}

	template<typename T>
	inline bool Line<T>::Inside(const Vector2<T>& aPosition) const
	{
		Vector2<T> temp = (aPosition - myPoint);
		T dot = temp.Dot(myNormal);
		return (dot < 0);
	}
}