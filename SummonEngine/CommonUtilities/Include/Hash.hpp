#pragma once
#include <string>

namespace CU
{
	unsigned long long Hash(const unsigned char* aBuffer, const unsigned int aCount);

	template<class T>
	unsigned long long Hash(const T& aKey)
	{
		return Hash(reinterpret_cast<const unsigned char*>(&aKey), sizeof(aKey));
	}
	unsigned long long Hash(const std::string& aKey);
}