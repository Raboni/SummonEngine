#pragma once
#include <assert.h>
#include <string>

namespace CU
{
	template<typename ObjectType, typename SizeType = int>
	class GrowingQueue
	{
	public:
		GrowingQueue();
		GrowingQueue(ObjectType* aArrayPointer, const SizeType aSize, const bool aUseSafeModeFlag = true);
		GrowingQueue(const SizeType aCapacity, const bool aUseSafeModeFlag = true);
		GrowingQueue(const GrowingQueue& aGrowingQueue);
		GrowingQueue(GrowingQueue&& aGrowingQueue);
		~GrowingQueue();

		GrowingQueue& operator=(const GrowingQueue& aGrowingQueue);
		GrowingQueue& operator=(GrowingQueue&& aGrowingQueue);
		GrowingQueue<ObjectType, SizeType> operator+(const GrowingQueue<ObjectType, SizeType>& aGrowingQueue);
		void operator+=(const GrowingQueue<ObjectType, SizeType>& aGrowingQueue1);
		void Init(const SizeType aCapacity, const bool aUseSafeModeFlag = true);
		inline ObjectType& operator[](const SizeType& aIndex);
		inline const ObjectType& operator[](const SizeType& aIndex) const;

		inline ObjectType& Add();
		inline ObjectType& Add(const ObjectType& aObject);
		inline void Insert(const SizeType aIndex, const ObjectType& aObject);
		inline void Switch(const SizeType aIndex1, const SizeType aIndex2);

		inline void DeleteLast();
		inline void DeleteAtIndex(const SizeType aIndex);
		inline void DeleteDuplicates();
		inline void DeleteAll();

		inline void RemoveLast();
		inline void RemoveAtIndex(const SizeType aIndex);
		inline void RemoveDuplicates();
		inline void RemoveAll();

		inline void Resize(const SizeType aNewSize);
		inline void Reserve(const SizeType aCapacity);
		inline void Optimize();
		inline void Grow();

		inline ObjectType* GetPointer();
		inline const ObjectType* GetPointer() const;
		inline SizeType Find(const ObjectType& aObject);
		inline ObjectType& GetLast();
		inline const ObjectType& GetLast() const;
		__forceinline SizeType Size() const;
		__forceinline SizeType Capacity() const;
		__forceinline bool IsInitialized() const;
		static const SizeType FoundNone = static_cast<SizeType>(-1);
		
	private:
		ObjectType* myArray;
		SizeType myStart;
		SizeType mySize;
		SizeType myCapacity;
		bool mySafeMode;
	};

