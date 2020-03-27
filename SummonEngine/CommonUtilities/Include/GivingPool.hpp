#pragma once
#include "Heap.hpp"
#include <new>
#undef GetObject

namespace CU
{
	template<class ObjectType, typename SizeType = unsigned int>
	class GivingPool
	{
	public:
		GivingPool();
		GivingPool(SizeType aPoolSize);
		~GivingPool();
		GivingPool& operator=(const GivingPool& aPool);

		void FillArray();
		void FillArrayObject(const ObjectType& aEmptyObject);
		ObjectType* GetObject();
		bool ReturnObject(ObjectType* aObject);
		SizeType Size() const;
		void SetCanGrow(bool aCanGrow);

	protected:
		ObjectType* myData;
		ObjectType myEmptyObject;
		Heap<SizeType> myIndexHeap;
		SizeType mySize;
		bool myFilled;
		bool myCanGrow;
	};

	template<class ObjectType, typename SizeType>
	inline GivingPool<ObjectType, SizeType>::GivingPool()
	{
		myData = nullptr;
		mySize = static_cast<SizeType>(0);
		myFilled = false;
		myCanGrow = false;
	}
	template<class ObjectType, typename SizeType>
	inline GivingPool<ObjectType, SizeType>::GivingPool(SizeType aPoolSize)
	{
		assert(aPoolSize > 0 && "Size must be larger than zero");
		if (aPoolSize > static_cast<SizeType>(0))
		{
			myData = new ObjectType[aPoolSize]();
			mySize = aPoolSize;
		}
		else
		{
			myData = nullptr;
			mySize = static_cast<SizeType>(0);
		}
		myFilled = false;
		myCanGrow = false;
	}
	template<class ObjectType, typename SizeType>
	inline GivingPool<ObjectType, SizeType>::~GivingPool()
	{
		if (myData != nullptr)
		{
			delete[] myData;
			myData = nullptr;
		}
	}

	template<class ObjectType, typename SizeType>
	inline GivingPool<ObjectType, SizeType>& GivingPool<ObjectType, SizeType>::operator=(const GivingPool<ObjectType, SizeType>& aPool)
	{
		if (myData != nullptr)
		{
			delete[] myData;
		}
		if (aPool.myData != nullptr)
		{
			myData = new ObjectType[aPool.mySize];
			for (SizeType i = 0; i < aPool.mySize; i++)
			{
				myData[i] = aPool.myData[i];
			}
		}

		myEmptyObject = aPool.myEmptyObject;
		myIndexHeap = aPool.myIndexHeap;
		mySize = aPool.mySize;
		myFilled = aPool.myFilled;
		myCanGrow = aPool.myCanGrow;
		return *this;
	}

	template<class ObjectType, typename SizeType>
	inline void GivingPool<ObjectType, SizeType>::FillArray()
	{
		assert(myData != nullptr && "Pool has no size");
		myEmptyObject = ObjectType();
		for (SizeType index = 0; index < mySize; index++)
		{
			new(&myData[index]) ObjectType();
			//myData[index] = myEmptyObject;
			myIndexHeap.Enqueue(index);
		}
		myFilled = true;
	}
	template<class ObjectType, typename SizeType>
	inline void GivingPool<ObjectType, SizeType>::FillArrayObject(const ObjectType& aEmptyObject)
	{
		assert(myData != nullptr && "Pool has no size");
		myEmptyObject = aEmptyObject;
		for (SizeType index = 0; index < mySize; index++)
		{
			myData[index] = myEmptyObject;
			myIndexHeap.Enqueue(index);
		}
		myFilled = true;
	}

	template<class ObjectType, typename SizeType>
	inline ObjectType* GivingPool<ObjectType, SizeType>::GetObject()
	{
		assert(myData != nullptr && "Pool has no size");
		if (myIndexHeap.GetSize() > 0)
		{
			return &myData[myIndexHeap.Dequeue()];
		}
		else if (myCanGrow)
		{
			new(&myData[mySize]) ObjectType[mySize];
			for (SizeType index = mySize; index < mySize * 2; index++)
			{
				myData[index] = myEmptyObject;
				myIndexHeap.Enqueue(index);
			}
			mySize += mySize;
			return &myData[myIndexHeap.Dequeue()];
		}
		return nullptr;
	}

	template<class ObjectType, typename SizeType>
	inline bool GivingPool<ObjectType, SizeType>::ReturnObject(ObjectType* aObject)
	{
		if (aObject == nullptr)
		{
			return false;
		}

		assert(myData != nullptr && "Pool has no size");
		for (SizeType index = 0; index < mySize; index++)
		{
			if (aObject == &myData[index])
			{
				myIndexHeap.Enqueue(index);
				return true;
			}
		}

		return false;
	}

	template<class ObjectType, typename SizeType>
	inline SizeType GivingPool<ObjectType, SizeType>::Size() const
	{
		return mySize;
	}

	template<class ObjectType, typename SizeType>
	inline void GivingPool<ObjectType, SizeType>::SetCanGrow(bool aCanGrow)
	{
		myCanGrow = aCanGrow;
	}
}