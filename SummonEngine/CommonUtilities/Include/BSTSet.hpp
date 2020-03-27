#pragma once
#include "BSTNode.hpp"
#include <assert.h>
#include <math.h>

namespace CU
{
	template<typename T>
	class BSTSet
	{
	public:
		BSTSet();
		~BSTSet();

		const BSTSetNode<T>* GetRoot() const;
		bool HasElement(const T& aValue);

		void Insert(const T& aValue);
		void Remove(const T& aValue);
		void RemoveSwitch(const T& aValue);//slower but more balanced
		void DSWBalance();

	private:
		unsigned int CreateBackbone();
		BSTSetNode<T>* FindNodeRecursive(BSTSetNode<T>* aCurrentNode, const T& aValue, BSTSetNode<T>** aParentNode = nullptr);
		BSTSetNode<T>* FindHighNodeRecursive(BSTSetNode<T>* aCurrentNode, BSTSetNode<T>** aParentNode = nullptr);
		void AddNodeRecursive(BSTSetNode<T>* aCurrentNode, const T& aValue);
		void RotateRight(BSTSetNode<T>* aParentNode, BSTSetNode<T>* aCurrentNode);
		void RotateLeft(BSTSetNode<T>* aParentNode, BSTSetNode<T>* aCurrentNode);

		BSTSetNode<T>* myRoot;
	};

	template<typename T>
	inline BSTSet<T>::BSTSet()
	{
		myRoot = nullptr;
	}
	template<typename T>
	inline BSTSet<T>::~BSTSet()
	{
		delete myRoot;
	}

	template<typename T>
	inline const BSTSetNode<T>* BSTSet<T>::GetRoot() const
	{
		return myRoot;
	}
	template<typename T>
	inline bool BSTSet<T>::HasElement(const T& aValue)
	{
		return (FindNodeRecursive(myRoot, aValue) != nullptr);
	}

