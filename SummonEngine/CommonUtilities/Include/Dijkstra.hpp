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
	struct DijkstraNode
	{
		unsigned int distance;
		DijkstraNode* predecessor;
		Tile tile;
	};

	std::vector<int> Dijkstra(const std::vector<Tile>& aMap, int aStartIndex, int aEndIndex);

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