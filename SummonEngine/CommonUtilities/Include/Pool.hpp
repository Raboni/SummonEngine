#pragma once
#include "GrowingArray.hpp"

namespace CU
{
	template<class ObjectType, typename SizeType = unsigned int>
	class Pool
	{
	public:
		Pool();
		Pool(SizeType aPoolSize);
		~Pool() = default;
		ObjectType& operator[](const SizeType& aIndex);
		const ObjectType& operator[](const SizeType& aIndex) const;

		virtual void FillArray();
		virtual void FillArrayObject(const ObjectType& aEmptyObject);
		virtual ObjectType& Add();
		virtual ObjectType& Add(const ObjectType& aObject);
		void RemoveIndex(const SizeType& aIndex);
		bool Remove(const ObjectType& aObject);
		ObjectType& GetLast();
		const ObjectType& GetLast() const;
		SizeType Size() const;
		SizeType Capacity() const;
		void SetCanGrow(bool aCanGrow);

	protected:
		GrowingArray<ObjectType, SizeType> myArray;
		ObjectType myEmptyObject;
		SizeType mySize;
		bool myFilled;
		bool myCanGrow;
	};
	template<class ObjectType, typename SizeType = unsigned int>
	class PointerPool : public Pool<ObjectType*, SizeType>
	{
	public:
		PointerPool();
		PointerPool(SizeType aPoolSize);
		~PointerPool();

		void FillArray() override;
		void FillArrayObject(ObjectType* aEmptyObject);
		void FillArrayCopy(ObjectType* aEmptyObject);
		ObjectType& Add(const ObjectType& aObject) = delete;
		ObjectType* Add(ObjectType* aObject);
		bool RemoveObject(const ObjectType& aObject);

	private:
		ObjectType* myEmptyObject;
		bool myCopied;
	};

	template<class ObjectType, typename SizeType>
	inline Pool<ObjectType, SizeType>::Pool()
	{
		mySize = static_cast<SizeType>(0);
		myFilled = false;
		myCanGrow = false;
		myArray.Init(64);
	}
	template<class ObjectType, typename SizeType>
	inline Pool<ObjectType, SizeType>::Pool(SizeType aPoolSize)
	{
		mySize = static_cast<SizeType>(0);
		myFilled = false;
		myCanGrow = false;
		myArray.Init(aPoolSize);
	}
	template<class ObjectType, typename SizeType>
	inline ObjectType& Pool<ObjectType, SizeType>::operator[](const SizeType& aIndex)
	{
		assert(aIndex < mySize && "out of scope");
		return myArray[aIndex];
	}
	template<class ObjectType, typename SizeType>
	inline const ObjectType& Pool<ObjectType, SizeType>::operator[](const SizeType& aIndex) const
	{
		assert(aIndex < mySize && "out of scope");
		return myArray[aIndex];
	}
	template<class ObjectType, typename SizeType>
	inline void Pool<ObjectType, SizeType>::FillArray()
	{
		myArray.RemoveAll();
		myEmptyObject = ObjectType();
		for (SizeType index = 0; index < myArray.Capacity(); index++)
		{
			myArray.Add(myEmptyObject);
		}
	}
	template<class ObjectType, typename SizeType>
	inline void Pool<ObjectType, SizeType>::FillArrayObject(const ObjectType& aEmptyObject)
	{
		myArray.RemoveAll();
		myEmptyObject = aEmptyObject;
		for (SizeType index = 0; index < myArray.Capacity(); index++)
		{
			myArray.Add(myEmptyObject);
		}
	}
	template<class ObjectType, typename SizeType>
	inline ObjectType& Pool<ObjectType, SizeType>::Add()
	{
		assert(mySize < myArray.Capacity() || myCanGrow && "Pool full");
		if (mySize >= myArray.Capacity())
		{
			if (!myCanGrow)
			{
				return myArray[mySize - 1];
			}
			myArray.Grow();
			for (SizeType index = mySize; index < myArray.Capacity(); index++)
			{
				myArray.Add(myEmptyObject);
			}
		}
		return myArray[mySize++];
	}
	template<class ObjectType, typename SizeType>
	inline ObjectType& Pool<ObjectType, SizeType>::Add(const ObjectType& aObject)
	{
		assert(mySize < myArray.Capacity() || myCanGrow && "Pool full");
		if (mySize >= myArray.Capacity())
		{
			if (!myCanGrow)
			{
				return myArray[mySize - 1];
			}
			myArray.Grow();
			for (SizeType index = mySize; index < myArray.Capacity(); index++)
			{
				myArray.Add(myEmptyObject);
			}
		}
		myArray[mySize] = aObject;
		return myArray[mySize++];
	}
	template<class ObjectType, typename SizeType>
	inline void Pool<ObjectType, SizeType>::RemoveIndex(const SizeType& aIndex)
	{
		assert(aIndex >= 0 && aIndex < mySize);
		myArray.Switch(aIndex, --mySize);
	}
	template<class ObjectType, typename SizeType>
	inline bool Pool<ObjectType, SizeType>::Remove(const ObjectType& aObject)
	{
		for (SizeType index = 0; index < mySize; index++)
		{
			if (myArray[index] == aObject)
			{
				myArray[aIndex].~ObjectType();
				myArray.Switch(index, --mySize);
				return true;
			}
		}
		return false;
	}
	template<class ObjectType, typename SizeType>
	inline ObjectType& Pool<ObjectType, SizeType>::GetLast()
	{
		assert(mySize > 0 && "Pool empty");
		if (mySize > 0)
		{
			return myArray[mySize - 1];
		}
		else
		{
			return myArray[0];
		}
	}
	template<class ObjectType, typename SizeType>
	inline const ObjectType& Pool<ObjectType, SizeType>::GetLast() const
	{
		assert(mySize > 0 && "Pool empty");
		if (mySize > 0)
		{
			return myArray[mySize - 1];
		}
		else
		{
			return myArray[0];
		}
	}
	template<class ObjectType, typename SizeType>
	__forceinline SizeType Pool<ObjectType, SizeType>::Size() const
	{
		return mySize;
	}
	template<class ObjectType, typename SizeType>
	inline SizeType Pool<ObjectType, SizeType>::Capacity() const
	{
		return myArray.Capacity();
	}

