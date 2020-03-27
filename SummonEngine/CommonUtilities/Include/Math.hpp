#pragma once
#include <math.h>

namespace CU
{
	class Math
	{
	public:
		static const double PI;
		static double ToDegrees(double radians);
		static double ToRadians(double degrees);
		template<typename T>
		static T Clamp(T aValue, const T aMin, const T aMax);
	private:
		Math() = default;
		~Math() = default;
	};

	template<typename T>
	inline T Math::Clamp(T aValue, const T aMin, const T aMax)
	{
		if (aValue < aMin)
		{
			aValue = aMin;
		}
		else if (aValue > aMax)
		{
			aValue = aMax;
		}
		return aValue;
	}
}