	template<typename ObjectType, typename SizeType>
	GrowingQueue<ObjectType, SizeType> GrowingQueue<ObjectType, SizeType>::operator+(const GrowingQueue<ObjectType, SizeType>& aGrowingQueue)
	{
		GrowingQueue<ObjectType, SizeType> temp;

		assert(myArray != nullptr && aGrowingQueue.IsInitialized() && "init needed");
		if (myArray != nullptr && aGrowingQueue.IsInitialized())
		{
			SizeType newCapacity = mySize + aGrowingQueue.Size();
			SizeType newStart = newCapacity / 2;
			newCapacity += newStart;

			temp.myArray = new ObjectType[newCapacity];
			for (SizeType index = 0; index < mySize; index++)
			{
				temp.myArray[newStart + index] = myArray[myStart + index];
			}
			for (SizeType index = 0; index < aGrowingQueue.Size(); index++)
			{
				temp.myArray[newStart + mySize + index] = aGrowingQueue[index];
			}

			temp.myStart = newStart;
			temp.mySize = newCapacity - newStart;
			temp.myCapacity = newCapacity;
			temp.mySafeMode = mySafeMode && aGrowingQueue.mySafeMode;
		}

		return temp;
	}
	template<typename ObjectType, typename SizeType>
	void GrowingQueue<ObjectType, SizeType>::operator+=(const GrowingQueue<ObjectType, SizeType>& aGrowingQueue)
	{
		assert(myArray != nullptr && aGrowingQueue.IsInitialized() && "init needed");
		if (myArray != nullptr && aGrowingQueue.IsInitialized())
		{
			SizeType newCapacity = mySize + aGrowingQueue.Size();
			SizeType newStart = newCapacity / 2;
			newCapacity += newStart;

			ObjectType* oldArray = myArray;
			myArray = new ObjectType[newCapacity];
			for (SizeType index = 0; index < mySize; index++)
			{
				myArray[newStart + index] = oldArray[myStart + index];
			}
			delete oldArray;
			for (SizeType index = 0; index < aGrowingQueue.Size(); index++)
			{
				myArray[newStart + mySize + index] = aGrowingQueue[index];
			}

			myStart = newStart;
			mySize = newCapacity - newStart;
			myCapacity = newCapacity;
			mySafeMode = mySafeMode && aGrowingQueue.mySafeMode;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>::GrowingQueue()
	{
		myArray = nullptr;
		myStart = 0;
		mySize = 0;
		myCapacity = 0;
		mySafeMode = true;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>::GrowingQueue(ObjectType* aArrayPointer, const SizeType aSize, const bool aUseSafeModeFlag)
	{
		myStart = aSize;
		mySize = aSize;
		myCapacity = aSize * 2;
		mySafeMode = aUseSafeModeFlag;

		if (aArrayPointer != nullptr && aSize > 0)
		{
			myArray = new ObjectType[myCapacity];
			if (mySafeMode)
			{
				for (SizeType index = 0; index < aSize; index++)
				{
					myArray[myStart + index] = aArrayPointer[index];
				}
			}
			else
			{
				std::memcpy(myArray + myStart, aArrayPointer, sizeof(ObjectType) * aSize);
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>::GrowingQueue(const SizeType aCapacity, const bool aUseSafeModeFlag)
	{
		myArray = new ObjectType[aCapacity];
		myStart = aCapacity;
		mySize = 0;
		myCapacity = aCapacity;
		mySafeMode = aUseSafeModeFlag;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>::GrowingQueue(const GrowingQueue& aGrowingQueue)
	{
		myStart = aGrowingQueue.myStart;
		mySize = aGrowingQueue.mySize;
		myCapacity = aGrowingQueue.myCapacity;
		mySafeMode = aGrowingQueue.mySafeMode;

		if (aGrowingQueue.IsInitialized())
		{
			myArray = new ObjectType[myCapacity];
			if (mySafeMode)
			{
				for (SizeType index = 0; index < mySize; index++)
				{
					myArray[myStart + index] = aGrowingQueue[index];
				}
			}
			else
			{
				std::memcpy(myArray + myStart, aGrowingQueue.myArray + myStart, sizeof(ObjectType) * mySize);
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>::GrowingQueue(GrowingQueue&& aGrowingQueue)
	{
		myStart = aGrowingQueue.myStart;
		mySize = aGrowingQueue.mySize;
		myCapacity = aGrowingQueue.myCapacity;
		mySafeMode = aGrowingQueue.mySafeMode;

		myArray = aGrowingQueue.myArray;
		aGrowingQueue.myArray = nullptr;
		aGrowingQueue.~GrowingQueue();
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>::~GrowingQueue()
	{
		myStart = 0;
		mySize = 0;
		myCapacity = 0;

		if (myArray != nullptr)
		{
			delete[] myArray;
			myArray = nullptr;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>& GrowingQueue<ObjectType, SizeType>::operator=(const GrowingQueue& aGrowingQueue)
	{
		myStart = aGrowingQueue.myStart;
		mySize = aGrowingQueue.mySize;
		myCapacity = aGrowingQueue.myCapacity;
		mySafeMode = aGrowingQueue.mySafeMode;

		if (myArray != nullptr)
		{
			delete[] myArray;
		}
		if (aGrowingQueue.myArray != nullptr)
		{
			myArray = new ObjectType[myCapacity];
			if (mySafeMode)
			{
				for (SizeType index = myStart; index < myStart + mySize; index++)
				{
					myArray[index] = aGrowingQueue.myArray[index];
				}
			}
			else
			{
				std::memcpy(myArray, aGrowingQueue.myArray, sizeof(ObjectType) * myCapacity);
			}
		}
		return *this;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingQueue<ObjectType, SizeType>& GrowingQueue<ObjectType, SizeType>::operator=(GrowingQueue&& aGrowingQueue)
	{
		myStart = aGrowingQueue.myStart;
		mySize = aGrowingQueue.mySize;
		myCapacity = aGrowingQueue.myCapacity;
		mySafeMode = aGrowingQueue.mySafeMode;

		if (myArray != nullptr)
		{
			delete[] myArray;
		}
		myArray = aGrowingQueue.myArray;
		aGrowingQueue.myArray = nullptr;
		aGrowingQueue.~GrowingQueue();

		return *this;
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::Init(const SizeType aCapacity, const bool aUseSafeModeFlag)
	{
		if (myArray != nullptr)
		{
			delete[] myArray;
		}
		myArray = new ObjectType[aCapacity];
		myStart = aCapacity;
		mySize = 0;
		myCapacity = aCapacity;
		mySafeMode = aUseSafeModeFlag;
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingQueue<ObjectType, SizeType>::operator[](const SizeType& aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (myCapacity > 0 && aIndex >= 0 && aIndex < mySize)
		{
			return myArray[myStart + aIndex];
		}
		else
		{
			return *myArray;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline const ObjectType& GrowingQueue<ObjectType, SizeType>::operator[](const SizeType& aIndex) const
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (myCapacity > 0 && aIndex >= 0 && aIndex < mySize)
		{
			return myArray[myStart + aIndex];
		}
		else
		{
			return *myArray;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingQueue<ObjectType, SizeType>::Add()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			if (myStart + mySize >= myCapacity)
			{
				Grow();
			}
			else if (myStart == 0)
			{
				Optimize();
			}
			myStart--;
			mySize++;
		}
		return myArray[myStart];
	}
	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingQueue<ObjectType, SizeType>::Add(const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			if (mySize >= myCapacity)
			{
				Grow();
			}
			else if (myStart == 0)
			{
				Optimize();
			}
			myStart--;
			mySize++;
			myArray[myStart] = aObject;
		}
		return myArray[myStart];
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::Insert(const SizeType aIndex, const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (myArray != nullptr && aIndex >= 0)
		{
			if (aIndex < mySize)
			{
				if (mySize >= myCapacity)
				{
					Grow();
				}
				myStart--;
				mySize++;
				if (mySafeMode)
				{
					for (SizeType index = myStart; index < myStart + aIndex; index++)
					{
						myArray[index] = myArray[index - 1];
					}
				}
				else
				{
					memcpy(&myArray[myStart], &myArray[myStart + 1], sizeof(ObjectType) * (mySize - aIndex));
				}
				myArray[myStart + aIndex] = aObject;
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::Switch(const SizeType aIndex1, const SizeType aIndex2)
	{
		assert(myArray != nullptr && "init needed");
		assert(aIndex1 >= 0 && aIndex1 < mySize && "aIndex1 out of scope");
		assert(aIndex2 >= 0 && aIndex2 < mySize && "aIndex2 out of scope");
		if (myArray != nullptr)
		{
			ObjectType temp = myArray[myStart + aIndex1];
			myArray[myStart + aIndex1] = myArray[myStart + aIndex2];
			myArray[myStart + aIndex2] = temp;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::DeleteLast()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr && mySize > 0)
		{
			SizeType lastIndex = myStart + mySize - 1;
			delete myArray[lastIndex];
			myArray[lastIndex] = nullptr;
			mySize--;
			if (mySize == 0)
			{
				myStart = myCapacity - 1;
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::DeleteAtIndex(const SizeType aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > 0 && "Array is empty");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (mySize > 0 && aIndex >= 0 && aIndex < mySize)
		{
			delete myArray[myStart + aIndex];
			for (SizeType index = myStart + aIndex; index > myStart; index--)
			{
				myArray[index] = myArray[index - 1];
			}
			myArray[myStart] = nullptr;
			mySize--;
			myStart++;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::DeleteDuplicates()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			for (SizeType index = myStart; index < myStart + mySize; index++)
			{
				for (SizeType reverseIndex = myStart + mySize - 1; reverseIndex > index; reverseIndex--)
				{
					if (myArray[index] == myArray[reverseIndex])
					{
						//DeleteAtIndex(reverseIndex--);
						delete myArray[myStart + index];
						for (SizeType moveIndex = myStart + index; moveIndex > myStart; moveIndex--)
						{
							myArray[moveIndex] = myArray[moveIndex - 1];
						}
						mySize--;
						myStart++;
						index++;
					}
				}
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::DeleteAll()
	{
		assert(myArray != nullptr && "init needed");
		if (myCapacity > 0)
		{
			for (SizeType index = myStart; index < myStart + mySize; index++)
			{
				if (myArray[index] != nullptr)
				{
					delete myArray[index];
					myArray[index] = nullptr;
				}
			}
			mySize = 0;
			myStart = myCapacity - 1;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::RemoveLast()
	{
		assert(myArray != nullptr && "init needed");
		if (mySize > 0)
		{
			mySize--;
			if (mySize == 0)
			{
				myStart = myCapacity - 1;
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::RemoveAtIndex(const SizeType aIndex)
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > 0 && "Array is empty");
		assert(aIndex >= 0 && aIndex < mySize && "out of scope");
		if (mySize > 0 && aIndex >= 0 && aIndex < mySize)
		{
			for (SizeType index = myStart + aIndex; index > myStart; index--)
			{
				myArray[index] = myArray[index - 1];
			}
			mySize--;
			myStart++;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::RemoveDuplicates()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			for (SizeType index = myStart; index < myStart + mySize; index++)
			{
				for (SizeType reverseIndex = myStart + mySize - 1; reverseIndex > index; reverseIndex--)
				{
					if (myArray[index] == myArray[reverseIndex])
					{
						//RemoveAtIndex(reverseIndex--);
						for (SizeType moveIndex = myStart + index; moveIndex > myStart; moveIndex++)
						{
							myArray[moveIndex] = myArray[moveIndex - 1];
						}
						mySize--;
						myStart++;
						index++;
					}
				}
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::RemoveAll()
	{
		assert(myArray != nullptr && "init needed");
		mySize = 0;
		myStart = myCapacity - 1;
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::Resize(const SizeType aNewSize)
	{
		assert(myArray != nullptr && "init needed");
		//assert(aNewSize <= myCapacity && "capacity too low");
		if (myArray != nullptr)
		{
			mySize = aNewSize;
			if (myStart + aNewSize > myCapacity)
			{
				Reserve(myStart + aNewSize);
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::Reserve(const SizeType aCapacity)
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			myCapacity = aCapacity;
			ObjectType* newArray = new ObjectType[aCapacity];
			SizeType indexDiff = 0;
			if (mySize > aCapacity - myStart)
			{
				mySize = aCapacity - myStart;
			}
			else
			{
				indexDiff = aCapacity - (myStart + mySize);
			}

			if (mySafeMode)
			{
				for (SizeType index = myStart; index < myStart + mySize; index++)
				{
					newArray[index + indexDiff] = myArray[index];
				}
			}
			else
			{
				std::memcpy(newArray + myStart + indexDiff, myArray + myStart, sizeof(ObjectType) * mySize);
			}
			delete[] myArray;
			myArray = newArray;
			myStart += indexDiff;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::Optimize()
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr)
		{
			SizeType halfSize = mySize / 2;
			if (myCapacity - mySize < halfSize)
			{
				Reserve(halfSize + mySize);
			}
			else if (myStart + mySize < myCapacity)
			{
				SizeType indexDiff = myCapacity - (myStart + mySize);
				for (SizeType index = myStart + mySize - 1; index >= myStart; index--)
				{
					myArray[index + indexDiff] = myArray[index];
				}
			}
		}
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingQueue<ObjectType, SizeType>::Grow()
	{
		assert(myArray != nullptr && "init needed");
		if (myCapacity > 0)
		{
			Reserve(myCapacity * 2);
		}
		else
		{
			Reserve(1);
		}
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType* GrowingQueue<ObjectType, SizeType>::GetPointer()
	{
		return myArray + myStart;
	}
	template<typename ObjectType, typename SizeType>
	inline const ObjectType* GrowingQueue<ObjectType, SizeType>::GetPointer() const
	{
		return myArray + myStart;
	}

	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingQueue<ObjectType, SizeType>::Find(const ObjectType& aObject)
	{
		assert(myArray != nullptr && "init needed");
		if (myArray != nullptr && myCapacity > 0)
		{
			for (SizeType index = myStart; index < myStart + mySize; index++)
			{
				if (myArray[index] == aObject)
				{
					return index;
				}
			}
		}
		return FoundNone;
	}
	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingQueue<ObjectType, SizeType>::GetLast()
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > 0 && "empty vector");
		if (myCapacity > 0 && mySize > 0)
		{
			return myArray[myStart + mySize - 1];
		}
		else
		{
			return *myArray;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline const ObjectType& GrowingQueue<ObjectType, SizeType>::GetLast() const
	{
		assert(myArray != nullptr && "init needed");
		assert(mySize > 0 && "empty vector");
		if (myCapacity > 0 && mySize > 0)
		{
			return myArray[myStart + mySize - 1];
		}
		else
		{
			return *myArray;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingQueue<ObjectType, SizeType>::Size() const
	{
		return mySize;
	}
	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingQueue<ObjectType, SizeType>::Capacity() const
	{
		return myCapacity;
	}
	template<typename ObjectType, typename SizeType>
	__forceinline bool GrowingQueue<ObjectType, SizeType>::IsInitialized() const
	{
		return (myArray != nullptr);
	}
};