	template<class ObjectType, typename SizeType>
	inline void Pool<ObjectType, SizeType>::SetCanGrow(bool aCanGrow)
	{
		myCanGrow = aCanGrow;
	}


	template<class ObjectType, typename SizeType>
	inline PointerPool<ObjectType, SizeType>::PointerPool() : Pool()
	{
		myEmptyObject = nullptr;
		myCopied = false;
	}
	template<class ObjectType, typename SizeType>
	inline PointerPool<ObjectType, SizeType>::PointerPool(SizeType aPoolSize) : Pool(aPoolSize)
	{
		myEmptyObject = nullptr;
		myCopied = false;
	}
	template<class ObjectType, typename SizeType>
	inline PointerPool<ObjectType, SizeType>::~PointerPool()
	{
		myArray.DeleteAll();
	}
	template<class ObjectType, typename SizeType>
	inline void PointerPool<ObjectType, SizeType>::FillArray()
	{
		if (myArray.Size() > 0)
		{
			if (myCopied)
			{
				myArray.DeleteAll();
			}
			else
			{
				while (myArray.Size() > 0)
				{
					if (myArray.GetLast() != myEmptyObject)
					{
						myArray.DeleteLast();
					}
					else
					{
						myArray.RemoveLast();
					}
				}
			}
		}
		myCopied = false;
		delete myEmptyObject;
		myEmptyObject = new ObjectType();
		for (SizeType index = 0; index < myArray.Capacity(); index++)
		{
			myArray.Add(myEmptyObject);
		}
	}
	template<class ObjectType, typename SizeType>
	inline void PointerPool<ObjectType, SizeType>::FillArrayObject(ObjectType* aEmptyObject)
	{
		if (myArray.Size() > 0)
		{
			if (myCopied)
			{
				myArray.DeleteAll();
			}
			else
			{
				while (myArray.Size() > 0)
				{
					if (myArray.GetLast() != myEmptyObject)
					{
						myArray.DeleteLast();
					}
					else
					{
						myArray.RemoveLast();
					}
				}
			}
		}
		myCopied = false;
		delete myEmptyObject;
		myEmptyObject = aEmptyObject;
		for (SizeType index = 0; index < myArray.Capacity(); index++)
		{
			myArray.Add(myEmptyObject);
		}
	}
	template<class ObjectType, typename SizeType>
	inline void PointerPool<ObjectType, SizeType>::FillArrayCopy(ObjectType* aEmptyObject)
	{
		if (myArray.Size() > 0)
		{
			if (myCopied)
			{
				myArray.DeleteAll();
			}
			else
			{
				while (myArray.Size() > 0)
				{
					if (myArray.GetLast() != myEmptyObject)
					{
						myArray.DeleteLast();
					}
					else
					{
						myArray.RemoveLast();
					}
				}
			}
		}
		myCopied = true;
		myEmptyObject = aEmptyObject;
		for (SizeType index = 0; index < myArray.Capacity(); index++)
		{
			myArray.Add(new ObjectType(*myEmptyObject));
		}
	}
	template<class ObjectType, typename SizeType>
	inline ObjectType* PointerPool<ObjectType, SizeType>::Add(ObjectType* aObject)
	{
		if (myArray.Capacity() <= mySize)
		{
			myArray.Grow();
			if (myCopied)
			{
				for (SizeType index = mySize; index < myArray.Capacity(); index++)
				{
					myArray.Add(new ObjectType(*myEmptyObject));
				}
			}
			else
			{
				for (SizeType index = mySize; index < myArray.Capacity(); index++)
				{
					myArray.Add(myEmptyObject);
				}
			}
		}
		if (myCopied || myArray[mySize] != myEmptyObject)
		{
			delete myArray[mySize];
		}
		myArray[mySize] = aObject;
		return myArray[mySize++];
	}
	template<class ObjectType, typename SizeType>
	inline bool PointerPool<ObjectType, SizeType>::RemoveObject(const ObjectType & aObject)
	{
		for (SizeType index = 0; index < mySize; index++)
		{
			if (*(myArray[index]) == aObject)
			{
				myArray.Switch(index, --mySize);
				return true;
			}
		}
		return false;
	}
}