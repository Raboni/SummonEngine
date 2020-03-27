#pragma once
#include <vector>
#include <limits>
#include <assert.h>
#undef max

namespace CU
{
	static const int MapWidth = 20;
	static const int MapHeight = 20;
	static const int TileCount = MapWidth * MapHeight;
	enum class Tile
	{
		Impassable,
		Passable,
		Slow,
		Fast
	};
	enum class Category
	{
		Unvisited,
		Open,
		Closed
	};
	struct NodeRecord
	{
		int predecessor;
		int g;
		int f;
		int index;
		Category category;
		Tile tile;
	};
	std::vector<int> AStar(const std::vector<Tile>&aMap, int aStartIndex, int aEndIndex);

	template<typename T>
	int FindIndex(const std::vector<T>& aArray, const T& aObject)
	{
		for (int index = 0; index < aArray.size(); index++)
		{
			if (aArray[index] == aObject)
			{
				return index;
			}
		}
		return -1;
	}
}