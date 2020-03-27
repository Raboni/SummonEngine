#pragma once
#include <assert.h>
#include <vector>

namespace CU
{
	template<typename ObjectType>
	class GrowingArrayVector
	{
	public:
		GrowingArrayVector();
		GrowingArrayVector(ObjectType* aArrayPointer, const unsigned int aSize);
		GrowingArrayVector(const unsigned int aCapacity);
		GrowingArrayVector(const GrowingArrayVector& aGrowingArray);
		GrowingArrayVector(const std::vector<ObjectType>& aVector);
		GrowingArrayVector(GrowingArrayVector&& aGrowingArray);
		GrowingArrayVector(std::vector<ObjectType>&& aVector);
		~GrowingArrayVector();

		GrowingArrayVector& operator=(const GrowingArrayVector& aGrowingArray);
		GrowingArrayVector& operator=(GrowingArrayVector&& aGrowingArray);
		inline ObjectType& operator[](const unsigned int aIndex);
		inline const ObjectType& operator[](const unsigned int aIndex) const;

		inline ObjectType& Add();
		inline ObjectType& Add(const ObjectType& aObject);
		inline void Insert(const unsigned int aIndex, const ObjectType& aObject);
		inline void Switch(const unsigned int aIndex1, const unsigned int aIndex2);
		inline void Reverse();

		inline bool DeleteCyclic(const ObjectType& aObject);
		inline void DeleteCyclicAtIndex(const unsigned int aIndex);
		inline void DeleteAtIndex(const unsigned int aIndex);
		inline void DeleteLast();
		inline void DeleteAll();

		inline bool RemoveCyclic(const ObjectType& aObject);
		inline void RemoveCyclicAtIndex(const unsigned int aIndex);
		inline void RemoveAtIndex(const unsigned int aIndex);
		inline void RemoveLast();
		inline void RemoveAll();

		inline void Resize(const unsigned int aNewSize);
		inline void Reserve(const unsigned int aNewSize);
		inline void Optimize();
		inline void Grow();

