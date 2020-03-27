#pragma once
#include <math.h>
#include <assert.h>

namespace CU
{
	template<class T>
	class Vector2
	{
	public:
		static Vector2<T> AngleToVector(float aAngleInRadians);
		static float VectorToAngle(Vector2<T> aVector);

		Vector2<T>();//Creates a null-vector
		Vector2<T>(const T& aX, const T& aY);//Creates a vector (aX, aY)
		Vector2<T>(const Vector2<T>& aVector) = default;//Copy constructor (compiler generated)
		template<class T2>
		Vector2<T>(const Vector2<T2>& aVector);
		~Vector2<T>() = default;//Destructor (compiler generated)
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;//Assignment operator (compiler generated)

		T LengthSqr() const;//Returns the squared length of the vector
		T Length() const;//Returns the length of the vector

		Vector2<T> GetNormalized() const;//Returns a normalized copy of this
		void Normalize();//Normalizes the vector

		T Dot(const Vector2<T>& aVector) const;//Returns the dot product of this and aVector

	public:
		static const Vector2<T> Zero;
		static const Vector2<T> One;

		T x;
		T y;
	};
	typedef Vector2<float> Vector2f;
	typedef Vector2<int> Vector2i;

	template<class T>
	const Vector2<T> Vector2<T>::Zero;
	template<class T>
	const Vector2<T> Vector2<T>::One = Vector2<T>(static_cast<T>(1), static_cast<T>(1));

	//Returns the vector sum of aVector0 and aVector1
	template<class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> temp;
		temp.x = aVector0.x + aVector1.x;
		temp.y = aVector0.y + aVector1.y;
		return temp;
	}
	//Returns the vector difference of aVector0 and aVector1
	template<class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> temp;
		temp.x = aVector0.x - aVector1.x;
		temp.y = aVector0.y - aVector1.y;
		return temp;
	}
	//Returns the vector aVector1 multiplied by the vector aVector2
	template<class T>
	Vector2<T> operator*(const Vector2<T>& aVector1, const Vector2<T>& aVector2)
	{
		Vector2<T> temp;
		temp.x = aVector1.x * aVector2.x;
		temp.y = aVector1.y * aVector2.y;
		return temp;
	}
	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		Vector2<T> temp;
		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		return temp;
	}
	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		Vector2<T> temp;
		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		return temp;
	}
	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "can't divide by 0");
		Vector2<T> temp;
		temp.x = aVector.x / aScalar;
		temp.y = aVector.y / aScalar;
		return temp;
	}
	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template<class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}
	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template<class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}
	//Equivalent to setting aVector to (aVector * aScalar)
	template<class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}
	//Equivalent to setting aVector to (aVector / aScalar)
	template<class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "can't divide by 0");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
	}
	template<class T>
	bool operator==(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return (aVector0.x == aVector1.x && aVector0.y == aVector1.y);
	}
	template<class T>
	bool operator!=(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return (aVector0.x != aVector1.x || aVector0.y != aVector1.y);
	}


	template<class T>
	inline Vector2<T>::Vector2()
	{
		x = static_cast<T>(0);
		y = static_cast<T>(0);
	}
	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}
	template<class T>
	template<class T2>
	inline Vector2<T>::Vector2(const Vector2<T2>& aVector)
	{
		x = static_cast<T>(aVector.x);
		y = static_cast<T>(aVector.y);
	}

	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return (x * x) + (y * y);
	}
	template<class T>
	inline T Vector2<T>::Length() const
	{
		return static_cast<T>(sqrt(static_cast<double>((x * x) + (y * y))));
	}

	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		Vector2<T> temp(*this);
		T length = Length();
		if (length != 0)
		{
			temp.x = x / length;
			temp.y = y / length;
		}
		return temp;
	}
	template<class T>
	inline void Vector2<T>::Normalize()
	{
		T length = Length();
		if (length != 0)
		{
			x /= length;
			y /= length;
		}
	}

	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::AngleToVector(float aAngleInRadians)
	{
		return Vector2<T>(static_cast<T>(cos(aAngleInRadians)), static_cast<T>(sin(aAngleInRadians)));
	}
	template<typename T>
	inline float Vector2<T>::VectorToAngle(Vector2<T> aVector)
	{
		return static_cast<float>(atan2(static_cast<double>(aVector.y), static_cast<double>(aVector.x)));
	}
}