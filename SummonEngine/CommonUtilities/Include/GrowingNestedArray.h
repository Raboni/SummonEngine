#pragma once
#include <string>

namespace CU
{
	template<typename ObjectType, typename SizeType = int>
	class GrowingNestedArray
	{
	public:
		GrowingNestedArray();
		GrowingNestedArray(ObjectType* aArrayPointer, const SizeType aSize, const bool aUseSafeModeFlag = true);
		GrowingNestedArray(const SizeType aArrayCount, const SizeType aArrayCapacity, const bool aUseSafeModeFlag = true);
		GrowingNestedArray(const GrowingNestedArray& aArray);
		GrowingNestedArray(GrowingNestedArray&& aArray);
		~GrowingNestedArray();

		GrowingNestedArray& operator=(const GrowingNestedArray& aArray);
		GrowingNestedArray& operator=(GrowingNestedArray&& aArray);
		GrowingNestedArray<ObjectType, SizeType> operator+(const GrowingNestedArray<ObjectType, SizeType>& aArray);
		void operator+=(const GrowingNestedArray<ObjectType, SizeType>& aArray);
		inline ObjectType* operator[](const SizeType aArrayIndex);
		inline const ObjectType* operator[](const SizeType aArrayIndex) const;

		void Init(const SizeType aArrayCount, const SizeType aArrayCapacity, const bool aUseSafeModeFlag = true);
		inline ObjectType* AddArray();
		inline ObjectType* AddArray(ObjectType* aArrayPointer, const SizeType aSize);
		inline ObjectType& Add(const SizeType aArrayIndex);
		inline ObjectType& Add(const SizeType aArrayIndex, const ObjectType& aObject);
		inline void ResizeArray(const SizeType aArrayIndex, const SizeType aSize);

		inline SizeType ArrayCount() const;
		inline SizeType ArraySize(const SizeType aArrayIndex) const;

	private:
		SizeType* mySizes;
		ObjectType* myArray;
		SizeType myArrayCount;
		SizeType myArrayCapacity;
		bool mySafeMode;
	};

