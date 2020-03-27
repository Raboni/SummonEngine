#pragma once
#include "GrowingArray.hpp"

namespace CU
{
	template <class T, typename SizeType = unsigned short>
	class Stack
	{
	public:
		//Skapar en tom stack
		Stack();
		Stack(GrowingArray<T, SizeType> aArray);
		//Returnerar antal element i stacken
		int GetSize() const;
		//Returnerar det översta elementet i stacken. Kraschar med en assert om
		//stacken är tom.
		const T &GetTop() const;
		//Returnerar det översta elementet i stacken. Kraschar med en assert om
		//stacken är tom.
		T &GetTop();
		//Lägger in ett nytt element överst på stacken
		void Push(const T& aValue);
		//Tar bort det översta elementet från stacken och returnerar det. Kraschar
		//med en assert om stacken är tom.
		T Pop();
	private:
		GrowingArray<T, SizeType> myArray;
	};

	template<class T, typename SizeType>
	inline Stack<T, SizeType>::Stack()
	{
		myArray.Init(4);
	}

	template<class T, typename SizeType>
	inline Stack<T, SizeType>::Stack(GrowingArray<T, SizeType> aArray)
	{
		assert(aArray.IsInitialized() && "array init needed");
		if(aArray.IsInitialized())
		{
			myArray = aArray;
		}
		else
		{
			myArray.Init(4);
		}
	}

	template<class T, typename SizeType>
	inline int Stack<T, SizeType>::GetSize() const
	{
		assert(myArray.IsInitialized() && "array init needed");
		if (myArray.IsInitialized())
		{
			return myArray.Size();
		}
		else
		{
			return static_cast<SizeType>(0);
		}
	}

	template<class T, typename SizeType>
	inline const T& Stack<T, SizeType>::GetTop() const
	{
		assert(myArray.IsInitialized() && "array init needed");
		assert(myArray.Size() > static_cast<SizeType>(0) && "stack empty");
		return myArray.GetLast();
	}

	template<class T, typename SizeType>
	inline T& Stack<T, SizeType>::GetTop()
	{
		assert(myArray.IsInitialized() && "array init needed");
		assert(myArray.Size() > static_cast<SizeType>(0) && "stack empty");
		return myArray.GetLast();
	}

	template<class T, typename SizeType>
	inline void Stack<T, SizeType>::Push(const T& aValue)
	{
		assert(myArray.IsInitialized() && "array init needed");
		myArray.Add(aValue);
	}

	template<class T, typename SizeType>
	inline T Stack<T, SizeType>::Pop()
	{
		assert(myArray.IsInitialized() && "array init needed");
		assert(myArray.Size() > static_cast<SizeType>(0) && "stack empty");
		T last = myArray.GetLast();
		myArray.RemoveLast();
		return last;
	}
}