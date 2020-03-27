#pragma once
#include "DoublyLinkedList.hpp"

namespace CU
{
	template <class T>
	class Queue
	{
	public:
		//Skapar en tom k�
		Queue();
		//Returnerar antal element i k�n
		int GetSize() const;
		//Returnerar elementet l�ngst fram i k�n. Kraschar med en assert om k�n �r
		//tom
		const T &GetFront() const;
		//Returnerar elementet l�ngst fram i k�n. Kraschar med en assert om k�n �r
		//tom
		T &GetFront();
		//L�gger in ett nytt element l�ngst bak i k�n
		void Enqueue(const T& aValue);
		//Tar bort elementet l�ngst fram i k�n och returnerar det. Kraschar med en
		//assert om k�n �r tom.
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