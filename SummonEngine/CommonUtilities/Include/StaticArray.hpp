#pragma once
#include "Macro.hpp"
#include <initializer_list>
#include <assert.h>

namespace CU
{
	template <typename Type, int size>
	class StaticArray
	{
	public:
		StaticArray() = default;
		~StaticArray() = default;
		StaticArray(const StaticArray& aStaticArray);
		StaticArray(const std::initializer_list<Type>& aInitList);
		StaticArray(const int* aArrayPointer, const int aArraySize = size);
		StaticArray& operator=(const StaticArray& aStaticArray);
		inline const Type& operator[](const int aIndex) const;
		inline Type& operator[](const int aIndex);
		// Utility functions
		inline void Insert(int aIndex, const Type& aObject);
		inline void DeleteAll();
		inline void Delete(int aIndex);
		__forceinline int Size() const;
	private:
		Type myArray[size];
	};

	template<typename Type, int size>
	inline StaticArray<Type, size>::StaticArray(const StaticArray& aStaticArray)
	{
		assert(aStaticArray.Size() <= size && "too large size");
		const int minSize = MIN(aStaticArray.Size(), size);
		for (int index = 0; index < minSize; index++)
		{
			myArray[index] = aStaticArray[index];
		}
	}
	template<typename Type, int size>
	inline StaticArray<Type, size>::StaticArray(const std::initializer_list<Type>& aInitList)
	{
		assert(aInitList.size() <= size && "too large size");
		const int minSize = MIN(aInitList.size(), size);
		for (int index = 0; index < minSize; index++)
		{
			myArray[index] = *(aInitList.begin() + index);
		}
	}
	template<typename Type, int size>
	inline StaticArray<Type, size>::StaticArray(const int* aArrayPointer, const int aArraySize)
	{
		assert(aArraySize <= size && "too large size");
		const int minSize = MIN(aArraySize, size);
		for (int index = 0; index < minSize; index++)
		{
			myArray[index] = aArrayPointer[index];
		}
	}
	template<typename Type, int size>
	inline StaticArray<Type, size>& StaticArray<Type, size>::operator=(const StaticArray& aStaticArray)
	{
		assert(aStaticArray.Size() <= size && "too large size");
		const int minSize = MIN(aStaticArray.Size(), size);
		for (int index = 0; index < minSize; index++)
		{
			myArray[index] = aStaticArray[index];
		}
		return *this;
	}
	template<typename Type, int size>
	inline const Type& StaticArray<Type, size>::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < size && "out of scope");
		if (aIndex >= 0 && aIndex < size)
		{
			return myArray[aIndex];
		}
		else
		{
			return myArray[0];
		}
	}
	template<typename Type, int size>
	inline Type& StaticArray<Type, size>::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < size && "out of scope");
		if (aIndex >= 0 && aIndex < size)
		{
			return myArray[aIndex];
		}
		else
		{
			return myArray[0];
		}
	}
	template<typename Type, int size>
	inline void StaticArray<Type, size>::Insert(int aIndex, const Type& aObject)
	{
		assert(aIndex >= 0 && aIndex < size && "out of scope");
		if (aIndex >= 0 && aIndex < size)
		{
			for (int index = size - 1; index > aIndex; index--)
			{
				myArray[index] = myArray[index - 1];
			}
			myArray[aIndex] = aObject;
		}
	}
	template<typename Type, int size>
	inline void StaticArray<Type, size>::DeleteAll()
	{
		for (int index = 0; index < size; index++)
		{
			SAFE_DELETE(myArray[index]);
		}
	}
	template<typename Type, int size>
	inline void StaticArray<Type, size>::Delete(int aIndex)
	{
		assert(aIndex >= 0 && aIndex < size && "out of scope");
		if (aIndex >= 0 && aIndex < size)
		{
			SAFE_DELETE(myArray[aIndex]);
		}
	}
	template<typename Type, int size>
	__forceinline int StaticArray<Type, size>::Size() const
	{
		return size;
	}
}