	template<typename T>
	inline void BSTSet<T>::Insert(const T& aValue)
	{
		if (myRoot != nullptr)
		{
			AddNodeRecursive(myRoot, aValue);
		}
		else
		{
			myRoot = new BSTSetNode<T>(aValue);
		}
	}
	template<typename T>
	inline void BSTSet<T>::Remove(const T& aValue)
	{
		assert(myRoot != nullptr && "tree doesn't exist");
		if (myRoot != nullptr)
		{
			BSTSetNode<T>* parent = myRoot;
			BSTSetNode<T>* node = FindNodeRecursive(myRoot, aValue, &parent);
			bool parentOfRight = (parent->myRight == node);
			if (node != nullptr)
			{
				if (node == myRoot)
				{
					if (myRoot->myLeft != nullptr)
					{
						BSTSetNode<T>* high = FindHighNodeRecursive(myRoot->myLeft);
						BSTSetNode<T>* toDelete = myRoot;
						high->myRight = myRoot->myRight;
						myRoot = myRoot->myLeft;
						toDelete->myLeft = nullptr;
						toDelete->myRight = nullptr;
						delete toDelete;
					}
					else if (myRoot->myRight != nullptr)
					{
						BSTSetNode<T>* toDelete = myRoot;
						myRoot = myRoot->myRight;
						toDelete->myRight = nullptr;
						delete toDelete;
					}
					else
					{
						delete myRoot;
						myRoot = nullptr;
					}
				}
				else if (node->myLeft != nullptr)
				{
					BSTSetNode<T>* high = FindHighNodeRecursive(node->myLeft);
					if (!parentOfRight)
					{
						parent->myLeft = node->myLeft;
					}
					else
					{
						parent->myRight = node->myLeft;
					}
					high->myRight = node->myRight;
					node->myLeft = nullptr;
					node->myRight = nullptr;
					delete node;
				}
				else if (node->myRight != nullptr)
				{
					if (!parentOfRight)
					{
						parent->myLeft = node->myRight;
					}
					else
					{
						parent->myRight = node->myRight;
					}
					node->myRight = nullptr;
					delete node;
				}
				else
				{
					if (!parentOfRight)
					{
						delete parent->myLeft;
						parent->myLeft = nullptr;
					}
					else
					{
						delete parent->myRight;
						parent->myRight = nullptr;
					}
				}
			}
		}
	}
	template<typename T>
	inline void BSTSet<T>::RemoveSwitch(const T& aValue)
	{
		assert(myRoot != nullptr && "tree doesn't exist");
		if (myRoot != nullptr)
		{
			BSTSetNode<T>* parent = myRoot;
			BSTSetNode<T>* node = FindNodeRecursive(myRoot, aValue, &parent);
			bool parentOfRight = (parent->myRight == node);
			if (node != nullptr)
			{
				if (node == myRoot)
				{
					if (myRoot->myLeft != nullptr)
					{
						BSTSetNode<T>* highParent = myRoot;
						BSTSetNode<T>* high = FindHighNodeRecursive(myRoot->myLeft, &highParent);
						bool highParentOfRight = (highParent->myRight == high);
						myRoot->myData = high->myData;
						if (highParentOfRight)
						{
							highParent->myRight = high->myLeft;
						}
						else
						{
							highParent->myLeft = high->myLeft;
						}
						high->myLeft = nullptr;
						delete high;
					}
					else if (myRoot->myRight != nullptr)
					{
						BSTSetNode<T>* toDelete = myRoot;
						myRoot = myRoot->myRight;
						toDelete->myRight = nullptr;
						delete toDelete;
					}
					else
					{
						delete myRoot;
						myRoot = nullptr;
					}
				}
				else if (node->myLeft != nullptr)
				{
					BSTSetNode<T>* highParent = node;
					BSTSetNode<T>* high = FindHighNodeRecursive(node->myLeft, &highParent);
					bool highParentOfRight = (highParent->myRight == high);
					node->myData = high->myData;
					if (!highParentOfRight)
					{
						highParent->myLeft = high->myLeft;
					}
					else
					{
						highParent->myRight = high->myLeft;
					}
					high->myLeft = nullptr;
					delete high;
				}
				else if (node->myRight != nullptr)
				{
					if (!parentOfRight)
					{
						parent->myLeft = node->myRight;
					}
					else
					{
						parent->myRight = node->myRight;
					}
					node->myRight = nullptr;
					delete node;
				}
				else
				{
					if (!parentOfRight)
					{
						delete parent->myLeft;
						parent->myLeft = nullptr;
					}
					else
					{
						delete parent->myRight;
						parent->myRight = nullptr;
					}
				}
			}
		}
	}
	template<typename T>
	inline void BSTSet<T>::DSWBalance()
	{
		assert(myRoot != nullptr && "tree doesn't exist");
		if (myRoot != nullptr)
		{
			unsigned int n = CreateBackbone();
			int m = static_cast<int>(pow(2, static_cast<int>(log2(n + 1)))) - 1;
			int rotationAmount = n - m;
			BSTSetNode<T>* currentNodeParent = nullptr;
			BSTSetNode<T>* currentNode = myRoot;

			if (currentNode != nullptr)
			{
				for (int rotation = 0; rotation < rotationAmount; rotation++)
				{
					assert(currentNode != nullptr && currentNode->myRight != nullptr);
					RotateLeft(currentNodeParent, currentNode);
					if (currentNodeParent != nullptr)
					{
						currentNode = currentNodeParent->myRight;
					}
					else
					{
						currentNodeParent = nullptr;
						currentNode = myRoot;
					}

					if (currentNode != nullptr && currentNode->myRight != nullptr)
					{
						currentNodeParent = currentNode;
						currentNode = currentNode->myRight;
					}
				}

				while (m > 1)
				{
					currentNodeParent = nullptr;
					currentNode = myRoot;
					m /= 2;

					for (int rotation = 0; rotation < m; rotation++)
					{
						assert(currentNode != nullptr && currentNode->myRight != nullptr);
						RotateLeft(currentNodeParent, currentNode);
						if (currentNodeParent != nullptr)
						{
							currentNode = currentNodeParent->myRight;
						}
						else
						{
							currentNodeParent = nullptr;
							currentNode = myRoot;
						}

						if (currentNode != nullptr && currentNode->myRight != nullptr)
						{
							currentNodeParent = currentNode;
							currentNode = currentNode->myRight;
						}
					}
				}
			}
		}
	}
	template<typename T>
	inline unsigned int BSTSet<T>::CreateBackbone()
	{
		assert(myRoot != nullptr && "tree doesn't exist");
		if (myRoot != nullptr)
		{
			unsigned int amount = 0;
			BSTSetNode<T>* currentNodeParent = nullptr;
			BSTSetNode<T>* currentNode = myRoot;
			while (currentNode != nullptr)
			{
				if (currentNode->myLeft != nullptr)
				{
					RotateRight(currentNodeParent, currentNode);
					if (currentNodeParent != nullptr)
					{
						currentNode = currentNodeParent->myRight;
					}
					else
					{
						currentNode = myRoot;
					}
				}
				else
				{
					amount++;
					currentNodeParent = currentNode;
					currentNode = currentNode->myRight;
				}
			}
			return amount;
		}
		return 0;
	}

