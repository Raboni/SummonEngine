#pragma once
#include <string>

template<typename T>
void Serialize(const T& aValue, char* aData, unsigned short& aDataOffset);
void Serialize(const std::string& aValue, char* aData, unsigned short& aDataOffset);

template<typename T>
void Deserialize(T& aValue, char* aData, unsigned short& aDataOffset);
void Deserialize(std::string& aValue, char* aData, unsigned short& aDataOffset);

inline void Serialize(const std::string& aValue, char* aData, unsigned short& aDataOffset)
{
	new(&aData[aDataOffset]) int(static_cast<int>(aValue.length()));
	memcpy(&aData[aDataOffset + sizeof(int)], aValue.c_str(), aValue.length());
	aDataOffset += static_cast<unsigned short>(aValue.length() + sizeof(int));
}

template<typename T>
inline void Serialize(const T& aValue, char* aData, unsigned short& aDataOffset)
{
	memcpy(&aData[aDataOffset], &aValue, sizeof(aValue));
	aDataOffset += sizeof(aValue);
}

inline void Deserialize(std::string& aValue, char* aData, unsigned short& aDataOffset)
{
	int size;
	memcpy(&size, &aData[aDataOffset], sizeof(int));
	aValue.assign(&aData[aDataOffset + sizeof(int)], size);
	aDataOffset += static_cast<unsigned short>(sizeof(int) + size);
}

template<typename T>
inline void Deserialize(T& aValue, char* aData, unsigned short& aDataOffset)
{
	memcpy(&aValue, &aData[aDataOffset], sizeof(aValue));
	aDataOffset += sizeof(aValue);
}