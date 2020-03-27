#pragma once
#include "Macro.hpp"
#include <initializer_list>
#include <cassert>
#include <cstring>

namespace CU
{
	template <typename Type, int Capacity = 50, typename CountType = unsigned short, bool UseSafeModeFlag = true>
	class VectorOnStack
	{
	public:
		VectorOnStack() = default;
		~VectorOnStack() = default;
		VectorOnStack(const VectorOnStack& aVectorOnStack);
		VectorOnStack(const std::initializer_list<Type>& aInitList);
		VectorOnStack& operator=(const VectorOnStack& aVectorOnStack);
		inline const Type& operator[](const CountType& aIndex) const;
		inline Type& operator[](const CountType& aIndex);
		inline void Add(const Type& aObject);
		inline void Insert(CountType aIndex, const Type& aObject);
		inline void DeleteCyclic(const Type& aObject);
		inline void DeleteCyclicAtIndex(CountType aItemNumber);
		inline void RemoveCyclic(const Type& aObject);
		inline void RemoveCyclicAtIndex(CountType aItemNumber);
		inline void Clear();
		inline void DeleteAll();
		__forceinline CountType Size() const;
		__forceinline CountType GetCapacity() const;
		inline Type& GetLast();
	private:
		Type myArray[Capacity];
		CountType mySize = 0;
	};

	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::VectorOnStack(const VectorOnStack& aVectorOnStack)
	{
		if (UseSafeModeFlag)
		{
			mySize = aVectorOnStack.Size();
			for (CountType index = 0; index < mySize; index++)
			{
				myArray[index] = aVectorOnStack[index];
			}
		}
		else
		{
			std::memcpy(myArray, aVectorOnStack.myArray, sizeof(Type) * aVectorOnStack.Size());
			mySize = aVectorOnStack.Size();
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::VectorOnStack(const std::initializer_list<Type>& aInitList)
	{
		CountType otherSize = aInitList.size();
		assert(otherSize <= Capacity && "vector capacity too low");
		if (otherSize <= Capacity)
		{
			mySize = otherSize;
		}
		else
		{
			mySize = Capacity;
		}
		for (CountType index = 0; index < mySize; index++)
		{
			myArray[index] = *(aInitList.begin() + index);
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>& VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::operator=(const VectorOnStack& aVectorOnStack)
	{
		if (UseSafeModeFlag)
		{
			mySize = aVectorOnStack.Size();
			for (CountType index = 0; index < mySize; index++)
			{
				myArray[index] = aVectorOnStack[index];
			}
		}
		else
		{
			std::memcpy(myArray, aVectorOnStack.myArray, sizeof(Type) * aVectorOnStack.Size());
			mySize = aVectorOnStack.Size();
		}
		return *this;
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline const Type& VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::operator[](const CountType& aIndex) const
	{
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (aIndex >= 0 && aIndex < mySize)
		{
			return myArray[aIndex];
		}
		else
		{
			return myArray[0];
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline Type& VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::operator[](const CountType& aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (aIndex >= 0 && aIndex < mySize)
		{
			return myArray[aIndex];
		}
		else
		{
			return myArray[0];
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::Add(const Type& aObject)
	{
		assert(mySize < Capacity && "vector full");
		if (mySize < Capacity)
		{
			myArray[mySize] = aObject;
			mySize++;
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::Insert(CountType aIndex, const Type& aObject)
	{
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		assert(mySize < Capacity && "vector full");
		if (aIndex >= 0 && aIndex < mySize)
		{
			if (mySize < Capacity)
			{
				for (CountType index = mySize; index > aIndex; index--)
				{
					myArray[index] = myArray[index - 1];
				}
				myArray[aIndex] = aObject;
				mySize++;
			}
			else
			{
				for (CountType index = mySize - 1; index > aIndex; index--)
				{
					myArray[index] = myArray[index - 1];
				}
				myArray[aIndex] = aObject;
			}
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::DeleteCyclic(const Type& aObject)
	{
		for (CountType index = 0; index < mySize; index++)
		{
			if (myArray[index] == aObject)
			{
				SAFE_DELETE(myArray[index]);
				myArray[index] = myArray[mySize - 1];
				mySize--;
				break;
			}
			else if (index >= mySize - 1)
			{
				assert(false && "object not found");
			}
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::DeleteCyclicAtIndex(CountType aItemNumber)
	{
		assert(aItemNumber >= 0 && aItemNumber < mySize && "out of scope");
		if (aItemNumber >= 0 && aItemNumber < mySize)
		{
			SAFE_DELETE(myArray[aItemNumber]);
			myArray[aItemNumber] = myArray[mySize - 1];
			mySize--;
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::RemoveCyclic(const Type& aObject)
	{
		for (CountType index = 0; index < mySize; index++)
		{
			if (myArray[index] == aObject)
			{
				myArray[index] = myArray[mySize - 1];
				mySize--;
				break;
			}
			else if (index >= mySize - 1)
			{
				assert(false && "object not found");
			}
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::RemoveCyclicAtIndex(CountType aItemNumber)
	{
		assert(aItemNumber >= 0 && aItemNumber < mySize && "out of scope");
		if (aItemNumber >= 0 && aItemNumber < mySize)
		{
			myArray[aItemNumber] = myArray[mySize - 1];
			mySize--;
		}
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::Clear()
	{
		mySize = 0;
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::DeleteAll()
	{
		for (CountType index = 0; index < mySize; index++)
		{
			SAFE_DELETE(myArray[index]);
		}
		mySize = 0;
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	__forceinline CountType VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::Size() const
	{
		return mySize;
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	__forceinline CountType VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::GetCapacity() const
	{
		return Capacity;
	}
	template<typename Type, int Capacity, typename CountType, bool UseSafeModeFlag>
	inline Type& VectorOnStack<Type, Capacity, CountType, UseSafeModeFlag>::GetLast()
	{
		assert(mySize > 0 && "empty vector");
		if (mySize > 0)
		{
			return myArray[mySize - 1];
		}
		else
		{
			return myArray[0];
		}
	}
}

namespace CommonUtilities = CU;