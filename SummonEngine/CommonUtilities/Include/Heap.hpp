#pragma once
#include "GrowingArray.hpp"

namespace CU
{
	template <class T>
	class Heap
	{
	public:
		Heap();
		Heap& operator=(const Heap& aHeap);
		Heap& operator+=(const Heap& aHeap);
		Heap operator+(const Heap& aHeap);
		int GetSize() const; //returnerar antal element i heapen
		void Enqueue(const T& aElement); //lägger till elementet i heapen
		const T& GetTop() const; //returnerar det största elementet i heapen
		T Dequeue(); //tar bort det största elementet ur heapen och returnerar det
	private:
		void BubbleUp(const unsigned short& aIndex);
		void BubbleDown(const unsigned short& aIndex);
		GrowingArray<T> myArray;
	};

	template<class T>
	inline Heap<T>::Heap()
	{
		myArray.Init(4);
	}

	template<class T>
	inline Heap<T>& Heap<T>::operator=(const Heap<T>& aHeap)
	{
		myArray = aHeap.myArray;
		return *this;
	}

	template<class T>
	inline Heap<T>& Heap<T>::operator+=(const Heap<T>& aHeap)
	{
		myArray = myArray + aHeap.myArray;
		return *this;
	}

	template<class T>
	inline int Heap<T>::GetSize() const
	{
		return myArray.Size();
	}
	template<class T>
	inline void Heap<T>::Enqueue(const T& aElement)
	{
		myArray.Add(aElement);
		BubbleUp(myArray.Size() - 1);
	}
	template<class T>
	inline const T& Heap<T>::GetTop() const
	{
		assert(myArray.Size() > 0 && "Heap empty");
		return myArray[0];
	}
	template<class T>
	inline T Heap<T>::Dequeue()
	{
		assert(myArray.Size() > 0 && "Heap empty");
		T top = myArray[0];
		myArray.RemoveCyclicAtIndex(0);
		BubbleDown(0);
		return top;
	}
	template<class T>
	inline void Heap<T>::BubbleUp(const unsigned short& aIndex)
	{
		if (aIndex > 0)
		{
			unsigned short parentIndex = (aIndex - 1) / 2;
			if (myArray[parentIndex] < myArray[aIndex])
			{
				myArray.Switch(parentIndex, aIndex);
				BubbleUp(parentIndex);
			}
		}
	}
	template<class T>
	inline void Heap<T>::BubbleDown(const unsigned short& aIndex)
	{
		if (aIndex < myArray.Size())
		{
			unsigned short childIndex = (aIndex * 2) + 2;
			if (childIndex < myArray.Size())
			{
				childIndex -= static_cast<unsigned short>(myArray[childIndex] < myArray[childIndex - 1]);
			}
			else if (childIndex - 1 < myArray.Size())
			{
				childIndex -= 1;
			}
			else
			{
				return;
			}

			if (myArray[aIndex] < myArray[childIndex])
			{
				myArray.Switch(aIndex, childIndex);
				BubbleDown(childIndex);
			}
		}
	}
}