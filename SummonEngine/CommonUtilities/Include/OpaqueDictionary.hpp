#pragma once
#include "HashMap.hpp"
#include <new>

namespace CU
{
	template<typename Key>
	class OpaqueDictionary
	{
	public:
		OpaqueDictionary(const unsigned int aCapacity);
		OpaqueDictionary(const OpaqueDictionary& aOpaqueDictionary);
		~OpaqueDictionary();
		OpaqueDictionary& operator=(const OpaqueDictionary& aOpaqueDictionary);

		template<class T>
		const bool SetValue(const Key& aKey, const T& aValue);
		template<class T>
		T* GetValue(const Key& aKey);

		void Clear();

		const bool HasKey(const Key& aKey) const;
		const unsigned int GetEntryCount() const;

	private:
		HashMap<Key, unsigned int> myIndexMap;
		char* myData;
		unsigned int myCapacity;
		unsigned int myEntryCount;
		unsigned int myIndex;
	};

	template<typename Key>
	template<class T>
	inline const bool OpaqueDictionary<Key>::SetValue(const Key& aKey, const T& aValue)
	{
		unsigned int* index = myIndexMap.Get(aKey);
		if (index == nullptr)
		{
			unsigned int newSize = sizeof(aValue);
			if (myIndex + newSize > myCapacity)
			{
				return false;
			}
			new(&myData[myIndex]) T(aValue);
			if (!myIndexMap.Insert(aKey, myIndex))
			{
				return false;
			}
			myIndex += newSize;
			myEntryCount++;
		}
		else
		{
			new(&myData[(*index)]) T(aValue);
		}
		return true;
	}
	template<typename Key>
	template<class T>
	inline T* OpaqueDictionary<Key>::GetValue(const Key& aKey)
	{
		unsigned int* index = myIndexMap.Get(aKey);
		if (index != nullptr)
		{
			return reinterpret_cast<T*>(&myData[*index]);
		}
		return nullptr;
	}
	template<typename Key>
	inline void OpaqueDictionary<Key>::Clear()
	{
		myIndexMap.RemoveAll();
		memset(myData, 0, myCapacity);
		myEntryCount = 0;
		myIndex = 0;
	}
	template<typename Key>
	inline const bool OpaqueDictionary<Key>::HasKey(const Key& aKey) const
	{
		return myIndexMap.Get(aKey) != nullptr;
	}
	template<typename Key>
	inline const unsigned int OpaqueDictionary<Key>::GetEntryCount() const
	{
		return myEntryCount;
	}
	template<typename Key>
	inline OpaqueDictionary<Key>::OpaqueDictionary(const unsigned int aCapacity) : myCapacity(aCapacity), myIndexMap(aCapacity)
	{
		myData = new char[myCapacity];
		myEntryCount = 0;
		myIndex = 0;
	}
	template<typename Key>
	inline OpaqueDictionary<Key>::OpaqueDictionary(const OpaqueDictionary<Key>& aOpaqueDictionary) : myCapacity(aOpaqueDictionary.myCapacity), myIndexMap(aOpaqueDictionary.myIndexMap)
	{
		myData = new char[myCapacity];
		memcpy(myData, aOpaqueDictionary.myData, myCapacity);
		myEntryCount = aOpaqueDictionary.myEntryCount;
		myIndex = aOpaqueDictionary.myIndex;
	}
	template<typename Key>
	inline OpaqueDictionary<Key>::~OpaqueDictionary()
	{
		Clear();
		delete[] myData;
	}
	template<typename Key>
	inline OpaqueDictionary<Key>& OpaqueDictionary<Key>::operator=(const OpaqueDictionary<Key>& aOpaqueDictionary)
	{
		if (myCapacity != aOpaqueDictionary.myCapacity)
		{
			myCapacity = aOpaqueDictionary.myCapacity;
			delete[] myData;
			myData = new char[myCapacity];
		}
		memcpy(myData, aOpaqueDictionary.myData, myCapacity);
		myIndexMap = aOpaqueDictionary.myIndexMap;
		myEntryCount = aOpaqueDictionary.myEntryCount;
		myIndex = aOpaqueDictionary.myIndex;
		return *this;
	}
}