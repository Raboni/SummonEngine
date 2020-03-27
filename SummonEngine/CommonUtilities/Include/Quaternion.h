#pragma once
#include "Math.hpp"
#include "Macro.hpp"
#include "Vector.hpp"

namespace CU
{
	template <typename T>
	class Matrix3x3;
	template <typename T>
	class Matrix4x4;

	template <typename T = float>
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(const T aX, const T aY, const T aZ, const T aW);
		Quaternion(const Vector3<T> aAxis, const float aRadianAngle);
		Quaternion(const T aRadianAngleX, const T aRadianAngleY, const T aRadianAngleZ, const bool aLocalRotation = true);
		Quaternion(const Matrix3x3<T>& aMatrix);
		~Quaternion() = default;

		const Quaternion<T> operator*(const Quaternion<T>& aQuaternion) const;
		void operator*=(const Quaternion<T>& aQuaternion);
		const Quaternion<T> operator/(const float aScalar) const;

		static Quaternion<T> Interpolate(const Quaternion<T>& aFrom, const Quaternion<T>& aTo, const T aExtent);

		void Interpolate(const Quaternion<T>& aTo, const T aExtent);
		void Rotate(const Vector3<T>& aAxis, const float aRadianAngle);
		void LookTowards(const Vector3<T>& aDirection);
		void LookTowards(const Vector3<T>& aDirection, const Quaternion<T>& aSpace);
		void RotateVector(Vector3<T>& aVector) const;

		void Normalize();
		void Conjugate();

		Matrix4x4<T> CreateMatrix4x4() const;
		Matrix3x3<T> CreateMatrix3x3() const;

		Vector3<T> GetVectorX() const;
		Vector3<T> GetVectorY() const;
		Vector3<T> GetVectorZ() const;

		Vector3<T> GetEulerAngles() const;

		Quaternion<T> GetNormalized() const;
		Quaternion<T> GetConjugate() const;
		Quaternion<T> GetInverse() const;

		Vector3<T> GetAxis() const;
		Vector4<T> GetAxisAngle() const;

		T GetMagnitude() const;
		T GetMagnitudeSqr() const;
		T GetAngle() const;

		bool IsUnit() const;

	public:
		static const Quaternion<T> Identity;

