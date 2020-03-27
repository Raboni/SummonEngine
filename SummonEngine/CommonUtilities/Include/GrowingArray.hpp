#pragma once
#include <assert.h>
#include <string>

namespace CU
{
	template<typename ObjectType, typename SizeType = int>
	class GrowingArray
	{
	public:
		GrowingArray();
		GrowingArray(ObjectType* aArrayPointer, const SizeType aSize, const bool aUseSafeModeFlag = true);
		GrowingArray(const SizeType aCapacity, const bool aUseSafeModeFlag = true);
		GrowingArray(const GrowingArray& aGrowingArray);
		GrowingArray(GrowingArray&& aGrowingArray);
		~GrowingArray();

		GrowingArray& operator=(const GrowingArray& aGrowingArray);
		GrowingArray& operator=(GrowingArray&& aGrowingArray);
		GrowingArray<ObjectType, SizeType> operator+(const GrowingArray<ObjectType, SizeType>& aGrowingArray);
		void operator+=(const GrowingArray<ObjectType, SizeType>& aGrowingArray);
		void Init(const SizeType aCapacity, const bool aUseSafeModeFlag = true);
		inline ObjectType& operator[](const SizeType aIndex);
		inline const ObjectType& operator[](const SizeType aIndex) const;

		inline ObjectType& Add();
		inline ObjectType& Add(const ObjectType& aObject);
		inline void Insert(const SizeType aIndex, const ObjectType& aObject);
		inline void Switch(const SizeType aIndex1, const SizeType aIndex2);
		inline void Reverse();

		inline bool DeleteCyclic(const ObjectType& aObject);
		inline void DeleteCyclicAtIndex(const SizeType aIndex);
		inline void DeleteAtIndex(const SizeType aIndex);
		inline void DeleteLast();
		inline void DeleteDuplicates();
		inline void DeleteAll();

		inline bool RemoveCyclic(const ObjectType& aObject);
		inline void RemoveCyclicAtIndex(const SizeType aIndex);
		inline bool Remove(const ObjectType& aObject);
		inline void RemoveAtIndex(const SizeType aIndex);
		inline void RemoveLast();
		inline void RemoveDuplicates();
		inline void RemoveAll();

		inline void Resize(const SizeType aNewSize);
		inline void Reserve(const SizeType aNewSize);
		inline void Fill();
		inline void Fill(const ObjectType& aObject);
		inline void Optimize();
		inline void Grow();

		inline SizeType Find(const ObjectType& aObject) const;
		inline bool Contains(const ObjectType& aObject) const;

		inline ObjectType* GetPointer();
		inline const ObjectType* GetPointer() const;
		inline ObjectType& GetLast();
		inline const ObjectType& GetLast() const;
		__forceinline SizeType Size() const;
		__forceinline SizeType Capacity() const;
		__forceinline bool IsInitialized() const;
		static const SizeType FoundNone = static_cast<SizeType>(-1);
		
	private:
		ObjectType* myArray;
		SizeType mySize;
		SizeType myCapacity;
		bool mySafeMode;
	};

