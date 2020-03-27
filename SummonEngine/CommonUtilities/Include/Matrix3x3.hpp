#pragma once
#include "StaticArray.hpp"
#include "Vector3.hpp"

namespace CU
{
	template<typename T>
	class Matrix4x4;

	template<typename T>
	class Matrix3x3
	{
	public:
		Matrix3x3();
		Matrix3x3(const Matrix3x3<T>& aMatrix);
		Matrix3x3(const Matrix4x4<T>& aMatrix);
		~Matrix3x3() = default;
		Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix);
		const T& operator()(int aRow, int aColumn) const;
		T& operator()(int aRow, int aColumn);
		const T& operator[](int aIndex) const;
		T& operator[](int aIndex);
		Matrix3x3<T> GetFastInverse();

		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

	private:
		StaticArray<T, 9> myMatrix;
	};
	typedef Matrix3x3<float> Matrix3x3f;

	template<class T>
	Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> temp;
		for (int index = 0; index < 9; index++)
		{
			temp[index] = aMatrix0[index] + aMatrix1[index];
		}
		return temp;
	}
	template<class T>
	void operator+=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		for (int index = 0; index < 9; index++)
		{
			aMatrix0[index] += aMatrix1[index];
		}
	}
	template<class T>
	Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> temp;
		for (int index = 0; index < 9; index++)
		{
			temp[index] = aMatrix0[index] - aMatrix1[index];
		}
		return temp;
	}
	template<class T>
	void operator-=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		for (int index = 0; index < 9; index++)
		{
			aMatrix0[index] -= aMatrix1[index];
		}
	}

	template<class T>
	Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> temp;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				temp[(row * 3) + col] = aMatrix0[(row * 3)] * aMatrix1[col]
					+ aMatrix0[(row * 3) + 1] * aMatrix1[col + 3]
					+ aMatrix0[(row * 3) + 2] * aMatrix1[col + 6];
			}
		}
		return temp;
	}
	template<class T>
	void operator*=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> temp;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				temp[(row * 3) + col] = aMatrix0[(row * 3)] * aMatrix1[col]
					+ aMatrix0[(row * 3) + 1] * aMatrix1[col + 3]
					+ aMatrix0[(row * 3) + 2] * aMatrix1[col + 6];
			}
		}
		aMatrix0 = temp;
	}
	template<class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		Vector3<T> temp;
		temp.x = aVector.x * aMatrix[0]
			+ aVector.y * aMatrix[3]
			+ aVector.z * aMatrix[6];
		temp.y = aVector.x * aMatrix[1]
			+ aVector.y * aMatrix[4]
			+ aVector.z * aMatrix[7];
		temp.z = aVector.x * aMatrix[2]
			+ aVector.y * aMatrix[5]
			+ aVector.z * aMatrix[8];
		return temp;
	}

	template<class T>
	bool operator==(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		for (int index = 0; index < 9; index++)
		{
			if (aMatrix0[index] != aMatrix1[index])
			{
				return false;
			}
		}
		return true;
	}


	template<typename T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		for (int index = 0; index < 9; index++)
		{
			if (index % 4 == 0)
			{
				myMatrix[index] = 1;
			}
			else
			{
				myMatrix[index] = 0;
			}
		}
	}
	template<typename T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		for (int index = 0; index < 9; index++)
		{
			myMatrix[index] = aMatrix[index];
		}
	}
	template<typename T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (int index = 0; index < 9; index++)
		{
			int row = index / 3;
			int col = index % 3;
			myMatrix[index] = aMatrix[(row * 4) + col];
		}
	}
	template<typename T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		for (int index = 0; index < 9; index++)
		{
			myMatrix[index] = aMatrix[index];
		}
		return *this;
	}
	template<typename T>
	inline const T& Matrix3x3<T>::operator()(int aRow, int aColumn) const
	{
		return myMatrix[((aRow - 1) * 3) + aColumn - 1];
	}
	template<typename T>
	inline T& Matrix3x3<T>::operator()(int aRow, int aColumn)
	{
		return myMatrix[((aRow - 1) * 3) + aColumn - 1];
	}
	template<typename T>
	inline const T& Matrix3x3<T>::operator[](int aIndex) const
	{
		return myMatrix[aIndex];
	}
	template<typename T>
	inline T& Matrix3x3<T>::operator[](int aIndex)
	{
		return myMatrix[aIndex];
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::GetFastInverse()
	{
		Matrix3x3<T> tempMatrix;
		tempMatrix = *this;
		Matrix3x3<T> returnMatrix;
		Vector3<T> translationVector = { -tempMatrix[6], -tempMatrix[7], static_cast<T>(0) };
		T temp = tempMatrix[1];
		tempMatrix[1] = tempMatrix[3];
		tempMatrix[3] = temp;
		returnMatrix = tempMatrix;

		returnMatrix[2] = static_cast<T>(0);
		returnMatrix[5] = static_cast<T>(0);
		returnMatrix[8] = static_cast<T>(0);
		translationVector = translationVector * returnMatrix;

		returnMatrix[6] = translationVector.x;
		returnMatrix[7] = translationVector.y;
		returnMatrix[8] = static_cast<T>(1);

		return returnMatrix;
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> rotMatrix;
		rotMatrix[4] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[5] = static_cast<T>(sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[7] = static_cast<T>(-sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[8] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		return rotMatrix;
	}
	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3<T> rotMatrix;
		rotMatrix[0] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[2] = static_cast<T>(-sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[6] = static_cast<T>(sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[8] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		return rotMatrix;
	}
	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3<T> rotMatrix;
		rotMatrix[0] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[1] = static_cast<T>(sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[3] = static_cast<T>(-sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[4] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		return rotMatrix;
	}
	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> transMatrix(aMatrixToTranspose);

		transMatrix[1] = aMatrixToTranspose[3];
		transMatrix[3] = aMatrixToTranspose[1];

		transMatrix[2] = aMatrixToTranspose[6];
		transMatrix[6] = aMatrixToTranspose[2];

		transMatrix[5] = aMatrixToTranspose[7];
		transMatrix[7] = aMatrixToTranspose[5];

		return transMatrix;
	}
}