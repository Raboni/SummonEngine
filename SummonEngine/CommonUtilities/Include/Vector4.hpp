#pragma once
#include <math.h>
#include <cassert>
#include "Vector3.hpp"

namespace CU
{
	template<class T>
	class Vector4
	{
	public:
		Vector4<T>();//Creates a null-vector
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);//Creates a vector (aX, aY, aZ, aW)
		Vector4<T>(const Vector3<T>& aVector3, const T& aW);
		Vector4<T>(const Vector4<T>& aVector) = default;//Copy constructor (compiler generated)
		~Vector4<T>() = default;//Destructor (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;//Assignment operator (compiler generated)

		T LengthSqr() const;//Returns the squared length of the vector
		T Length() const;//Returns the length of the vector

		Vector4<T> GetNormalized() const;//Returns a normalized copy of this
		void Normalize();//Normalizes the vector

		T Dot(const Vector4<T>& aVector) const;//Returns the dot product of this and aVector

	public:
		static const Vector4<T> Zero;
		static const Vector4<T> One;

		T x;
		T y;
		T z;
		T w;
	};
	typedef Vector4<float> Vector4f;
	typedef Vector4<int> Vector4i;

	template<class T>
	const Vector4<T> Vector4<T>::Zero;
	template<class T>
	const Vector4<T> Vector4<T>::One = Vector4<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));

	//Returns the vector sum of aVector0 and aVector1
	template<class T>
	Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp;
		temp.x = aVector0.x + aVector1.x;
		temp.y = aVector0.y + aVector1.y;
		temp.z = aVector0.z + aVector1.z;
		temp.w = aVector0.w + aVector1.w;
		return temp;
	}

	//Returns the vector difference of aVector0 and aVector1
	template<class T>
	Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp;
		temp.x = aVector0.x - aVector1.x;
		temp.y = aVector0.y - aVector1.y;
		temp.z = aVector0.z - aVector1.z;
		temp.w = aVector0.w - aVector1.w;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> temp;
		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;
		temp.w = aVector.w * aScalar;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T>
	Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		Vector4<T> temp;
		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;
		temp.w = aVector.w * aScalar;
		return temp;
	}

	template<class T>
	Vector4<T> operator*(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp;
		temp.x = aVector0.x * aVector1.x;
		temp.y = aVector0.y * aVector1.y;
		temp.z = aVector0.z * aVector1.z;
		temp.w = aVector0.w * aVector1.w;
		return temp;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template<class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "can't divide by 0");
		Vector4<T> temp;
		temp.x = aVector.x / aScalar;
		temp.y = aVector.y / aScalar;
		temp.z = aVector.z / aScalar;
		temp.w = aVector.w / aScalar;
		return temp;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template<class T>
	void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template<class T>
	void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
		aVector0.w -= aVector1.w;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template<class T>
	void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}

	template<class T>
	void operator*=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x *= aVector1.x;
		aVector0.y *= aVector1.y;
		aVector0.z *= aVector1.z;
		aVector0.w *= aVector1.w;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template<class T>
	void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "can't divide by 0");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
	}

	template<class T>
	bool operator==(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return (aVector0.x == aVector1.x && aVector0.y == aVector1.y && aVector0.z == aVector1.z && aVector0.w == aVector1.w);
	}
	template<class T>
	bool operator!=(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return (aVector0.x != aVector1.x || aVector0.y != aVector1.y || aVector0.z != aVector1.z || aVector0.w != aVector1.w);
	}


	template<class T>
	inline Vector4<T>::Vector4()
	{
		x = static_cast<T>(0);
		y = static_cast<T>(0);
		z = static_cast<T>(0);
		w = static_cast<T>(0);
	}
	template<class T>
	inline Vector4<T>::Vector4(const T & aX, const T & aY, const T & aZ, const T & aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector3<T>& aVector3, const T & aW)
	{
		x = aVector3.x;
		y = aVector3.y;
		z = aVector3.z;
		w = aW;
	}

	template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
		return (x * x) + (y * y) + (z * z) + (w * w);
	}
	template<class T>
	inline T Vector4<T>::Length() const
	{
		return static_cast<T>(sqrt(static_cast<double>((x * x) + (y * y) + (z * z) + (w * w))));
	}

	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		Vector4<T> temp(*this);
		T length = Length();
		if (length != 0)
		{
			temp.x = x / length;
			temp.y = y / length;
			temp.z = z / length;
			temp.w = w / length;
		}
		return temp;
	}
	template<class T>
	inline void Vector4<T>::Normalize()
	{
		T length = Length();
		if (length != 0)
		{
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}
	}

	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w);
	}
}