#pragma once
#include "StaticArray.hpp"
#include "Vector4.hpp"
#include <string>

namespace CU
{
	template<typename T>
	class Matrix3x3;

	template<typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4();
		Matrix4x4(const Matrix4x4<T>& aMatrix);
		Matrix4x4(const Matrix3x3<T>& aMatrix);
		~Matrix4x4() = default;
		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix);
		const T& operator()(int aRow, int aColumn) const;
		T& operator()(int aRow, int aColumn);
		const T& operator[](int aIndex) const;
		T& operator[](int aIndex);
		Matrix4x4<T> GetFastInverse() const;
		Matrix4x4<T> GetInverse() const;

		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
		static Matrix4x4<T> Transpose3x3(const Matrix4x4<T>& aMatrixToTranspose);

	private:
		StaticArray<T, 16> myMatrix;
	};
	typedef Matrix4x4<float> Matrix4x4f;

	template<class T>
	Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> temp;
		for (int index = 0; index < 16; index++)
		{
			temp[index] = aMatrix0[index] + aMatrix1[index];
		}
		return temp;
	}
	template<class T>
	void operator+=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int index = 0; index < 16; index++)
		{
			aMatrix0[index] += aMatrix1[index];
		}
	}
	template<class T>
	Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> temp;
		for (int index = 0; index < 16; index++)
		{
			temp[index] = aMatrix0[index] - aMatrix1[index];
		}
		return temp;
	}
	template<class T>
	void operator-=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int index = 0; index < 16; index++)
		{
			aMatrix0[index] -= aMatrix1[index];
		}
	}

	template<class T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> temp;
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				temp[(row * 4) + col] = aMatrix0[(row * 4)] * aMatrix1[col]
					+ aMatrix0[(row * 4) + 1] * aMatrix1[col + 4]
					+ aMatrix0[(row * 4) + 2] * aMatrix1[col + 8]
					+ aMatrix0[(row * 4) + 3] * aMatrix1[col + 12];
			}
		}
		return temp;
	}
	template<class T>
	void operator*=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> temp;
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				temp[(row * 4) + col] = aMatrix0[(row * 4)] * aMatrix1[col]
					+ aMatrix0[(row * 4) + 1] * aMatrix1[col + 4]
					+ aMatrix0[(row * 4) + 2] * aMatrix1[col + 8]
					+ aMatrix0[(row * 4) + 3] * aMatrix1[col + 12];
			}
		}
		aMatrix0 = temp;
	}
	template<class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> temp;
		temp.x = aVector.x * aMatrix[0]
			+ aVector.y * aMatrix[4]
			+ aVector.z * aMatrix[8]
			+ aVector.w * aMatrix[12];
		temp.y = aVector.x * aMatrix[1]
			+ aVector.y * aMatrix[5]
			+ aVector.z * aMatrix[9]
			+ aVector.w * aMatrix[13];
		temp.z = aVector.x * aMatrix[2]
			+ aVector.y * aMatrix[6]
			+ aVector.z * aMatrix[10]
			+ aVector.w * aMatrix[14];
		temp.w = aVector.x * aMatrix[3]
			+ aVector.y * aMatrix[7]
			+ aVector.z * aMatrix[11]
			+ aVector.w * aMatrix[15];
		return temp;
	}
	template<class T>
	void operator*=(Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> temp;
		temp.x = aVector.x * aMatrix[0]
			+ aVector.y * aMatrix[4]
			+ aVector.z * aMatrix[8]
			+ aVector.w * aMatrix[12];
		temp.y = aVector.x * aMatrix[1]
			+ aVector.y * aMatrix[5]
			+ aVector.z * aMatrix[9]
			+ aVector.w * aMatrix[13];
		temp.z = aVector.x * aMatrix[2]
			+ aVector.y * aMatrix[6]
			+ aVector.z * aMatrix[10]
			+ aVector.w * aMatrix[14];
		temp.w = aVector.x * aMatrix[3]
			+ aVector.y * aMatrix[7]
			+ aVector.z * aMatrix[11]
			+ aVector.w * aMatrix[15];
		aVector = temp;
	}
	template<class T>
	bool operator==(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int index = 0; index < 16; index++)
		{
			if (aMatrix0[index] != aMatrix1[index])
			{
				return false;
			}
		}
		return true;
	}


	template<typename T>
	inline Matrix4x4<T>::Matrix4x4()
	{
		/*for (int index = 0; index < 16; index++)
		{
			if (index % 5 == 0)
			{
				myMatrix[index] = 1;
			}
			else
			{
				myMatrix[index] = 0;
			}
		}*/
		memset(&myMatrix, 0, sizeof(float) * 16);
		myMatrix[0] = 1.0f;
		myMatrix[5] = 1.0f;
		myMatrix[10] = 1.0f;
		myMatrix[15] = 1.0f;
	}
	template<typename T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		memcpy(&myMatrix, &aMatrix.myMatrix, sizeof(CU::StaticArray<T, 16>));
		/*for (int index = 0; index < 16; index++)
		{
			myMatrix[index] = aMatrix[index];
		}*/
	}
	template<typename T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix3x3<T>& aMatrix)
	{
		for (int index = 0; index < 9; index++)
		{
			int row = index / 3;
			int col = index % 3;
			myMatrix[(row * 4) + col] = aMatrix[index];
		}
		myMatrix[3] = 0.0f;
		myMatrix[7] = 0.0f;
		myMatrix[11] = 0.0f;
		myMatrix[12] = 0.0f;
		myMatrix[13] = 0.0f;
		myMatrix[14] = 0.0f;
		myMatrix[15] = 1.0f;
	}
	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		memcpy(&myMatrix, &aMatrix.myMatrix, sizeof(CU::StaticArray<T, 16>));
		/*for (int index = 0; index < 16; index++)
		{
			myMatrix[index] = aMatrix[index];
		}*/
		return *this;
	}
	template<typename T>
	inline const T& Matrix4x4<T>::operator()(int aRow, int aColumn) const
	{
		return myMatrix[((aRow - 1) * 4) + aColumn - 1];
	}
	template<typename T>
	inline T& Matrix4x4<T>::operator()(int aRow, int aColumn)
	{
		return myMatrix[((aRow - 1) * 4) + aColumn - 1];
	}
	template<typename T>
	inline const T& Matrix4x4<T>::operator[](int aIndex) const
	{
		return myMatrix[aIndex];
	}
	template<typename T>
	inline T& Matrix4x4<T>::operator[](int aIndex)
	{
		return myMatrix[aIndex];
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse() const
	{
		Vector4<T> translationVector = { -myMatrix[12],-myMatrix[13] ,-myMatrix[14] , static_cast<T>(0) };
		Matrix4x4<T> returnMatrix = Matrix4x4<T>::Transpose3x3(*this);

		returnMatrix[3] = static_cast<T>(0);
		returnMatrix[7] = static_cast<T>(0);
		returnMatrix[11] = static_cast<T>(0);
		returnMatrix[15] = static_cast<T>(0);
		translationVector = translationVector * returnMatrix;

		returnMatrix[12] = translationVector.x;
		returnMatrix[13] = translationVector.y;
		returnMatrix[14] = translationVector.z;
		returnMatrix[15] = static_cast<T>(1);

		return returnMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetInverse() const
	{
		Matrix4x4<T> retMatrix;
		//start turn [4][4] into [16]
		T matrixToInverse[16], determinant;
		determinant;
		int j = 0;
		for (unsigned int index = 0; index < 16; ++index)
		{
			unsigned int i = index % 4;
			matrixToInverse[index] = (*this)(j + 1, i + 1);					//switch this if x and y is wrong(also when u revert back)

			if (i == 3)
				++j;
		}
		//end turn [4][4] into [16]

		//start real inverse magic
		//https://www.gamedev.net/forums/topic/648190-algorithm-for-4x4-matrix-inverse/
		T inv[16], invOut[16];

		inv[0] = matrixToInverse[5] * matrixToInverse[10] * matrixToInverse[15] -
			matrixToInverse[5] * matrixToInverse[11] * matrixToInverse[14] -
			matrixToInverse[9] * matrixToInverse[6] * matrixToInverse[15] +
			matrixToInverse[9] * matrixToInverse[7] * matrixToInverse[14] +
			matrixToInverse[13] * matrixToInverse[6] * matrixToInverse[11] -
			matrixToInverse[13] * matrixToInverse[7] * matrixToInverse[10];

		inv[4] = -matrixToInverse[4] * matrixToInverse[10] * matrixToInverse[15] +
			matrixToInverse[4] * matrixToInverse[11] * matrixToInverse[14] +
			matrixToInverse[8] * matrixToInverse[6] * matrixToInverse[15] -
			matrixToInverse[8] * matrixToInverse[7] * matrixToInverse[14] -
			matrixToInverse[12] * matrixToInverse[6] * matrixToInverse[11] +
			matrixToInverse[12] * matrixToInverse[7] * matrixToInverse[10];

		inv[8] = matrixToInverse[4] * matrixToInverse[9] * matrixToInverse[15] -
			matrixToInverse[4] * matrixToInverse[11] * matrixToInverse[13] -
			matrixToInverse[8] * matrixToInverse[5] * matrixToInverse[15] +
			matrixToInverse[8] * matrixToInverse[7] * matrixToInverse[13] +
			matrixToInverse[12] * matrixToInverse[5] * matrixToInverse[11] -
			matrixToInverse[12] * matrixToInverse[7] * matrixToInverse[9];

		inv[12] = -matrixToInverse[4] * matrixToInverse[9] * matrixToInverse[14] +
			matrixToInverse[4] * matrixToInverse[10] * matrixToInverse[13] +
			matrixToInverse[8] * matrixToInverse[5] * matrixToInverse[14] -
			matrixToInverse[8] * matrixToInverse[6] * matrixToInverse[13] -
			matrixToInverse[12] * matrixToInverse[5] * matrixToInverse[10] +
			matrixToInverse[12] * matrixToInverse[6] * matrixToInverse[9];

		inv[1] = -matrixToInverse[1] * matrixToInverse[10] * matrixToInverse[15] +
			matrixToInverse[1] * matrixToInverse[11] * matrixToInverse[14] +
			matrixToInverse[9] * matrixToInverse[2] * matrixToInverse[15] -
			matrixToInverse[9] * matrixToInverse[3] * matrixToInverse[14] -
			matrixToInverse[13] * matrixToInverse[2] * matrixToInverse[11] +
			matrixToInverse[13] * matrixToInverse[3] * matrixToInverse[10];

		inv[5] = matrixToInverse[0] * matrixToInverse[10] * matrixToInverse[15] -
			matrixToInverse[0] * matrixToInverse[11] * matrixToInverse[14] -
			matrixToInverse[8] * matrixToInverse[2] * matrixToInverse[15] +
			matrixToInverse[8] * matrixToInverse[3] * matrixToInverse[14] +
			matrixToInverse[12] * matrixToInverse[2] * matrixToInverse[11] -
			matrixToInverse[12] * matrixToInverse[3] * matrixToInverse[10];

		inv[9] = -matrixToInverse[0] * matrixToInverse[9] * matrixToInverse[15] +
			matrixToInverse[0] * matrixToInverse[11] * matrixToInverse[13] +
			matrixToInverse[8] * matrixToInverse[1] * matrixToInverse[15] -
			matrixToInverse[8] * matrixToInverse[3] * matrixToInverse[13] -
			matrixToInverse[12] * matrixToInverse[1] * matrixToInverse[11] +
			matrixToInverse[12] * matrixToInverse[3] * matrixToInverse[9];

		inv[13] = matrixToInverse[0] * matrixToInverse[9] * matrixToInverse[14] -
			matrixToInverse[0] * matrixToInverse[10] * matrixToInverse[13] -
			matrixToInverse[8] * matrixToInverse[1] * matrixToInverse[14] +
			matrixToInverse[8] * matrixToInverse[2] * matrixToInverse[13] +
			matrixToInverse[12] * matrixToInverse[1] * matrixToInverse[10] -
			matrixToInverse[12] * matrixToInverse[2] * matrixToInverse[9];

		inv[2] = matrixToInverse[1] * matrixToInverse[6] * matrixToInverse[15] -
			matrixToInverse[1] * matrixToInverse[7] * matrixToInverse[14] -
			matrixToInverse[5] * matrixToInverse[2] * matrixToInverse[15] +
			matrixToInverse[5] * matrixToInverse[3] * matrixToInverse[14] +
			matrixToInverse[13] * matrixToInverse[2] * matrixToInverse[7] -
			matrixToInverse[13] * matrixToInverse[3] * matrixToInverse[6];

		inv[6] = -matrixToInverse[0] * matrixToInverse[6] * matrixToInverse[15] +
			matrixToInverse[0] * matrixToInverse[7] * matrixToInverse[14] +
			matrixToInverse[4] * matrixToInverse[2] * matrixToInverse[15] -
			matrixToInverse[4] * matrixToInverse[3] * matrixToInverse[14] -
			matrixToInverse[12] * matrixToInverse[2] * matrixToInverse[7] +
			matrixToInverse[12] * matrixToInverse[3] * matrixToInverse[6];

		inv[10] = matrixToInverse[0] * matrixToInverse[5] * matrixToInverse[15] -
			matrixToInverse[0] * matrixToInverse[7] * matrixToInverse[13] -
			matrixToInverse[4] * matrixToInverse[1] * matrixToInverse[15] +
			matrixToInverse[4] * matrixToInverse[3] * matrixToInverse[13] +
			matrixToInverse[12] * matrixToInverse[1] * matrixToInverse[7] -
			matrixToInverse[12] * matrixToInverse[3] * matrixToInverse[5];

		inv[14] = -matrixToInverse[0] * matrixToInverse[5] * matrixToInverse[14] +
			matrixToInverse[0] * matrixToInverse[6] * matrixToInverse[13] +
			matrixToInverse[4] * matrixToInverse[1] * matrixToInverse[14] -
			matrixToInverse[4] * matrixToInverse[2] * matrixToInverse[13] -
			matrixToInverse[12] * matrixToInverse[1] * matrixToInverse[6] +
			matrixToInverse[12] * matrixToInverse[2] * matrixToInverse[5];

		inv[3] = -matrixToInverse[1] * matrixToInverse[6] * matrixToInverse[11] +
			matrixToInverse[1] * matrixToInverse[7] * matrixToInverse[10] +
			matrixToInverse[5] * matrixToInverse[2] * matrixToInverse[11] -
			matrixToInverse[5] * matrixToInverse[3] * matrixToInverse[10] -
			matrixToInverse[9] * matrixToInverse[2] * matrixToInverse[7] +
			matrixToInverse[9] * matrixToInverse[3] * matrixToInverse[6];

		inv[7] = matrixToInverse[0] * matrixToInverse[6] * matrixToInverse[11] -
			matrixToInverse[0] * matrixToInverse[7] * matrixToInverse[10] -
			matrixToInverse[4] * matrixToInverse[2] * matrixToInverse[11] +
			matrixToInverse[4] * matrixToInverse[3] * matrixToInverse[10] +
			matrixToInverse[8] * matrixToInverse[2] * matrixToInverse[7] -
			matrixToInverse[8] * matrixToInverse[3] * matrixToInverse[6];

		inv[11] = -matrixToInverse[0] * matrixToInverse[5] * matrixToInverse[11] +
			matrixToInverse[0] * matrixToInverse[7] * matrixToInverse[9] +
			matrixToInverse[4] * matrixToInverse[1] * matrixToInverse[11] -
			matrixToInverse[4] * matrixToInverse[3] * matrixToInverse[9] -
			matrixToInverse[8] * matrixToInverse[1] * matrixToInverse[7] +
			matrixToInverse[8] * matrixToInverse[3] * matrixToInverse[5];

		inv[15] = matrixToInverse[0] * matrixToInverse[5] * matrixToInverse[10] -
			matrixToInverse[0] * matrixToInverse[6] * matrixToInverse[9] -
			matrixToInverse[4] * matrixToInverse[1] * matrixToInverse[10] +
			matrixToInverse[4] * matrixToInverse[2] * matrixToInverse[9] +
			matrixToInverse[8] * matrixToInverse[1] * matrixToInverse[6] -
			matrixToInverse[8] * matrixToInverse[2] * matrixToInverse[5];

		determinant = matrixToInverse[0] * inv[0] + matrixToInverse[1] * inv[4] + matrixToInverse[2] * inv[8] + matrixToInverse[3] * inv[12];

		if (determinant == 0)
			assert("determinant was 0 in realinvserse calculation(divide by zero not allowed without supervision)");

		determinant = 1 / determinant;

		for (int i = 0; i < 16; ++i)
			invOut[i] = inv[i] * determinant;
		//end real inverse magic

		//start revert from [4][4] to [16]
		j = 0;
		for (unsigned int index = 0; index < 16; ++index)
		{
			unsigned int k = index % 4;
			retMatrix(j + 1, k + 1) = invOut[index];					//switch this if x and y is wrong

			if (k == 3)
				++j;
		}
		//end revert from [4][4] to [16]

		return retMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4<T> rotMatrix;
		rotMatrix[5] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[6] = static_cast<T>(sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[9] = static_cast<T>(-sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[10] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[15] = 0.0f;
		return rotMatrix;
	}
	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4<T> rotMatrix;
		rotMatrix[0] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[2] = static_cast<T>(-sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[8] = static_cast<T>(sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[10] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[15] = 0.0f;
		return rotMatrix;
	}
	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4<T> rotMatrix;
		rotMatrix[0] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[1] = static_cast<T>(sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[4] = static_cast<T>(-sin(static_cast<double>(aAngleInRadians)));
		rotMatrix[5] = static_cast<T>(cos(static_cast<double>(aAngleInRadians)));
		rotMatrix[15] = 0.0f;
		return rotMatrix;
	}
	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> transMatrix(aMatrixToTranspose);

		transMatrix[1] = aMatrixToTranspose[4];
		transMatrix[4] = aMatrixToTranspose[1];

		transMatrix[2] = aMatrixToTranspose[8];
		transMatrix[8] = aMatrixToTranspose[2];

		transMatrix[3] = aMatrixToTranspose[12];
		transMatrix[12] = aMatrixToTranspose[3];

		transMatrix[6] = aMatrixToTranspose[9];
		transMatrix[9] = aMatrixToTranspose[6];

		transMatrix[7] = aMatrixToTranspose[13];
		transMatrix[13] = aMatrixToTranspose[7];

		transMatrix[11] = aMatrixToTranspose[14];
		transMatrix[14] = aMatrixToTranspose[11];

		return transMatrix;
	}
	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose3x3(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> transMatrix(aMatrixToTranspose);

		transMatrix[1] = aMatrixToTranspose[4];
		transMatrix[4] = aMatrixToTranspose[1];

		transMatrix[2] = aMatrixToTranspose[8];
		transMatrix[8] = aMatrixToTranspose[2];

		transMatrix[6] = aMatrixToTranspose[9];
		transMatrix[9] = aMatrixToTranspose[6];

		return transMatrix;
	}
}