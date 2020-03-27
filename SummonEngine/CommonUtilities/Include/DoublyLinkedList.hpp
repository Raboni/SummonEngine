#pragma once
#include "DoublyLinkedListNode.hpp"
#include <assert.h>

namespace CU
{
	template <class T>
	class DoublyLinkedList
	{
	public:
		// Skapar en tom lista
		DoublyLinkedList();

		// Frigör allt minne som listan allokerat
		~DoublyLinkedList();

		// Returnerar antalet element i listan
		int GetSize() const;

		// Returnerar första noden i listan, eller nullptr om listan är tom
		DoublyLinkedListNode<T>* GetFirst();

		const DoublyLinkedListNode<T>* GetFirst() const;

		// Returnerar sista noden i listan, eller nullptr om listan är tom
		DoublyLinkedListNode<T>* GetLast();

		const DoublyLinkedListNode<T>* GetLast() const;

		// Skjuter in ett nytt element först i listan
		void InsertFirst(const T& aValue);

		// Skjuter in ett nytt element sist i listan
		void InsertLast(const T& aValue);

		// Skjuter in ett nytt element innan aNode
		void InsertBefore(DoublyLinkedListNode<T>* aNode, const T& aValue);

		// Skjuter in ett nytt element efter aNode
		void InsertAfter(DoublyLinkedListNode<T>* aNode, const T& aValue);

		// Plockar bort noden ur listan och frigör minne. (Det är ok att anta att
		// aNode är en nod i listan, och inte från en annan lista)
		void Remove(DoublyLinkedListNode<T>* aNode);

		// Hittar första elementet i listan som har ett visst värde. Jämförelsen
		// görs med operator==. Om inget element hittas returneras nullptr.
		DoublyLinkedListNode<T>* FindFirst(const T& aValue);

		// Hittar sista elementet i listan som har ett visst värde. Jämförelsen
		// görs med operator==. Om inget element hittas returneras nullptr.
		DoublyLinkedListNode<T>* FindLast(const T& aValue);

		// Plockar bort första elementet i listan som har ett visst värde. 
		// Jämförelsen görs med operator==. Om inget element hittas görs ingenting.
		// Returnerar true om ett element plockades bort, och false annars.
		bool RemoveFirst(const T& aValue);

		// Plockar bort sista elementet i listan som har ett visst värde.
		// Jämförelsen görs med operator==. Om inget element hittas görs ingenting.
		// Returnerar true om ett element plockades bort, och false annars.
		bool RemoveLast(const T& aValue);
	private:
		DoublyLinkedListNode<T>* myFirstNode;
		DoublyLinkedListNode<T>* myLastNode;
		int mySize;
	};

	template<class T>
	inline DoublyLinkedList<T>::DoublyLinkedList()
	{
		myFirstNode = nullptr;
		myLastNode = nullptr;
		mySize = 0;
	}
	template<class T>
	inline DoublyLinkedList<T>::~DoublyLinkedList()
	{
		DoublyLinkedListNode<T>* node = myFirstNode;
		DoublyLinkedListNode<T>* nodeNext = nullptr;
		while (node != nullptr)
		{
			nodeNext = node->myNextNode;
			node->myPreviousNode = nullptr;
			node->myNextNode = nullptr;
			delete node;
			node = nodeNext;
		}
		mySize = 0;
	}

	template<class T>
	inline int DoublyLinkedList<T>::GetSize() const
	{
		return mySize;
	}
	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetFirst()
	{
		return myFirstNode;
	}
	template<class T>
	inline const DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetFirst() const
	{
		return myFirstNode;
	}
	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetLast()
	{
		return myLastNode;
	}
	template<class T>
	inline const DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetLast() const
	{
		return myLastNode;
	}
	template<class T>
	inline void DoublyLinkedList<T>::InsertFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* node = new DoublyLinkedListNode<T>(aValue);
		if (myFirstNode != nullptr)
		{
			node->myNextNode = myFirstNode;
			myFirstNode->myPreviousNode = node;
		}
		else
		{
			myLastNode = node;
		}
		myFirstNode = node;
		mySize++;
	}
	template<class T>
	inline void DoublyLinkedList<T>::InsertLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* node = new DoublyLinkedListNode<T>(aValue);
		if (myLastNode != nullptr)
		{
			node->myPreviousNode = myLastNode;
			myLastNode->myNextNode = node;
		}
		else
		{
			myFirstNode = node;
		}
		myLastNode = node;
		mySize++;
	}
	template<class T>
	inline void DoublyLinkedList<T>::InsertBefore(DoublyLinkedListNode<T>* aNode, const T& aValue)
	{
		assert(aNode != nullptr);
		if (aNode != nullptr)
		{
			DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>(aValue);
			newNode->myNextNode = aNode;
			newNode->myPreviousNode = aNode->myPreviousNode;
			if (aNode == myFirstNode)
			{
				myFirstNode = newNode;
			}
			else
			{
				aNode->myPreviousNode->myNextNode = newNode;
			}
			aNode->myPreviousNode = newNode;
			mySize++;
		}
	}
	template<class T>
	inline void DoublyLinkedList<T>::InsertAfter(DoublyLinkedListNode<T>* aNode, const T& aValue)
	{
		assert(aNode != nullptr);
		if (aNode != nullptr)
		{
			DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>(aValue);
			newNode->myPreviousNode = aNode;
			newNode->myNextNode = aNode->myNextNode;
			if (aNode == myLastNode)
			{
				myLastNode = newNode;
			}
			else
			{
				aNode->myNextNode->myPreviousNode = newNode;
			}
			aNode->myNextNode = newNode;
			mySize++;
		}
	}
	template<class T>
	inline void DoublyLinkedList<T>::Remove(DoublyLinkedListNode<T>* aNode)
	{
		assert(aNode != nullptr);
		if (aNode != nullptr)
		{
			if (aNode == myFirstNode)
			{
				myFirstNode = aNode->myNextNode;
			}
			else
			{
				aNode->myPreviousNode->myNextNode = aNode->myNextNode;
			}
			if (aNode == myLastNode)
			{
				myLastNode = aNode->myPreviousNode;
			}
			else
			{
				aNode->myNextNode->myPreviousNode = aNode->myPreviousNode;
			}
			aNode->myNextNode = nullptr;
			aNode->myPreviousNode = nullptr;
			delete aNode;
			mySize--;
		}
	}
	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* node = myFirstNode;
		while (node != nullptr)
		{
			if (node->myData == aValue)
			{
				return node;
			}
			node = node->myNextNode;
		}
		return nullptr;
	}
	template<class T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* node = myLastNode;
		while (node != nullptr)
		{
			if (node->myData == aValue)
			{
				return node;
			}
			node = node->myPreviousNode;
		}
		return nullptr;
	}
	template<class T>
	inline bool DoublyLinkedList<T>::RemoveFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* node = myFirstNode;
		while (node != nullptr)
		{
			if (node->myData == aValue)
			{
				Remove(node);
				return true;
			}
			node = node->myNextNode;
		}
		return false;
	}
	template<class T>
	inline bool DoublyLinkedList<T>::RemoveLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* node = myLastNode;
		while (node != nullptr)
		{
			if (node->myData == aValue)
			{
				Remove(node);
				return true;
			}
			node = node->myPreviousNode;
		}
		return false;
	}
}