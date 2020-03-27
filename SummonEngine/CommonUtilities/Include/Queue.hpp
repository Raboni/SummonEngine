#pragma once
#include "DoublyLinkedList.hpp"

namespace CU
{
	template <class T>
	class Queue
	{
	public:
		//Skapar en tom kö
		Queue();
		//Returnerar antal element i kön
		int GetSize() const;
		//Returnerar elementet längst fram i kön. Kraschar med en assert om kön är
		//tom
		const T &GetFront() const;
		//Returnerar elementet längst fram i kön. Kraschar med en assert om kön är
		//tom
		T &GetFront();
		//Lägger in ett nytt element längst bak i kön
		void Enqueue(const T& aValue);
		//Tar bort elementet längst fram i kön och returnerar det. Kraschar med en
		//assert om kön är tom.
		T Dequeue();
	private:
		DoublyLinkedList<T> myList;
	};
	template<class T>
	inline Queue<T>::Queue()
	{
	}
	template<class T>
	inline int Queue<T>::GetSize() const
	{
		return myList.GetSize();
	}
	template<class T>
	inline const T& Queue<T>::GetFront() const
	{
		assert(myList.GetSize() > 0 && "empty list");
		return myList.GetFirst()->GetValue();
	}
	template<class T>
	inline T& Queue<T>::GetFront()
	{
		assert(myList.GetSize() > 0 && "empty list");
		return myList.GetFirst()->GetValue();
	}
	template<class T>
	inline void Queue<T>::Enqueue(const T& aValue)
	{
		myList.InsertLast(aValue);
	}
	template<class T>
	inline T Queue<T>::Dequeue()
	{
		assert(myList.GetSize() > 0 && "empty list");
		T first = myList.GetFirst()->GetValue();
		myList.Remove(myList.GetFirst());
		return first;
	}
}