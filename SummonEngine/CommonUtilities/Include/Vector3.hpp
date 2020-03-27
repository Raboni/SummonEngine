#pragma once
#include <math.h>
#include <assert.h>

namespace CU
{
	template<class T>
	class Vector2;
	template<class T>
	class Vector4;

	template<class T>
	class Vector3
	{
	public:
		Vector3<T>();//Creates a null-vector
		Vector3<T>(const T& aX, const T& aY, const T& aZ);//Creates a vector (aX, aY, aZ)
		Vector3<T>(const Vector4<T>& aVector4);
		Vector3<T>(const Vector3<T>& aVector) = default;//Copy constructor (compiler generated)
		Vector3<T>(const Vector2<T>& aVector2, const T& aZ);
		~Vector3<T>() = default;//Destructor (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;//Assignment operator (compiler generated)

		T LengthSqr() const;//Returns the squared length of the vector
		T Length() const;//Returns the length of the vector

		Vector3<T> GetNormalized() const;//Returns a normalized copy of this
		void Normalize();//Normalizes the vector

		T Dot(const Vector3<T>& aVector) const;//Returns the dot product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const;//Returns the cross product of this and aVector

	public:
		static const Vector3<T> Zero;
		static const Vector3<T> One;

		T x;
		T y;
		T z;
	};
	typedef Vector3<float> Vector3f;
	typedef Vector3<int> Vector3i;

	template<class T>
	const Vector3<T> Vector3<T>::Zero;
	template<class T>
	const Vector3<T> Vector3<T>::One = Vector3<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));

	//Returns the vector sum of aVector0 and aVector1
	template<class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> temp;
		temp.x = aVector0.x + aVector1.x;
		temp.y = aVector0.y + aVector1.y;
		temp.z = aVector0.z + aVector1.z;
		return temp;
	}

	//Returns the vector difference of aVector0 and aVector1
	template<class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> temp;
		temp.x = aVector0.x - aVector1.x;
		temp.y = aVector0.y - aVector1.y;
		temp.z = aVector0.z - aVector1.z;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> temp;
		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		Vector3<T> temp;
		temp.x = aVector.x * aScalar;
		temp.y = aVector.y * aScalar;
		temp.z = aVector.z * aScalar;
		return temp;
	}

	template<class T>
	Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> temp;
		temp.x = aVector0.x * aVector1.x;
		temp.y = aVector0.y * aVector1.y;
		temp.z = aVector0.z * aVector1.z;
		return temp;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template<class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "can't divide by 0");
		Vector3<T> temp;
		temp.x = aVector.x / aScalar;
		temp.y = aVector.y / aScalar;
		temp.z = aVector.z / aScalar;
		return temp;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template<class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template<class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template<class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	template<class T>
	void operator*=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x *= aVector1.x;
		aVector0.y *= aVector1.y;
		aVector0.z *= aVector1.z;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template<class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "can't divide by 0");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
	}

	template<class T>
	bool operator==(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return (aVector0.x == aVector1.x && aVector0.y == aVector1.y && aVector0.z == aVector1.z);
	}
	template<class T>
	bool operator!=(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return (aVector0.x != aVector1.x || aVector0.y != aVector1.y || aVector0.z != aVector1.z);
	}


	template<class T>
	inline Vector3<T>::Vector3()
	{
		x = static_cast<T>(0);
		y = static_cast<T>(0);
		z = static_cast<T>(0);
	}

	template<class T>
	inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}
	template<class T>
	inline Vector3<T>::Vector3(const Vector4<T>& aVector4)
	{
		x = aVector4.x;
		y = aVector4.y;
		z = aVector4.z;
	}
	template<class T>
	inline Vector3<T>::Vector3(const Vector2<T>& aVector2, const T & aZ)
	{
		x = aVector2.x;
		y = aVector2.y;
		z = aZ;
	}

	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return (x * x) + (y * y) + (z * z);
	}
	template<class T>
	inline T Vector3<T>::Length() const
	{
		return static_cast<T>(sqrt(static_cast<double>((x * x) + (y * y) + (z * z))));
	}

	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		Vector3<T> temp(*this);
		T length = Length();
		if (length != 0)
		{
			temp.x = x / length;
			temp.y = y / length;
			temp.z = z / length;
		}
		return temp;
	}
	template<class T>
	inline void Vector3<T>::Normalize()
	{
		T length = Length();
		if (length != 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
	}

	template<class T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
	}
	template<class T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		Vector3<T> temp;
		temp.x = (y * aVector.z) - (z * aVector.y);
		temp.y = (z * aVector.x) - (x * aVector.z);
		temp.z = (x * aVector.y) - (y * aVector.x);
		return temp;
	}
}