	template<typename ObjectType, typename SizeType>
	GrowingArray<ObjectType, SizeType> GrowingArray<ObjectType, SizeType>::operator+(const GrowingArray<ObjectType, SizeType>& aGrowingArray)
	{
		GrowingArray<ObjectType, SizeType> temp;

		assert(myArray != nullptr && aGrowingArray.IsInitialized() && "init needed");
		if (myArray != nullptr && aGrowingArray.IsInitialized())
		{
			SizeType size = mySize + aGrowingArray.Size();

			temp.myArray = new ObjectType[size];
			for (SizeType index = static_cast<SizeType>(0); index < mySize; index++)
			{
				temp.myArray[index] = myArray[index];
			}
			for (SizeType index = mySize; index < size; index++)
			{
				temp.myArray[index] = aGrowingArray.myArray[index - mySize];
			}

			temp.mySize = size;
			temp.myCapacity = size;
			temp.mySafeMode = mySafeMode && aGrowingArray.mySafeMode;
		}

		return temp;
	}
	template<typename ObjectType, typename SizeType>
	void GrowingArray<ObjectType, SizeType>::operator+=(const GrowingArray<ObjectType, SizeType>& aGrowingArray)
	{
		assert(myArray != nullptr && aGrowingArray.IsInitialized() && "init needed");
		if (myArray != nullptr && aGrowingArray.IsInitialized())
		{
			SizeType size = mySize + aGrowingArray.Size();

			ObjectType* tempArray = myArray;
			myArray = new ObjectType[size];
			for (SizeType index = static_cast<SizeType>(0); index < mySize; index++)
			{
				myArray[index] = tempArray[index];
			}
			delete tempArray;
			for (SizeType index = static_cast<SizeType>(0); index < aGrowingArray.Size(); index++)
			{
				myArray[mySize + index] = aGrowingArray[index];
			}

			mySize = size;
			myCapacity = size;
			mySafeMode = mySafeMode && aGrowingArray.mySafeMode;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>::GrowingArray()
	{
		myArray = nullptr;
		mySize = 0;
		myCapacity = 0;
		mySafeMode = true;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>::GrowingArray(ObjectType* aArrayPointer, const SizeType aSize, const bool aUseSafeModeFlag)
	{
		mySize = aSize;
		myCapacity = aSize;
		mySafeMode = aUseSafeModeFlag;

		if (aArrayPointer != nullptr)
		{
			myArray = new ObjectType[aSize];
			if (mySafeMode)
			{
				for (SizeType index = 0; index < aSize; index++)
				{
					myArray[index] = aArrayPointer[index];
				}
			}
			else
			{
				memcpy(myArray, aArrayPointer, sizeof(ObjectType) * mySize);
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>::GrowingArray(const SizeType aCapacity, const bool aUseSafeModeFlag)
	{
		myArray = new ObjectType[aCapacity];
		mySize = 0;
		myCapacity = aCapacity;
		mySafeMode = aUseSafeModeFlag;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>::GrowingArray(const GrowingArray& aGrowingArray)
	{
		mySize = aGrowingArray.mySize;
		myCapacity = aGrowingArray.myCapacity;
		mySafeMode = aGrowingArray.mySafeMode;

		if (aGrowingArray.myArray != nullptr)
		{
			myArray = new ObjectType[myCapacity];
			if (mySafeMode)
			{
				for (SizeType index = 0; index < mySize; index++)
				{
					myArray[index] = aGrowingArray.myArray[index];
				}
			}
			else
			{
				memcpy(myArray, aGrowingArray.myArray, sizeof(ObjectType) * mySize);
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>::GrowingArray(GrowingArray&& aGrowingArray)
	{
		mySize = aGrowingArray.mySize;
		myCapacity = aGrowingArray.myCapacity;
		mySafeMode = aGrowingArray.mySafeMode;

		myArray = aGrowingArray.myArray;
		aGrowingArray.myArray = nullptr;
		aGrowingArray.~GrowingArray();
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>::~GrowingArray()
	{
		mySize = static_cast<SizeType>(0);
		myCapacity = static_cast<SizeType>(0);

		if (myArray != nullptr)
		{
			delete[] myArray;
			myArray = nullptr;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>& GrowingArray<ObjectType, SizeType>::operator=(const GrowingArray& aGrowingArray)
	{
		mySize = aGrowingArray.mySize;
		myCapacity = aGrowingArray.myCapacity;
		mySafeMode = aGrowingArray.mySafeMode;

		if (myArray != nullptr)
		{
			delete[] myArray;
		}
		if (aGrowingArray.myArray != nullptr)
		{
			myArray = new ObjectType[myCapacity];
			if (mySafeMode)
			{
				for (SizeType index = 0; index < mySize; index++)
				{
					myArray[index] = aGrowingArray.myArray[index];
				}
			}
			else
			{
				memcpy(myArray, aGrowingArray.myArray, sizeof(ObjectType) * mySize);
			}
		}
		return *this;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingArray<ObjectType, SizeType>& GrowingArray<ObjectType, SizeType>::operator=(GrowingArray&& aGrowingArray)
	{
		mySize = aGrowingArray.mySize;
		myCapacity = aGrowingArray.myCapacity;
		mySafeMode = aGrowingArray.mySafeMode;

		if (myArray != nullptr)
		{
			delete[] myArray;
		}
		myArray = aGrowingArray.myArray;
		aGrowingArray.myArray = nullptr;
		aGrowingArray.~GrowingArray();

		return *this;
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Init(const SizeType aCapacity, const bool aUseSafeModeFlag)
	{
		if (myArray != nullptr)
		{
			delete[] myArray;
		}
		myArray = new ObjectType[aCapacity];
		mySize = 0;
		myCapacity = aCapacity;
		mySafeMode = aUseSafeModeFlag;
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingArray<ObjectType, SizeType>::operator[](const SizeType aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (myCapacity > 0 && aIndex >= 0 && aIndex < mySize)
		{
			return myArray[aIndex];
		}
		else
		{
			return *myArray;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline const ObjectType& GrowingArray<ObjectType, SizeType>::operator[](const SizeType aIndex) const
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (myCapacity > 0 && aIndex >= 0 && aIndex < mySize)
		{
			return myArray[aIndex];
		}
		else
		{
			return *myArray;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingArray<ObjectType, SizeType>::Add()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			if (mySize >= myCapacity)
			{
				Grow();
			}
			mySize++;
		}
		return myArray[mySize - 1];
	}
	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingArray<ObjectType, SizeType>::Add(const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			if (mySize >= myCapacity)
			{
				Grow();
			}
			myArray[mySize] = aObject;
			mySize++;
		}
		return myArray[mySize - 1];
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Insert(const SizeType aIndex, const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex >= static_cast<SizeType>(0) && aIndex < mySize && "out of scope");
		if (myArray != nullptr && aIndex >= static_cast<SizeType>(0))
		{
			if (aIndex < mySize)
			{
				if (mySize >= myCapacity)
				{
					Grow();
				}
				if (mySafeMode)
				{
					for (SizeType index = mySize; index > aIndex; index--)
					{
						myArray[index] = myArray[index - 1];
					}
				}
				else
				{
					memcpy(&myArray[aIndex + 1], &myArray[aIndex], sizeof(ObjectType) * (mySize - aIndex));
				}
				mySize++;
				myArray[aIndex] = aObject;
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Switch(const SizeType aIndex1, const SizeType aIndex2)
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex1 >= static_cast<SizeType>(0) && aIndex1 < mySize && "aIndex1 out of scope");
		assert(aIndex2 >= static_cast<SizeType>(0) && aIndex2 < mySize && "aIndex2 out of scope");
		if (myArray != nullptr)
		{
			ObjectType temp = myArray[aIndex1];
			myArray[aIndex1] = myArray[aIndex2];
			myArray[aIndex2] = temp;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Reverse()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			for (SizeType index = 0; index < mySize / static_cast<SizeType>(2); index++)
			{
				Switch(index, mySize - 1 - index);
			}
		}
	}

	template<typename ObjectType, typename SizeType>
	inline bool GrowingArray<ObjectType, SizeType>::DeleteCyclic(const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		if (myCapacity > static_cast<SizeType>(0))
		{
			SizeType index = Find(aObject);
			//assert(index != FoundNone && "object not found");
			if (index != FoundNone)
			{
				delete myArray[index];
				myArray[index] = myArray[mySize - 1];
				myArray[mySize - 1] = nullptr;
				mySize--;
			}
			return index != FoundNone;
		}
		return false;
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::DeleteCyclicAtIndex(const SizeType aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > 0 && "Array is empty");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (myArray != nullptr && mySize > 0 && aIndex >= 0 && aIndex < mySize)
		{
			delete myArray[aIndex];
			myArray[aIndex] = myArray[mySize - 1];
			myArray[mySize - 1] = nullptr;
			mySize--;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::DeleteAtIndex(const SizeType aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > 0 && "Array is empty");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (mySize > 0 && aIndex >= 0 && aIndex < mySize)
		{
			delete myArray[aIndex];
			for (SizeType index = aIndex; index < mySize - 1; index++)
			{
				myArray[index] = myArray[index + 1];
			}
			myArray[mySize - 1] = nullptr;
			mySize--;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::DeleteLast()
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > 0 && "already empty");
		if (myArray != nullptr && mySize > 0)
		{
			delete myArray[mySize - 1];
			myArray[mySize - 1] = nullptr;
			mySize--;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::DeleteDuplicates()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			for (SizeType index = 0; index < mySize; index++)
			{
				for (SizeType reverseIndex = mySize - 1; reverseIndex > index; reverseIndex--)
				{
					if (myArray[index] == myArray[reverseIndex])
					{
						DeleteAtIndex(reverseIndex--);
					}
				}
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::DeleteAll()
	{
		assert(myArray != nullptr && "init needed");
		if (myCapacity > static_cast<SizeType>(0))
		{
			for (SizeType index = static_cast<SizeType>(0); index < mySize; index++)
			{
				delete myArray[index];
				myArray[index] = nullptr;
			}
		}
		mySize = static_cast<SizeType>(0);
	}

	template<typename ObjectType, typename SizeType>
	inline bool GrowingArray<ObjectType, SizeType>::RemoveCyclic(const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > static_cast<SizeType>(0) && "Array is empty");
		if (mySize > static_cast<SizeType>(0))
		{
			SizeType index = Find(aObject);
			//assert(index != FoundNone && "object not found");
			if (index != FoundNone)
			{
				myArray[index] = myArray[mySize - 1];
				mySize--;
			}
			return index != FoundNone;
		}
		return false;
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::RemoveCyclicAtIndex(const SizeType aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > static_cast<SizeType>(0) && "Array is empty");
		assert(aIndex >= static_cast<SizeType>(0) && aIndex < mySize && "out of scope");
		if (mySize > static_cast<SizeType>(0) && aIndex >= static_cast<SizeType>(0) && aIndex < mySize)
		{
			myArray[aIndex] = myArray[mySize - 1];
			mySize--;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline bool GrowingArray<ObjectType, SizeType>::Remove(const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > static_cast<SizeType>(0) && "Array is empty");
		if (mySize > static_cast<SizeType>(0))
		{
			SizeType foundIndex = Find(aObject);
			if (foundIndex != FoundNone)
			{
				for (SizeType index = foundIndex; index < mySize - 1; index++)
				{
					myArray[index] = myArray[index + 1];
				}
				mySize--;
			}
		}
		return false;
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::RemoveAtIndex(const SizeType aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > static_cast<SizeType>(0) && "Array is empty");
		assert(aIndex >= static_cast<SizeType>(0) && aIndex < mySize && "out of scope");
		if (mySize > static_cast<SizeType>(0) && aIndex >= static_cast<SizeType>(0) && aIndex < mySize)
		{
			for (SizeType index = aIndex; index < mySize - 1; index++)
			{
				myArray[index] = myArray[index + 1];
			}
			mySize--;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::RemoveLast()
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > static_cast<SizeType>(0) && "already empty");
		if (mySize > static_cast<SizeType>(0))
		{
			mySize--;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::RemoveDuplicates()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			for (SizeType index = 0; index < mySize; index++)
			{
				for (SizeType reverseIndex = mySize - 1; reverseIndex > index; reverseIndex--)
				{
					if (myArray[index] == myArray[reverseIndex])
					{
						RemoveAtIndex(reverseIndex--);
					}
				}
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::RemoveAll()
	{
		assert(myArray != nullptr && "init needed");
		mySize = static_cast<SizeType>(0);
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Resize(const SizeType aNewSize)
	{
		assert(myArray != nullptr && "init needed");
		//assert(aNewSize <= myCapacity && "capacity too low");
		if (myArray != nullptr)
		{
			if (aNewSize > myCapacity)
			{
				Reserve(aNewSize);
			}
			mySize = aNewSize;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Reserve(const SizeType aNewSize)
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			myCapacity = aNewSize;
			ObjectType* temp = new ObjectType[aNewSize];
			if (mySize > aNewSize)
			{
				mySize = aNewSize;
			}

			if (mySafeMode)
			{
				for (SizeType index = static_cast<SizeType>(0); index < mySize; index++)
				{
					temp[index] = myArray[index];
				}
			}
			else
			{
				memcpy(temp, myArray, sizeof(ObjectType) * mySize);
			}
			delete[] myArray;
			myArray = temp;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Fill()
	{
		assert(myArray != nullptr && "init needed");
		mySize = myCapacity;
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Fill(const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			for (SizeType index = mySize; index < myCapacity; index++)
			{
				myArray[index] = aObject;
			}
			mySize = myCapacity;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Optimize()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			Reserve(mySize);
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingArray<ObjectType, SizeType>::Grow()
	{
		assert(myArray != nullptr && "init needed");
		if (myCapacity > static_cast<SizeType>(0))
		{
			Reserve(myCapacity * 2);
		}
		else
		{
			Reserve(1);
		}
	}

	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingArray<ObjectType, SizeType>::Find(const ObjectType& aObject) const
	{
		assert(myArray != nullptr && "init needed");
		if (myCapacity <= static_cast<SizeType>(0))
		{
			return FoundNone;
		}

		for (SizeType index = static_cast<SizeType>(0); index < mySize; index++)
		{
			if (myArray[index] == aObject)
			{
				return index;
			}
		}

		return FoundNone;
	}
	template<typename ObjectType, typename SizeType>
	inline bool GrowingArray<ObjectType, SizeType>::Contains(const ObjectType& aObject) const
	{
		assert(myArray != nullptr && "init needed");
		if (myCapacity <= static_cast<SizeType>(0))
		{
			return false;
		}

		for (SizeType index = static_cast<SizeType>(0); index < mySize; index++)
		{
			if (myArray[index] == aObject)
			{
				return true;
			}
		}

		return false;
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType* GrowingArray<ObjectType, SizeType>::GetPointer()
	{
		return myArray;
	}
	template<typename ObjectType, typename SizeType>
	inline const ObjectType* GrowingArray<ObjectType, SizeType>::GetPointer() const
	{
		return myArray;
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingArray<ObjectType, SizeType>::GetLast()
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > static_cast<SizeType>(0) && "empty vector");
		if (myCapacity > static_cast<SizeType>(0) && mySize > static_cast<SizeType>(0))
		{
			return myArray[mySize - 1];
		}
		else
		{
			return *myArray;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline const ObjectType& GrowingArray<ObjectType, SizeType>::GetLast() const
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > static_cast<SizeType>(0) && "empty vector");
		if (myCapacity > static_cast<SizeType>(0) && mySize > static_cast<SizeType>(0))
		{
			return myArray[mySize - 1];
		}
		else
		{
			return *myArray;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingArray<ObjectType, SizeType>::Size() const
	{
		return mySize;
	}
	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingArray<ObjectType, SizeType>::Capacity() const
	{
		return myCapacity;
	}
	template<typename ObjectType, typename SizeType>
	__forceinline bool GrowingArray<ObjectType, SizeType>::IsInitialized() const
	{
		return (myArray != nullptr);
	}
};