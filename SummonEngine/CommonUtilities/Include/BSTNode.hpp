#pragma once

namespace CU
{
	template<typename T>
	class BSTSet;
	template<typename T>
	class BSTSetNode
	{
	public:
		BSTSetNode(const BSTSetNode&) = delete;
		BSTSetNode &operator=(const BSTSetNode &) = delete;

		const T& GetData() const;
		const BSTSetNode<T>* GetLeft() const;
		const BSTSetNode<T>* GetRight() const;

	private:
		BSTSetNode<T>() {}
		BSTSetNode(T aValue);
		~BSTSetNode();
		friend class BSTSet<T>;

		T myData;
		BSTSetNode* myLeft;
		BSTSetNode* myRight;
	};

	template<typename T>
	inline const T& BSTSetNode<T>::GetData() const
	{
		return myData;
	}

	template<typename T>
	inline const BSTSetNode<T>* BSTSetNode<T>::GetLeft() const
	{
		return myLeft;
	}

	template<typename T>
	inline const BSTSetNode<T>* BSTSetNode<T>::GetRight() const
	{
		return myRight;
	}

	template<typename T>
	inline BSTSetNode<T>::BSTSetNode(T aValue)
	{
		myData = aValue;
		myLeft = nullptr;
		myRight = nullptr;
	}
	template<typename T>
	inline BSTSetNode<T>::~BSTSetNode()
	{
		delete myLeft;
		myLeft = nullptr;
		delete myRight;
		myRight = nullptr;
	}
}