		T x;
		T y;
		T z;
		T w;
	};
	typedef Quaternion<float> Quaternionf;

	template<class T>
	const Quaternion<T> Quaternion<T>::Identity;

	template<class T>
	bool operator==(const Quaternion<T>& aQuat0, const Quaternion<T>& aQuat1)
	{
		return (aQuat0.x == aQuat1.x && aQuat0.y == aQuat1.y && aQuat0.z == aQuat1.z && aQuat0.w == aQuat1.w);
	}
	template<class T>
	bool operator!=(const Quaternion<T>& aQuat0, const Quaternion<T>& aQuat1)
	{
		return (aQuat0.x != aQuat1.x || aQuat0.y != aQuat1.y || aQuat0.z != aQuat1.z || aQuat0.w != aQuat1.w);
	}

	template<typename T>
	inline Quaternion<T>::Quaternion()
	{
		x = static_cast<T>(0);
		y = static_cast<T>(0);
		z = static_cast<T>(0);
		w = static_cast<T>(1);
	}
	template<typename T>
	inline Quaternion<T>::Quaternion(const T aX, const T aY, const T aZ, const T aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}
	template<typename T>
	inline Quaternion<T>::Quaternion(const Vector3<T> aAxis, const float aRadianAngle)
	{
		const float theta = aRadianAngle / 2.0f;
		const float s = sin(theta);
		w = cos(theta);
		x = aAxis.x * s;
		y = aAxis.y * s;
		z = aAxis.z * s;
	}
	template<typename T>
	inline Quaternion<T>::Quaternion(const T aRadianAngleX, const T aRadianAngleY, const T aRadianAngleZ, const bool aLocalRotation)
	{
		aLocalRotation;
		//const T pitch = aRadiansX;
		//const T yaw = aRadiansY;
		//const T roll = aRadiansZ;

		*this = Quaternion<T>(CU::Vector3f(1.0f, 0.0f, 0.0f), aRadianAngleX) * Quaternion<T>(CU::Vector3f(0.0f, 1.0f, 0.0f), aRadianAngleY) * Quaternion<T>(CU::Vector3f(0.0f, 0.0f, 1.0f), aRadianAngleZ);

		/*const T X = aRadianAngleY;
		const T Y = aRadianAngleZ;
		const T Z = aRadianAngleX;
		const T fSinPitch(std::sin(X * static_cast<T>(0.5)));
		const T fCosPitch(std::cos(X * static_cast<T>(0.5)));
		const T fSinYaw(std::sin(Y * static_cast<T>(0.5)));
		const T fCosYaw(std::cos(Y * static_cast<T>(0.5)));
		const T fSinRoll(std::sin(Z * static_cast<T>(0.5)));
		const T fCosRoll(std::cos(Z * static_cast<T>(0.5)));
		const T fCosPitchCosYaw(fCosPitch * fCosYaw);
		const T fSinPitchSinYaw(fSinPitch * fSinYaw);
		x = fSinRoll * fCosPitchCosYaw - fCosRoll * fSinPitchSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitchCosYaw + fSinRoll * fSinPitchSinYaw;*/
	}
	template<typename T>
	inline Quaternion<T>::Quaternion(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> toConvert(aMatrix);

		{
			CU::Vector3<T>& matrixVectorX(*reinterpret_cast<CU::Vector3<T>*>(&toConvert[0]));
			CU::Vector3<T>& matrixVectorY(*reinterpret_cast<CU::Vector3<T>*>(&toConvert[3]));
			CU::Vector3<T>& matrixVectorZ(*reinterpret_cast<CU::Vector3<T>*>(&toConvert[6]));
			/*CU::Vector3<T> matrixVectorX(toConvert[0], toConvert[1], toConvert[2]);
			CU::Vector3<T> matrixVectorY(toConvert[3], toConvert[4], toConvert[5]);
			CU::Vector3<T> matrixVectorZ(toConvert[6], toConvert[7], toConvert[8]);*/
			matrixVectorX.Normalize();
			matrixVectorY.Normalize();
			matrixVectorZ.Normalize();
		}

		const T t = toConvert[0] + toConvert[4] + toConvert[8];

		// large enough
		if (t > static_cast<T>(0))
		{
			const T s = std::sqrt(1 + t) * static_cast<T>(2.0);
			x = (toConvert(3, 2) - toConvert(2, 3)) / s;
			y = (toConvert(1, 3) - toConvert(3, 1)) / s;
			z = (toConvert(2, 1) - toConvert(1, 2)) / s;
			w = static_cast<T>(0.25) * s;
		} // else we have to check several cases
		else if (toConvert(1, 1) > toConvert(2, 2) && toConvert(1, 1) > toConvert(3, 3))
		{
			// Column 0:
			T s = std::sqrt(static_cast<T>(1.0) + toConvert(1, 1) - toConvert(2, 2) - toConvert(3, 3)) * static_cast<T>(2.0);
			x = static_cast<T>(0.25) * s;
			y = (toConvert(2, 1) + toConvert(1, 2)) / s;
			z = (toConvert(1, 3) + toConvert(3, 1)) / s;
			w = (toConvert(3, 2) - toConvert(2, 3)) / s;
		}
		else if (toConvert(2, 2) > toConvert(3, 3))
		{
			// Column 1:
			T s = std::sqrt(static_cast<T>(1.0) + toConvert(2, 2) - toConvert(1, 1) - toConvert(3, 3)) * static_cast<T>(2.0);
			x = (toConvert(2, 1) + toConvert(1, 2)) / s;
			y = static_cast<T>(0.25) * s;
			z = (toConvert(3, 2) + toConvert(2, 3)) / s;
			w = (toConvert(1, 3) - toConvert(3, 1)) / s;
		}
		else
		{
			// Column 2:
			T s = std::sqrt(static_cast<T>(1.0) + toConvert(3, 3) - toConvert(1, 1) - toConvert(2, 2)) * static_cast<T>(2.0);
			x = (toConvert(1, 3) + toConvert(3, 1)) / s;
			y = (toConvert(3, 2) + toConvert(2, 3)) / s;
			z = static_cast<T>(0.25) * s;
			w = (toConvert(2, 1) - toConvert(1, 2)) / s;
		}
	}

	template<typename T>
	inline const Quaternion<T> Quaternion<T>::operator*(const Quaternion<T>& aQuaternion) const
	{
		Quaternion<T> quat;
		quat.x = (w * aQuaternion.x) + (x * aQuaternion.w) + (y * aQuaternion.z) - (z * aQuaternion.y);
		quat.y = (w * aQuaternion.y) - (x * aQuaternion.z) + (y * aQuaternion.w) + (z * aQuaternion.x);
		quat.z = (w * aQuaternion.z) + (x * aQuaternion.y) - (y * aQuaternion.x) + (z * aQuaternion.w);
		quat.w = (w * aQuaternion.w) - (x * aQuaternion.x) - (y * aQuaternion.y) - (z * aQuaternion.z);
		return quat;
	}
	template<typename T>
	inline void Quaternion<T>::operator*=(const Quaternion<T>& aQuaternion)
	{
		Quaternion<T> quat;
		quat.x = (w * aQuaternion.x) + (x * aQuaternion.w) + (y * aQuaternion.z) - (z * aQuaternion.y);
		quat.y = (w * aQuaternion.y) - (x * aQuaternion.z) + (y * aQuaternion.w) + (z * aQuaternion.x);
		quat.z = (w * aQuaternion.z) + (x * aQuaternion.y) - (y * aQuaternion.x) + (z * aQuaternion.w);
		quat.w = (w * aQuaternion.w) - (x * aQuaternion.x) - (y * aQuaternion.y) - (z * aQuaternion.z);
		*this = quat;
	}
	template<typename T>
	inline const Quaternion<T> Quaternion<T>::operator/(const float aScalar) const
	{
		return Quaternion<T>(x / aScalar, y / aScalar, z / aScalar, w / aScalar);
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::Interpolate(const Quaternion<T>& aFrom, const Quaternion<T>& aTo, const T aExtent)
	{
		Quaternion<T> quat;

		// calc cosine theta
		T omegaCos = (aFrom.x * aTo.x) + (aFrom.y * aTo.y) + (aFrom.z * aTo.z) + (aFrom.w * aTo.w);

		// adjust signs (if necessary)
		Quaternion<T> to = aTo;

		T sign = static_cast<T>(static_cast<T>(0) < omegaCos) - static_cast<T>(omegaCos < static_cast<T>(0));

		omegaCos *= sign;
		to.x *= sign;
		to.y *= sign;
		to.z *= sign;
		to.w *= sign;

		// Calculate coefficients
		T sclp;
		T sclq;
		if ((static_cast<T>(1.0) - omegaCos) > static_cast<T>(0.0001)) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			T omega;
			T omegaSin;
			omega = std::acos(omegaCos); // extract theta from dot product's cos theta
			omegaSin = std::sin(omega);
			sclp = std::sin((static_cast<T>(1.0) - aExtent) * omega) / omegaSin;
			sclq = std::sin(aExtent * omega) / omegaSin;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = static_cast<T>(1.0) - aExtent;
			sclq = aExtent;
		}

		quat.x = sclp * aFrom.x + sclq * to.x;
		quat.y = sclp * aFrom.y + sclq * to.y;
		quat.z = sclp * aFrom.z + sclq * to.z;
		quat.w = sclp * aFrom.w + sclq * to.w;

		return quat;
	}
	template<typename T>
	inline void Quaternion<T>::Interpolate(const Quaternion<T>& aTo, const T aExtent)
	{
		*this = Interpolate(*this, aTo, aExtent);
	}

	template<typename T>
	inline void Quaternion<T>::Rotate(const Vector3<T>& aAxis, const float aRadianAngle)
	{
		*this *= Quaternion(aAxis, aRadianAngle);
	}

	template<typename T>
	inline void Quaternion<T>::LookTowards(const Vector3<T>& aDirection)
	{
		const Vector3<T> up(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
		const Vector3<T> direction(aDirection.GetNormalized());

		Vector2<T> xzCoordinates(direction.x, direction.z);
		xzCoordinates.Normalize();

		T upwardAlign = direction.Dot(up);
		MIN(MAX(upwardAlign, static_cast<T>(-1)), static_cast<T>(1));

		const T zyPlaneRotation = asin(upwardAlign);
		const T xzPlaneRotation = atan2(-xzCoordinates.x + 0.0f, xzCoordinates.y + 0.0f);

		const Quaternion<T> zy(Vector3<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)), zyPlaneRotation);
		const Quaternion<T> xz(Vector3<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)), xzPlaneRotation);

		*this = zy * xz;
	}
	template<typename T>
	inline void Quaternion<T>::LookTowards(const Vector3<T>& aDirection, const Quaternion<T>& aSpace)
	{
		const Vector3<T> desired(aDirection.GetNormalized());
		const Vector3<T> cardinalUp(aSpace.GetVectorY());
		const Vector3<T> cardinalRight(aSpace.GetVectorX());
		const Vector3<T> cardinalForward(aSpace.GetVectorZ());

		Vector3<T> forward;
		Vector3<T> right;

		T yComponent = desired.Dot(cardinalUp);
		MIN(MAX(yComponent, static_cast<T>(-1)), static_cast<T>(1));

		const T threshhold = static_cast<T>(1.0f - 1e-4f);

		if (!(abs(yComponent) < threshhold))
		{
			forward = (static_cast<T>(static_cast<T>(0) < yComponent) - static_cast<T>(yComponent < static_cast<T>(0))) * cardinalUp;
			right = GetVectorX();

			const T yDeviation = right.Dot(cardinalUp);
			const Vector3<T> rightYComponent = cardinalUp * yDeviation;

			right -= rightYComponent;
		}
		else
		{
			forward = desired;
			right = cardinalUp.Cross(forward);
		}
		right.Normalize();

		const T xComponent = right.Dot(cardinalRight);
		const T zComponent = right.Dot(cardinalForward);


		const Vector2<T> xzCoordinates(Vector2<T>(xComponent, zComponent).GetNormalized());

		const T zyPlaneRotation = asin(yComponent);
		const T xzPlaneRotation = atan2(-xzCoordinates.x + 0.0f, xzCoordinates.y + 0.0f) + static_cast<T>(CU::Math::PI * 0.5);

		const Quaternion<T> zy(cardinalRight, zyPlaneRotation);
		const Quaternion<T> xz(cardinalUp, xzPlaneRotation);

		*this = (aSpace * zy) * xz;
	}

	template<typename T>
	inline void Quaternion<T>::RotateVector(Vector3<T>& aVector) const
	{
		aVector = aVector * CreateMatrix3x3();
	}

	template<typename T>
	inline void Quaternion<T>::Normalize()
	{
		const T magnitude = GetMagnitude();
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		w /= magnitude;
	}
	template<typename T>
	inline void Quaternion<T>::Conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	template<typename T>
	inline Matrix4x4<T> Quaternion<T>::CreateMatrix4x4() const
	{
		Matrix4x4<T> matrix;

		Vector3<T> vecX;
		vecX.x = 1.0f - 2 * ((y * y) + (z * z));
		vecX.y = 2 * ((x * y) - (z * w));
		vecX.z = 2 * ((x * z) + (y * w));

		Vector3<T> vecY;
		vecY.x = 2 * ((x * y) + (z * w));
		vecY.y = 1 - 2 * ((x * x) + (z * z));
		vecY.z = 2 * ((y * z) - (x * w));

		Vector3<T> vecZ;
		vecZ.x = 2 * ((x * z) - (y * w));
		vecZ.y = 2 * ((y * z) + (x * w));
		vecZ.z = 1 - 2 * ((x * x) + (y * y));

		matrix[0] = vecX.x;
		matrix[1] = vecX.y;
		matrix[2] = vecX.z;

		matrix[4] = vecY.x;
		matrix[5] = vecY.y;
		matrix[6] = vecY.z;

		matrix[8] = vecZ.x;
		matrix[9] = vecZ.y;
		matrix[10] = vecZ.z;

		/*matrix[0] = 1 - (2 * y * y) - (2 * z * z);
		matrix[1] = (2 * x * y) - (2 * w * z);
		matrix[2] = (2 * x * z) - (2 * w * y);

		matrix[4] = (2 * x * y) - (2 * w * z);
		matrix[5] = 1 - (2 * x * x) - (2 * z * z);
		matrix[6] = (2 * y * z) - (2 * w * x);

		matrix[8] = (2 * x * z) - (2 * w * y);
		matrix[9] = (2 * y * z) - (2 * w * x);
		matrix[10] = 1 - (2 * x * x) - (2 * y * y);*/

		return matrix;
	}
	template<typename T>
	inline Matrix3x3<T> Quaternion<T>::CreateMatrix3x3() const
	{
		Matrix3x3<T> matrix;

		Vector3<T> vecX;
		vecX.x = 1.0f - 2 * ((y * y) + (z * z));
		vecX.y = 2 * ((x * y) - (z * w));
		vecX.z = 2 * ((x * z) + (y * w));

		Vector3<T> vecY;
		vecY.x = 2 * ((x * y) + (z * w));
		vecY.y = 1 - 2 * ((x * x) + (z * z));
		vecY.z = 2 * ((y * z) - (x * w));

		Vector3<T> vecZ;
		vecZ.x = 2 * ((x * z) - (y * w));
		vecZ.y = 2 * ((y * z) + (x * w));
		vecZ.z = 1 - 2 * ((x * x) + (y * y));

		matrix[0] = vecX.x;
		matrix[1] = vecX.y;
		matrix[2] = vecX.z;

		matrix[3] = vecY.x;
		matrix[4] = vecY.y;
		matrix[5] = vecY.z;

		matrix[6] = vecZ.x;
		matrix[7] = vecZ.y;
		matrix[8] = vecZ.z;

		/*matrix[0] = 1 - (2 * y * y) - (2 * z * z);
		matrix[1] = (2 * x * y) - (2 * w * z);
		matrix[2] = (2 * x * z) - (2 * w * y);

		matrix[3] = (2 * x * y) - (2 * w * z);
		matrix[4] = 1 - (2 * x * x) - (2 * z * z);
		matrix[5] = (2 * y * z) - (2 * w * x);

		matrix[6] = (2 * x * z) - (2 * w * y);
		matrix[7] = (2 * y * z) - (2 * w * x);
		matrix[8] = 1 - (2 * x * x) - (2 * y * y);*/

		return matrix;
	}

	template<typename T>
	inline Vector3<T> Quaternion<T>::GetVectorX() const
	{
		Vector3<T> vector;
		vector.x = 1.0f - 2 * ((y * y) + (z * z));
		vector.y = 2 * ((x * y) - (z * w));
		vector.z = 2 * ((x * z) + (y * w));
		return vector;
	}
	template<typename T>
	inline Vector3<T> Quaternion<T>::GetVectorY() const
	{
		Vector3<T> vector;
		vector.x = 2 * ((x * y) + (z * w));
		vector.y = 1 - 2 * ((x * x) + (z * z));
		vector.z = 2 * ((y * z) - (x * w));
		return vector;
	}
	template<typename T>
	inline Vector3<T> Quaternion<T>::GetVectorZ() const
	{
		Vector3<T> vector;
		vector.x = 2 * ((x * z) - (y * w));
		vector.y = 2 * ((y * z) + (x * w));
		vector.z = 1 - 2 * ((x * x) + (y * y));
		return vector;
	}

	template<typename T>
	inline Vector3<T> Quaternion<T>::GetEulerAngles() const
	{
		//code from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		CU::Vector3<T> eulerAngles;

		// roll (x-axis rotation)
		const T sinr_cosp = 2 * (w * x + y * z);
		const T cosr_cosp = 1 - 2 * (x * x + y * y);
		eulerAngles.x = static_cast<T>(atan2(sinr_cosp, cosr_cosp));

		// pitch (y-axis rotation)
		const T sinp = 2 * (w * y - z * x);
		if (fabs(sinp) >= 1.0f)
		{
			eulerAngles.y = static_cast<T>(copysign(CU::Math::PI * 0.5, sinp)); // use 90 degrees if out of range
		}
		else
		{
			eulerAngles.y = static_cast<T>(asin(sinp));
		}

		// yaw (z-axis rotation)
		const T siny_cosp = 2 * (w * z + x * y);
		const T cosy_cosp = 1 - 2 * (y * y + z * z);
		eulerAngles.z = static_cast<T>(atan2(siny_cosp, cosy_cosp));

		return eulerAngles;
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::GetNormalized() const
	{
		const T magnitude = GetMagnitude();
		return Quaternion<T>(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
	}
	template<typename T>
	inline Quaternion<T> Quaternion<T>::GetConjugate() const
	{
		return Quaternion<T>(-x, -y, -z, w);
	}
	template<typename T>
	inline Quaternion<T> Quaternion<T>::GetInverse() const
	{
		return GetConjugate() / (x * x + y * y + z * z + w * w);
	}

	template<typename T>
	inline Vector3<T> Quaternion<T>::GetAxis() const
	{
		T div = sqrt(static_cast<T>(1) - (w * w));

		T threshhold = static_cast<T>(1e-4f);
		if (div < threshhold)
		{
			return Vector3<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
		}

		return Vector3<T>(x / div, y / div, z / div);
	}
	template<typename T>
	inline Vector4<T> Quaternion<T>::GetAxisAngle() const
	{
		const Vector3<T> axis(GetAxis());
		return Vector4<T>(axis.x, axis.y, axis.z, GetAngle());
	}

	template<typename T>
	inline T Quaternion<T>::GetMagnitude() const
	{
		return static_cast<T>(sqrt((x * x) + (y * y) + (z * z) + (w * w)));
	}
	template<typename T>
	inline T Quaternion<T>::GetMagnitudeSqr() const
	{
		return (x * x) + (y * y) + (z * z) + (w * w);
	}
	template<typename T>
	inline T Quaternion<T>::GetAngle() const
	{
		return acos(w) * static_cast<T>(2);
	}

	template<typename T>
	inline bool Quaternion<T>::IsUnit() const
	{
		T threshhold = static_cast<T>(1e-4f);
		return abs(static_cast<T>(1) - GetMagnitudeSqr()) < threshhold;
	}
}