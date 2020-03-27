#pragma once
#include "GrowingArray.hpp"
#include <vector>
#include <assert.h>

namespace CU
{
	template<class T>
	inline void SelectionSort(std::vector<T>& aVector)
	{
		assert(aVector.size() > 0 && "empty");
		int unsortedIndex = 0;
		int min = unsortedIndex;
		while (unsortedIndex < static_cast<int>(aVector.size()))
		{
			min = unsortedIndex;
			for (int i = unsortedIndex; i < static_cast<int>(aVector.size()); i++)
			{
				if (aVector[static_cast<size_t>(i)] < aVector[static_cast<size_t>(min)])
				{
					min = i;
				}
			}
			if (unsortedIndex != min)
			{
				std::swap(aVector[static_cast<size_t>(unsortedIndex)], aVector[static_cast<size_t>(min)]);
			}
			unsortedIndex++;
		}
	}
	template<class T, typename SizeType = int>
	inline void SelectionSort(GrowingArray<T, SizeType>& aArray)
	{
		assert(aArray.size() > 0 && "empty");
		int unsortedIndex = 0;
		int min = unsortedIndex;
		while (unsortedIndex < static_cast<int>(aArray.Size()))
		{
			min = unsortedIndex;
			for (int i = unsortedIndex; i < static_cast<int>(aArray.Size()); i++)
			{
				if (aArray[static_cast<SizeType>(i)] < aArray[static_cast<SizeType>(min)])
				{
					min = i;
				}
			}
			if (unsortedIndex != min)
			{
				std::swap(aArray[static_cast<SizeType>(unsortedIndex)], aArray[static_cast<SizeType>(min)]);
			}
			unsortedIndex++;
		}
	}

	template<class T>
	inline void BubbleSort(std::vector<T>& aVector)
	{
		for (size_t i = 0; i < aVector.size(); i++)
		{
			for (size_t j = 0; j < aVector.size() - 1; j++)
			{
				if (aVector[j + 1] < aVector[j])
				{
					T temp = aVector[j + 1];
					aVector[j + 1] = aVector[j];
					aVector[j] = temp;
				}
			}
		}
	}
	template<class T, typename SizeType = int>
	inline void BubbleSort(GrowingArray<T, SizeType>& aArray)
	{
		for (SizeType i = 0; i < aArray.size(); i++)
		{
			for (SizeType j = 0; j < aArray.size() - 1; j++)
			{
				if (aArray[j + 1] < aArray[j])
				{
					T temp = aArray[j + 1];
					aArray[j + 1] = aArray[j];
					aArray[j] = temp;
				}
			}
		}
	}

	template<class T>
	void QuickSortRecursive(std::vector<T>& aVector, const int& aLow, const int& aHigh)
	{
		if (aLow < aHigh)
		{
			size_t pivot = aHigh;
			int i = aLow - 1;
			for (size_t j = aLow; j < aHigh; j++)
			{
				if (aVector[j] < aVector[pivot])
				{
					std::swap(aVector[++i], aVector[j]);
				}
			}
			std::swap(aVector[++i], aVector[aHigh]);

			QuickSortRecursive(aVector, aLow, i - 1);
			QuickSortRecursive(aVector, i + 1, aHigh);
		}
	}
	template<class T>
	inline void QuickSort(std::vector<T>& aVector)
	{
		assert(aVector.size() > 0 && "empty");
		QuickSortRecursive(aVector, 0, static_cast<int>(aVector.size() - 1));
	}

	template<class T>
	std::vector<T> Merge(const std::vector<T>& aVectorLeft, const std::vector<T>& aVectorRight)
	{
		std::vector<T> list;
		size_t indexLeft = 0;
		size_t indexRight = 0;

		while (indexLeft < aVectorLeft.size() && indexRight < aVectorRight.size())
		{
			if (aVectorLeft[indexLeft] < aVectorRight[indexRight])
			{
				list.push_back(aVectorLeft[indexLeft++]);
			}
			else
			{
				list.push_back(aVectorRight[indexRight++]);
			}
		}
		while (indexLeft < aVectorLeft.size())
		{
			list.push_back(aVectorLeft[indexLeft++]);
		}
		while (indexRight < aVectorRight.size())
		{
			list.push_back(aVectorRight[indexRight++]);
		}

		return list;
	}
	template<class T>
	std::vector<T> MergeSortRecursive(std::vector<T>& aVector)
	{
		if (aVector.size() <= 1)
		{
			return aVector;
		}

		std::vector<T> left;
		std::vector<T> right;
		for (size_t i = 0; i < aVector.size(); i++)
		{
			if (i < aVector.size() / 2)
			{
				left.push_back(aVector[i]);
			}
			else
			{
				right.push_back(aVector[i]);
			}
		}
		left = MergeSortRecursive(left);
		right = MergeSortRecursive(right);

		return Merge(left, right);
	}
	template<class T>
	inline void MergeSort(std::vector<T>& aVector)
	{
		assert(aVector.size() > 0 && "empty");
		aVector = MergeSortRecursive(aVector);
	}

	template<class T>
	inline void InsertionSort(std::vector<T>& aVector)
	{
		for (int i = 1; i < static_cast<int>(aVector.size()); i++)
		{
			T tempVal = aVector[static_cast<size_t>(i)];
			int index = i - 1;

			while (index >= 0 && tempVal < aVector[static_cast<size_t>(index)])
			{
				aVector[static_cast<size_t>(index + 1)] = aVector[static_cast<size_t>(index)];
				index--;
			}

			aVector[static_cast<size_t>(index + 1)] = tempVal;
		}
	}
	template<class T, typename SizeType = int>
	inline void InsertionSort(GrowingArray<T, SizeType>& aArray)
	{
		for (int i = 1; i < static_cast<int>(aArray.Size()); i++)
		{
			T tempVal(aArray[static_cast<SizeType>(i)]);
			int index = i - 1;

			while (index >= 0 && tempVal < aArray[static_cast<SizeType>(index)])
			{
				aArray[static_cast<SizeType>(index + 1)] = aArray[static_cast<SizeType>(index)];
				index--;
			}

			aArray[static_cast<SizeType>(index + 1)] = tempVal;
		}
	}
}