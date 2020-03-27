#pragma once
#include <random>

namespace CU
{
	template<typename T = int>
	T RandomNumber(T aMin, T aMax)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<T> dis(aMin, aMax);

		return dis(gen);
	}

	template<typename T = float>
	T RandomDecimal(T aMin, T aMax, int aPrecision = 3)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		int prec = static_cast<int>(pow(10, aPrecision));
		std::uniform_int_distribution<int> dis(static_cast<int>(aMin * prec), static_cast<int>(aMax * prec));

		return static_cast<T>(dis(gen)) / prec;
	}
}