	template<typename T>
	inline BSTSetNode<T>* BSTSet<T>::FindNodeRecursive(BSTSetNode<T>* aCurrentNode, const T& aValue, BSTSetNode<T>** aParentNode)
	{
		if (aCurrentNode != nullptr)
		{
			if (aValue < aCurrentNode->myData)
			{
				if (aCurrentNode->myLeft != nullptr)
				{
					if (aParentNode != nullptr)
					{
						*aParentNode = aCurrentNode;
					}
					return FindNodeRecursive(aCurrentNode->myLeft, aValue, aParentNode);
				}
				return nullptr;
			}
			else if (aCurrentNode->myData < aValue)
			{
				if (aCurrentNode->myRight != nullptr)
				{
					if (aParentNode != nullptr)
					{
						*aParentNode = aCurrentNode;
					}
					return FindNodeRecursive(aCurrentNode->myRight, aValue, aParentNode);
				}
				return nullptr;
			}
			else
			{
				return aCurrentNode;
			}
		}
		return nullptr;
	}
	template<typename T>
	inline BSTSetNode<T>* BSTSet<T>::FindHighNodeRecursive(BSTSetNode<T>* aCurrentNode, BSTSetNode<T>** aParentNode)
	{
		assert(aCurrentNode != nullptr && "node doesn't exist");
		if (aCurrentNode != nullptr)
		{
			if (aCurrentNode->myRight != nullptr)
			{
				if (aParentNode != nullptr)
				{
					*aParentNode = aCurrentNode;
				}
				return FindHighNodeRecursive(aCurrentNode->myRight, aParentNode);
			}
			else
			{
				return aCurrentNode;
			}
		}
		return nullptr;
	}
	template<typename T>
	inline void BSTSet<T>::AddNodeRecursive(BSTSetNode<T>* aCurrentNode, const T& aValue)
	{
		assert(aCurrentNode != nullptr && "node doesn't exist");
		if (aCurrentNode != nullptr)
		{
			if (aValue < aCurrentNode->myData)
			{
				if (aCurrentNode->myLeft == nullptr)
				{
					aCurrentNode->myLeft = new BSTSetNode<T>(aValue);
				}
				else
				{
					AddNodeRecursive(aCurrentNode->myLeft, aValue);
				}
			}
			else if (aCurrentNode->myData < aValue)
			{
				if (aCurrentNode->myRight == nullptr)
				{
					aCurrentNode->myRight = new BSTSetNode<T>(aValue);
				}
				else
				{
					AddNodeRecursive(aCurrentNode->myRight, aValue);
				}
			}
		}
	}
	template<typename T>
	inline void BSTSet<T>::RotateRight(BSTSetNode<T>* aParentNode, BSTSetNode<T>* aCurrentNode)
	{
		assert(aCurrentNode != nullptr && "invalid node");
		if (aParentNode != nullptr)
		{
			BSTSetNode<T>* childNode = aCurrentNode->myLeft;
			assert(childNode != nullptr && "node has no left child");

			aParentNode->myRight = childNode;
			aCurrentNode->myLeft = childNode->myRight;
			childNode->myRight = aCurrentNode;
		}
		else
		{
			myRoot = aCurrentNode->myLeft;
			aCurrentNode->myLeft = myRoot->myRight;
			myRoot->myRight = aCurrentNode;
		}
	}
	template<typename T>
	inline void BSTSet<T>::RotateLeft(BSTSetNode<T>* aParentNode, BSTSetNode<T>* aCurrentNode)
	{
		assert(aCurrentNode != nullptr && "invalid node");
		if (aParentNode != nullptr)
		{
			BSTSetNode<T>* childNode = aCurrentNode->myRight;
			assert(childNode != nullptr && "node has no right child");

			aParentNode->myRight = childNode;
			aCurrentNode->myRight = childNode->myLeft;
			childNode->myLeft = aCurrentNode;
		}
		else
		{
			myRoot = aCurrentNode->myRight;
			aCurrentNode->myRight = myRoot->myLeft;
			myRoot->myLeft = aCurrentNode;
		}
	}
}