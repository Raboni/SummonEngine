#pragma once
#include <assert.h>

namespace CU
{
	template <typename ObjectType, int capacity>
	class SizableArray
	{
	public:
		SizableArray();
		~SizableArray() = default;
		SizableArray(const SizableArray& aArray);
		SizableArray(const int* aArrayPointer, const int aArraySize = capacity);

		SizableArray& operator=(const SizableArray& aArray);
		inline const ObjectType& operator[](const int aIndex) const;
		inline ObjectType& operator[](const int aIndex);
		
		inline ObjectType& Add();
		inline ObjectType& Add(const ObjectType& aObject);
		inline void RemoveAll();
		inline void RemoveLast();
		inline void RemoveIndex(const int aIndex);
		inline void DeleteAll();
		inline void DeleteLast();
		inline void DeleteIndex(const int aIndex);

		inline void Resize(const int aSize);

		inline int Find(const ObjectType& aObject) const;
		inline bool Contains(const ObjectType& aObject) const;

		inline ObjectType& GetLast();
		inline const ObjectType& GetLast() const;
		inline bool IsFull() const;
		__forceinline int Size() const;
		__forceinline int Capacity() const;

	private:
		ObjectType myArray[capacity];
		int mySize;
	};

	template<typename ObjectType, int capacity>
	inline SizableArray<ObjectType, capacity>::SizableArray()
	{
		mySize = 0;
	}
	template<typename ObjectType, int capacity>
	inline SizableArray<ObjectType, capacity>::SizableArray(const SizableArray& aArray)
	{
		mySize = aArray.mySize;
		if (mySize > capacity)
		{
			mySize = capacity;
		}

		for (int index = 0; index < mySize; index++)
		{
			myArray[index] = aArray.myArray[index];
		}
	}
	template<typename ObjectType, int capacity>
	inline SizableArray<ObjectType, capacity>::SizableArray(const int* aArrayPointer, const int aArraySize)
	{
		mySize = aArraySize;
		if (mySize > capacity)
		{
			mySize = capacity;
		}
		if (aArrayPointer == nullptr)
		{
			return;
		}

		for (int index = 0; index < mySize; index++)
		{
			myArray[index] = aArrayPointer[index];
		}
	}

	template<typename ObjectType, int capacity>
	inline SizableArray<ObjectType, capacity>& SizableArray<ObjectType, capacity>::operator=(const SizableArray& aArray)
	{
		mySize = aArray.mySize;
		if (mySize > capacity)
		{
			mySize = capacity;
		}

		for (int index = 0; index < mySize; index++)
		{
			myArray[index] = aArray.myArray[index];
		}
		return *this;
	}
	template<typename ObjectType, int capacity>
	inline const ObjectType& SizableArray<ObjectType, capacity>::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < capacity);
		if (aIndex < 0 || aIndex >= capacity)
		{
			return myArray[0];
		}

		return myArray[aIndex];
	}
	template<typename ObjectType, int capacity>
	inline ObjectType& SizableArray<ObjectType, capacity>::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < capacity);
		if (aIndex < 0 || aIndex >= capacity)
		{
			return myArray[0];
		}

		return myArray[aIndex];
	}

	template<typename ObjectType, int capacity>
	inline ObjectType& SizableArray<ObjectType, capacity>::Add()
	{
		assert(mySize < capacity);
		if (mySize >= capacity)
		{
			return myArray[capacity - 1];
		}

		mySize++;
		return myArray[mySize - 1];
	}
	template<typename ObjectType, int capacity>
	inline ObjectType& SizableArray<ObjectType, capacity>::Add(const ObjectType& aObject)
	{
		assert(mySize < capacity);
		if (mySize >= capacity)
		{
			return myArray[capacity - 1];
		}

		mySize++;
		myArray[mySize - 1] = aObject;
		return myArray[mySize - 1];
	}

	template<typename ObjectType, int capacity>
	inline void SizableArray<ObjectType, capacity>::RemoveAll()
	{
		mySize = 0;
	}
	template<typename ObjectType, int capacity>
	inline void SizableArray<ObjectType, capacity>::RemoveLast()
	{
		if (mySize <= 0)
		{
			return;
		}

		mySize--;
	}
	template<typename ObjectType, int capacity>
	inline void SizableArray<ObjectType, capacity>::RemoveIndex(const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize);
		if (aIndex < 0 || aIndex >= mySize)
		{
			return;
		}

		for (int index = aIndex; index < mySize - 1; index++)
		{
			myArray[index] = myArray[index + 1];
		}
		mySize--;
	}

	template<typename ObjectType, int capacity>
	inline void SizableArray<ObjectType, capacity>::DeleteAll()
	{
		if (mySize <= 0)
		{
			return;
		}

		for (int index = 0; index < mySize; index++)
		{
			delete myArray[index];
			myArray[index] = nullptr;
		}
		RemoveAll();
	}
	template<typename ObjectType, int capacity>
	inline void SizableArray<ObjectType, capacity>::DeleteLast()
	{
		if (mySize <= 0)
		{
			return;
		}

		mySize--;
		delete myArray[mySize];
		myArray[mySize] = nullptr;
	}
	template<typename ObjectType, int capacity>
	inline void SizableArray<ObjectType, capacity>::DeleteIndex(const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize);
		if (aIndex < 0 || aIndex >= mySize)
		{
			return;
		}
		if (aIndex == mySize - 1)
		{
			DeleteLast();
			return;
		}

		delete myArray[aIndex];
		myArray[aIndex] = nullptr;
		RemoveIndex(aIndex);
		myArray[mySize] = nullptr;
	}
	
	template<typename ObjectType, int capacity>
	inline void SizableArray<ObjectType, capacity>::Resize(const int aSize)
	{
		assert(aSize >= 0 && aSize <= capacity);
		if (aSize < 0 || aSize > capacity)
		{
			return;
		}

		mySize = aSize;
	}

	template<typename ObjectType, int capacity>
	inline int SizableArray<ObjectType, capacity>::Find(const ObjectType& aObject) const
	{
		for (int index = 0; index < mySize; index++)
		{
			if (myArray[index] == aObject)
			{
				return index;
			}
		}

		return -1;
	}
	template<typename ObjectType, int capacity>
	inline bool SizableArray<ObjectType, capacity>::Contains(const ObjectType& aObject) const
	{
		for (int index = 0; index < mySize; index++)
		{
			if (myArray[index] == aObject)
			{
				return true;
			}
		}

		return false;
	}

	template<typename ObjectType, int capacity>
	inline ObjectType& SizableArray<ObjectType, capacity>::GetLast()
	{
		assert(mySize > 0 && mySize <= capacity);
		if (mySize <= 0 || mySize > capacity)
		{
			return myArray[0];
		}

		return myArray[mySize - 1];
	}
	template<typename ObjectType, int capacity>
	inline const ObjectType& SizableArray<ObjectType, capacity>::GetLast() const
	{
		assert(mySize > 0 && mySize <= capacity);
		if (mySize <= 0 || mySize > capacity)
		{
			return myArray[0];
		}

		return myArray[mySize - 1];
	}

	template<typename ObjectType, int capacity>
	inline bool SizableArray<ObjectType, capacity>::IsFull() const
	{
		return mySize >= capacity;
	}
	template<typename ObjectType, int capacity>
	__forceinline int SizableArray<ObjectType, capacity>::Size() const
	{
		return mySize;
	}
	template<typename ObjectType, int capacity>
	inline int SizableArray<ObjectType, capacity>::Capacity() const
	{
		return capacity;
	}
}