		inline ObjectType* GetPointer();
		inline const ObjectType* GetPointer() const;
		inline const unsigned int Find(const ObjectType& aObject) const;
		inline ObjectType& GetLast();
		inline const ObjectType& GetLast() const;
		inline std::vector<ObjectType>& GetVector();
		inline const std::vector<ObjectType>& GetVector() const;
		__forceinline const unsigned int Size() const;
		__forceinline const unsigned int Capacity() const;
		static const unsigned int FoundNone = static_cast<unsigned int>(-1);
	private:
		std::vector<ObjectType> myVector;
	};
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::GrowingArrayVector()
	{
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::GrowingArrayVector(ObjectType* aArrayPointer, const unsigned int aSize) : myVector(aSize)
	{
		for (unsigned int index = 0; index < aSize; index++)
		{
			myVector.push_back(aArrayPointer[index]);
		}
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::GrowingArrayVector(const unsigned int aCapacity) : myVector(aCapacity)
	{
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::GrowingArrayVector(const GrowingArrayVector& aGrowingArray) : myVector(aGrowingArray.GetVector())
	{
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::GrowingArrayVector(const std::vector<ObjectType>& aVector) : myVector(aVector)
	{
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::GrowingArrayVector(GrowingArrayVector&& aGrowingArray)
	{
		myVector = std::vector<ObjectType>(aGrowingArray);
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::GrowingArrayVector(std::vector<ObjectType>&& aVector)
	{
		myVector = std::vector<ObjectType>(aVector);
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>::~GrowingArrayVector()
	{
	}

	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>& GrowingArrayVector<ObjectType>::operator=(const GrowingArrayVector& aGrowingArray)
	{
		myVector = aGrowingArray.GetVector();
	}
	template<typename ObjectType>
	inline GrowingArrayVector<ObjectType>& GrowingArrayVector<ObjectType>::operator=(GrowingArrayVector&& aGrowingArray)
	{
		myVector = std::move(aGrowingArray.GetVector());
	}

	template<typename ObjectType>
	inline ObjectType& GrowingArrayVector<ObjectType>::operator[](const unsigned int aIndex)
	{
		return myVector[aIndex];
	}
	template<typename ObjectType>
	inline const ObjectType& GrowingArrayVector<ObjectType>::operator[](const unsigned int aIndex) const
	{
		return myVector[aIndex];
	}

	template<typename ObjectType>
	inline ObjectType& GrowingArrayVector<ObjectType>::Add()
	{
		myVector.resize(myVector.size() + 1);
		return myVector.back();
	}
	template<typename ObjectType>
	inline ObjectType& GrowingArrayVector<ObjectType>::Add(const ObjectType& aObject)
	{
		myVector.push_back(aObject);
		return myVector.back();
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::Insert(const unsigned int aIndex, const ObjectType& aObject)
	{
		assert(aIndex != FoundNone && aIndex <= Size() && "out of scope");
		myVector.insert(aIndex, aObject);
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::Switch(const unsigned int aIndex1, const unsigned int aIndex2)
	{
		assert(aIndex1 != FoundNone && aIndex1 < Size() && "aIndex1 out of scope");
		assert(aIndex2 != FoundNone && aIndex2 < Size() && "aIndex2 out of scope");

		ObjectType temp = myVector[aIndex1];
		myVector[aIndex1] = myVector[aIndex2];
		myVector[aIndex2] = temp;
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::Reverse()
	{
		unsigned int size = myVector.size();
		for (unsigned int index = 0; index < size / 2; index++)
		{
			Switch(index, size - 1 - index);
		}
	}

	template<typename ObjectType>
	inline bool GrowingArrayVector<ObjectType>::DeleteCyclic(const ObjectType& aObject)
	{
		assert(myVector.size() > 0 && "Array is empty");
		if (myVector.size() > 0)
		{
			unsigned int index = Find(aObject);
			if (index != FoundNone)
			{
				delete myVector[index];
				myVector[index] = myVector[myVector.size() - 1];
				myVector.pop_back();
			}
			return index != FoundNone;
		}
		return false;
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::DeleteCyclicAtIndex(const unsigned int aIndex)
	{
		assert(myVector.size() > 0 && "Array is empty");
		assert(aIndex != FoundNone && aIndex < Size() && "out of scope");
		if (myVector.size() > 0 && aIndex != FoundNone && aIndex < Size())
		{
			delete myVector[aIndex];
			myVector[aIndex] = myVector[myVector.size() - 1];
			myVector.pop_back();
		}
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::DeleteAtIndex(const unsigned int aIndex)
	{
		assert(myVector.size() > 0 && "Array is empty");
		assert(aIndex != FoundNone && aIndex < Size() && "out of scope");
		if (myVector.size() > 0 && aIndex != FoundNone && aIndex < Size())
		{
			delete myVector[aIndex];
			myVector[aIndex] = nullptr;
			for (unsigned int index = aIndex; index < Size() - 1; index++)
			{
				myVector[index] = myVector[index + 1];
			}
			myVector.pop_back();
		}
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::DeleteLast()
	{
		assert(myVector.size() > 0 && "Array is empty");
		if (myVector.size() > 0)
		{
			delete myVector[myVector.size() - 1];
			myVector.pop_back();
		}
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::DeleteAll()
	{
		for (unsigned int index = 0; index < myVector.size(); index++)
		{
			delete myVector[index];
			myVector[index] = nullptr;
		}
		myVector.clear();
	}

	template<typename ObjectType>
	inline bool GrowingArrayVector<ObjectType>::RemoveCyclic(const ObjectType& aObject)
	{
		assert(myVector.size() > 0 && "Array is empty");
		if (myVector.size() > 0)
		{
			unsigned int index = Find(aObject);
			if (index != FoundNone)
			{
				myVector[index] = myVector[myVector.size() - 1];
				myVector.pop_back();
			}
			return index != FoundNone;
		}
		return false;
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::RemoveCyclicAtIndex(const unsigned int aIndex)
	{
		assert(myVector.size() > 0 && "Array is empty");
		assert(aIndex != FoundNone && aIndex < Size() && "out of scope");
		if (myVector.size() > 0 && aIndex != FoundNone && aIndex < Size())
		{
			myVector[aIndex] = myVector[myVector.size() - 1];
			myVector.pop_back();
		}
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::RemoveAtIndex(const unsigned int aIndex)
	{
		assert(myVector.size() > 0 && "Array is empty");
		assert(aIndex != FoundNone && aIndex < Size() && "out of scope");
		if (myVector.size() > 0 && aIndex != FoundNone && aIndex < Size())
		{
			for (unsigned int index = aIndex; index < Size() - 1; index++)
			{
				myVector[index] = myVector[index + 1];
			}
			myVector.pop_back();
		}
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::RemoveLast()
	{
		assert(myVector.size() > 0 && "Array is empty");
		if (myVector.size() > 0)
		{
			myVector.pop_back();
		}
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::RemoveAll()
	{
		myVector.clear();
	}

	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::Resize(const unsigned int aNewSize)
	{
		assert(aNewSize <= myVector.capacity() && "capacity too low");
		if (aNewSize <= myVector.capacity())
		{
			myVector.resize(aNewSize);
		}
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::Reserve(const unsigned int aNewSize)
	{
		myVector.reserve(aNewSize);
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::Optimize()
	{
		myVector.shrink_to_fit();
	}
	template<typename ObjectType>
	inline void GrowingArrayVector<ObjectType>::Grow()
	{
		myVector.reserve(myVector.capacity() * 2);
	}

	template<typename ObjectType>
	inline ObjectType* GrowingArrayVector<ObjectType>::GetPointer()
	{
		return myVector.data();
	}

	template<typename ObjectType>
	inline const ObjectType* GrowingArrayVector<ObjectType>::GetPointer() const
	{
		return myVector.data();
	}

	template<typename ObjectType>
	inline const unsigned int GrowingArrayVector<ObjectType>::Find(const ObjectType& aObject) const
	{
		for (unsigned int index = 0; index < Size(); index++)
		{
			if (myVector[index] == aObject)
			{
				return index;
			}
		}
		return FoundNone;
	}
	template<typename ObjectType>
	inline ObjectType& GrowingArrayVector<ObjectType>::GetLast()
	{
		assert(myVector.size() > 0 && "Array is empty");
		if (myVector.size() > 0)
		{
			return myVector[myVector.size() - 1];
		}
		else
		{
			return myVector[0];
		}
	}
	template<typename ObjectType>
	inline const ObjectType & GrowingArrayVector<ObjectType>::GetLast() const
	{
		assert(myVector.size() > 0 && "Array is empty");
		if (myVector.size() > 0)
		{
			return myVector[myVector.size() - 1];
		}
		else
		{
			return myVector[0];
		}
	}
	template<typename ObjectType>
	inline std::vector<ObjectType>& GrowingArrayVector<ObjectType>::GetVector()
	{
		return myVector;
	}
	template<typename ObjectType>
	inline const std::vector<ObjectType>& GrowingArrayVector<ObjectType>::GetVector() const
	{
		return myVector;
	}
	template<typename ObjectType>
	inline const unsigned int GrowingArrayVector<ObjectType>::Size() const
	{
		return static_cast<unsigned int>(myVector.size());
	}
	template<typename ObjectType>
	inline const unsigned int GrowingArrayVector<ObjectType>::Capacity() const
	{
		return static_cast<unsigned int>(myVector.capacity());
	}
}