	template<typename ObjectType, typename SizeType>
	GrowingNestedArray<ObjectType, SizeType>::GrowingNestedArray()
	{
		mySizes = nullptr;
		myArray = nullptr;
		myArrayCount = static_cast<SizeType>(0);
		myArrayCapacity = static_cast<SizeType>(0);
		mySafeMode = true;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingNestedArray<ObjectType, SizeType>::GrowingNestedArray(ObjectType* aArrayPointer, const SizeType aSize, const bool aUseSafeModeFlag)
	{
		mySizes = nullptr;
		myArray = nullptr;
		myArrayCount = static_cast<SizeType>(0);
		myArrayCapacity = static_cast<SizeType>(0);
		mySafeMode = true;

		Init(1, aSize, aUseSafeModeFlag);
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingNestedArray<ObjectType, SizeType>::GrowingNestedArray(const SizeType aArrayCount, const SizeType aArrayCapacity, const bool aUseSafeModeFlag)
	{
		mySizes = nullptr;
		myArray = nullptr;
		myArrayCount = static_cast<SizeType>(0);
		myArrayCapacity = static_cast<SizeType>(0);
		mySafeMode = true;

		Init(aArrayCount, aArrayCapacity, aUseSafeModeFlag);
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingNestedArray<ObjectType, SizeType>::GrowingNestedArray(const GrowingNestedArray& aArray)
	{
		mySizes = nullptr;
		myArray = nullptr;
		myArrayCount = static_cast<SizeType>(0);
		myArrayCapacity = static_cast<SizeType>(0);
		mySafeMode = aArray.mySafeMode;
		if (aArray.mySizes == nullptr)
		{
			return;
		}

		Init(aArray.myArrayCount, aArray.myArrayCapacity, aArray.mySafeMode);

		if (mySafeMode)
		{
			memcpy(mySizes, aArray.mySizes, sizeof(SizeType) * myArrayCount);
			for (int index = 0; index < myArrayCount; index++)
			{
				myArray[index] = aArray.myArray[index];
			}
		}
		else
		{
			memcpy(mySizes, aArray.mySizes, sizeof(SizeType) * myArrayCount + sizeof(ObjectType) * myArrayCapacity * myArrayCount);
		}
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingNestedArray<ObjectType, SizeType>::GrowingNestedArray(GrowingNestedArray&& aArray)
	{
	}
	template<typename ObjectType, typename SizeType>
	GrowingNestedArray<ObjectType, SizeType>::~GrowingNestedArray()
	{
		if (mySizes != nullptr)
		{
			delete[] mySizes;
			mySizes = nullptr;
			myArray = nullptr;
		}
	}

	template<typename ObjectType, typename SizeType>
	inline GrowingNestedArray<ObjectType, SizeType>& GrowingNestedArray<ObjectType, SizeType>::operator=(const GrowingNestedArray& aArray)
	{
		Init(aArray.myArrayCount, aArray.myArrayCapacity, aArray.mySafeMode);

		if (mySafeMode)
		{
			memcpy(mySizes, aArray.mySizes, sizeof(SizeType) * myArrayCount);
			for (SizeType index = static_cast<SizeType>(0); index < myArrayCount; index++)
			{
				myArray[index] = aArray.myArray[index];
			}
		}
		else
		{
			memcpy(mySizes, aArray.mySizes, sizeof(SizeType) * myArrayCount + sizeof(ObjectType) * myArrayCapacity * myArrayCount);
		}

		return *this;
	}
	template<typename ObjectType, typename SizeType>
	inline GrowingNestedArray<ObjectType, SizeType>& GrowingNestedArray<ObjectType, SizeType>::operator=(GrowingNestedArray&& aArray)
	{
		// TODO: insert return statement here
	}

	template<typename ObjectType, typename SizeType>
	inline GrowingNestedArray<ObjectType, SizeType> GrowingNestedArray<ObjectType, SizeType>::operator+(const GrowingNestedArray<ObjectType, SizeType>& aArray)
	{
		if (mySizes == nullptr)
		{
			return aArray;
		}
		if (aArray.mySizes == nullptr)
		{
			return *this;
		}
		GrowingNestedArray<ObjectType, SizeType> temp(*this);

		return temp;
	}
	template<typename ObjectType, typename SizeType>
	inline void GrowingNestedArray<ObjectType, SizeType>::operator+=(const GrowingNestedArray<ObjectType, SizeType>& aArray)
	{
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType* GrowingNestedArray<ObjectType, SizeType>::operator[](const SizeType aArrayIndex)
	{
		return myArray + aArrayIndex * myArrayCapacity;
	}
	template<typename ObjectType, typename SizeType>
	inline const ObjectType* GrowingNestedArray<ObjectType, SizeType>::operator[](const SizeType aArrayIndex) const
	{
		return myArray + aArrayIndex * myArrayCapacity;
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingNestedArray<ObjectType, SizeType>::Init(const SizeType aArrayCount, const SizeType aArrayCapacity, const bool aUseSafeModeFlag)
	{
		if (mySizes != nullptr)
		{
			delete[] mySizes;
			mySizes = nullptr;
			myArray = nullptr;
		}

		myArrayCount = aArrayCount;
		myArrayCapacity = aArrayCapacity;
		mySafeMode = aUseSafeModeFlag;
		if (myArrayCount <= 0 || myArrayCapacity <= 0)
		{
			return;
		}

		mySizes = reinterpret_cast<SizeType*>(new char[sizeof(SizeType) * myArrayCount + sizeof(ObjectType) * myArrayCapacity * myArrayCount]);
		myArray = new(mySizes + myArrayCount) ObjectType[myArrayCapacity * myArrayCount];
		memset(mySizes, 0, sizeof(SizeType) * myArrayCount);
	}

	template<typename ObjectType, typename SizeType>
	inline ObjectType* GrowingNestedArray<ObjectType, SizeType>::AddArray()
	{
		if (mySizes == nullptr)
		{
			return nullptr;
		}

		const SizeType newArrayCount = myArrayCount + 1;
		char* newData = new char[sizeof(SizeType) * newArrayCount + sizeof(ObjectType) * myArrayCapacity * newArrayCount];

		if (mySafeMode)
		{
			memcpy(newData, mySizes, sizeof(SizeType) * myArrayCount);
			memset(newData + sizeof(SizeType) * myArrayCount, 0, sizeof(SizeType));
			ObjectType* newArray = reinterpret_cast<ObjectType*>(newData + sizeof(SizeType) * newArrayCount);
			for (SizeType index = static_cast<SizeType>(0); index < myArrayCount; index++)
			{
				newArray[index] = myArray[index];
			}
			new(newArray + myArrayCapacity * myArrayCount) ObjectType[myArrayCapacity];

			delete[] mySizes;
			mySizes = reinterpret_cast<SizeType*>(newData);
			myArray = newArray;
			myArrayCount = newArrayCount;
		}
		else
		{
			memcpy(newData, mySizes, sizeof(SizeType) * myArrayCount);
			memset(newData + sizeof(SizeType) * myArrayCount, 0, sizeof(SizeType));
			memcpy(newData + sizeof(SizeType) * newArrayCount, mySizes + myArrayCount, sizeof(ObjectType) * myArrayCapacity * myArrayCount);
			ObjectType* newArray = new(newData + myArrayCapacity * myArrayCount) ObjectType[myArrayCapacity];

			delete[] mySizes;
			mySizes = reinterpret_cast<SizeType*>(newData);
			myArray = newArray;
			myArrayCount = newArrayCount;
		}

		return myArray + myArrayCapacity * (myArrayCount - 1);
	}
	template<typename ObjectType, typename SizeType>
	inline ObjectType* GrowingNestedArray<ObjectType, SizeType>::AddArray(ObjectType* aArrayPointer, const SizeType aSize)
	{
		if (mySizes == nullptr)
		{
			return nullptr;
		}

		ObjectType* newArray = AddArray();
		SizeType newArraySize = aSize;
		if (newArraySize > myArrayCapacity)
		{
			newArraySize = myArrayCapacity;
		}

		if (mySafeMode)
		{
			for (SizeType index = static_cast<SizeType>(0); index < newArraySize; index++)
			{
				newArray[index] = aArrayPointer[index];
			}
			mySizes[myArrayCount - 1] = newArraySize;
		}
		else
		{
			memcpy(newArray, aArrayPointer, sizeof(ObjectType) * newArraySize);
			mySizes[myArrayCount - 1] = newArraySize;
		}
	}
	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingNestedArray<ObjectType, SizeType>::Add(const SizeType aArrayIndex)
	{
		// TODO: insert return statement here
	}
	template<typename ObjectType, typename SizeType>
	inline ObjectType& GrowingNestedArray<ObjectType, SizeType>::Add(const SizeType aArrayIndex, const ObjectType& aObject)
	{
		// TODO: insert return statement here
	}

	template<typename ObjectType, typename SizeType>
	inline void GrowingNestedArray<ObjectType, SizeType>::ResizeArray(const SizeType aArrayIndex, const SizeType aSize)
	{
		if (aArrayIndex < static_cast<SizeType>(0) || aArrayIndex >= myArrayCount || aSize < static_cast<SizeType>(0) || aSize >= myArrayCapacity)
		{
			return;
		}

		mySizes[aArrayIndex] = aSize;
	}

	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingNestedArray<ObjectType, SizeType>::ArrayCount() const
	{
		return myArrayCount;
	}
	template<typename ObjectType, typename SizeType>
	inline SizeType GrowingNestedArray<ObjectType, SizeType>::ArraySize(const SizeType aArrayIndex) const
	{
		if (aArrayIndex < static_cast<SizeType>(0) || aArrayIndex >= myArrayCount)
		{
			return static_cast<SizeType>(0);
		}

		return mySizes[aArrayIndex];
	}
}