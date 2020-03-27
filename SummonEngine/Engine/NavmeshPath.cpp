#include "stdafx.h"
#include "Navmesh.h"

namespace Nav
{
	const CU::GrowingArray<Vertex>& Navmesh::GetVertexList()
	{
		return myVertexList;
	}
	const CU::GrowingArray<Triangle>& Navmesh::GetTriangleList()
	{
		return myTriangleList;
	}

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
	int GetHeuristic(const Triangle& aTriangle, const CU::Vector3f& aEndPos)
	{
		float x = aTriangle.Point.x;
		float y = aTriangle.Point.z;
		float goalX = aEndPos.x;
		float goalY = aEndPos.z;

		return static_cast<int>((abs(goalX - x) + abs(goalY - y)) * 1000);
	}
	int GetMinF(CU::GrowingArray<NodeRecord*> aArray)
	{
		int min = std::numeric_limits<int>::max();
		int findIndex = -1;
		for (int index = 0; index < aArray.Size(); index++)
		{
			if (aArray[index]->f < min)
			{
				min = aArray[index]->f;
				findIndex = index;
			}
		}
		return findIndex;
	}
	bool FindInArray(const std::vector<NodeRecord*>& aArray, NodeRecord * aNode)
	{
		for (size_t index = 0; index < aArray.size(); index++)
		{
			if (aArray[index] == aNode)
			{
				return true;
			}
		}
		return false;
	}
	unsigned int GetWeightAStar(Tile aTile)
	{
		switch (aTile)
		{
		case Tile::Passable:
			return 2;
		case Tile::Slow:
			return 3;
		case Tile::Fast:
			return 1;
		default:
			return 0;
		}
	}
	CU::GrowingArray<int> FindRouteBack(const CU::GrowingArray<NodeRecord>& aArray, int aEndIndex)
	{
		assert(aArray.Size() > aEndIndex && aEndIndex >= 0 && "out of scope");
		if (aArray.Size() > aEndIndex && aEndIndex >= 0 && aArray[aEndIndex].predecessor != -1)
		{
			CU::GrowingArray<int> revesedRoute(aArray.Size());
			NodeRecord node = aArray[aEndIndex];
			revesedRoute.Add(aEndIndex);
			while (node.predecessor != -1)
			{
				node = aArray[node.predecessor];
				revesedRoute.Add(node.index);
			}

			//std::reverse(revesedRoute.begin(), revesedRoute.end());
			revesedRoute.Reverse();
			return revesedRoute;
		}
		return CU::GrowingArray<int>();
	}

	CU::GrowingArray<CU::Vector3f> Navmesh::CalculatePath(int aStartIndex, int aEndIndex)
	{
		if (aStartIndex == -1 || aEndIndex == -1)
		{
			return CU::GrowingArray<CU::Vector3f>(1);
		}
		CU::GrowingArray<NodeRecord> allNodes(32);
		CU::GrowingArray<NodeRecord*> closedNodes(16);
		CU::GrowingArray<NodeRecord*> openNodes(16);

		for (int index = 0; index < myTriangleList.Size(); index++)
		{
			allNodes.Add(NodeRecord());
			allNodes.GetLast().g = std::numeric_limits<int>::max();
			allNodes.GetLast().f = std::numeric_limits<int>::max();
			allNodes.GetLast().index = index;
			allNodes.GetLast().category = Category::Unvisited;
			allNodes.GetLast().predecessor = -1;
			if (myTriangleList[index].Fast)
			{
				allNodes.GetLast().tile = Tile::Fast;
			}
			else
			{
				allNodes.GetLast().tile = Tile::Passable;
			}
		}

		allNodes[aStartIndex].category = Category::Open;
		allNodes[aStartIndex].g = 0;
		allNodes[aStartIndex].f = GetHeuristic(myTriangleList[aStartIndex], myTriangleList[aEndIndex].Point);
		openNodes.Add(&allNodes[aStartIndex]);

		while (openNodes.Size() > 0)
		{
			int currentNodeOpenIndex = GetMinF(openNodes);
			if (currentNodeOpenIndex == -1)
			{
				break;
			}
			else if (openNodes[currentNodeOpenIndex] == &allNodes[aEndIndex])
			{
				return IndexToPointList(FindRouteBack(allNodes, aEndIndex));
			}

			NodeRecord* currentNode = openNodes[currentNodeOpenIndex];
			if (currentNode->category == Category::Closed)
			{
				continue;
			}
			else
			{
				closedNodes.Add(openNodes[currentNodeOpenIndex]);
				openNodes.RemoveCyclicAtIndex(currentNodeOpenIndex);
				currentNode->category = Category::Closed;
			}

			CU::GrowingArray<NodeRecord*> adjacent(3);
			int allNodesIndex = currentNode->index;
			for (int edge = 0; edge < 3; edge++)
			{
				int* triangles = myEdgeList[myTriangleList[allNodesIndex].EdgeIndex[edge]].TriangleIndex;
				for (int i = 0; i < 2; i++)
				{
					if (triangles[i] != -1 && triangles[i] != allNodesIndex)
					{
						adjacent.Add(&allNodes[triangles[i]]);
						break;
					}
				}
			}

			for (int index = 0; index < adjacent.Size(); index++)
			{
				if (adjacent[index]->tile == Tile::Impassable || adjacent[index]->category == Category::Closed)
				{
					adjacent[index]->category = Category::Closed;
					continue;
				}
				else if (openNodes.Find(adjacent[index]) == -1)
				{
					openNodes.Add(adjacent[index]);
				}

				int tentG = currentNode->g + GetWeightAStar(adjacent[index]->tile);
				if (tentG >= adjacent[index]->g)
				{
					continue;
				}

				adjacent[index]->predecessor = currentNode->index;
				adjacent[index]->g = tentG;
				adjacent[index]->f = tentG + GetHeuristic(myTriangleList[adjacent[index]->index], myTriangleList[aEndIndex].Point);
			}
		}
		return IndexToPointList(FindRouteBack(allNodes, aEndIndex));
	}
	int Navmesh::GetTriangleIndex(const CU::Intersection::LineRay3D& aRay)
	{
		CU::Vector3f outPos;
		for (int triangleIndex = 0; triangleIndex < myTriangleList.Size(); triangleIndex++)
		{
			if (RayIntersectsTriangle(aRay, myTriangleList[triangleIndex], outPos))
			{
				return triangleIndex;
			}
		}
		return -1;
	}
	int Navmesh::GetTriangleIndex(const CU::Intersection::LineRay3D& aRay, CU::Vector3f& aOutPos)
	{
		CU::Vector3f outPos;
		for (int triangleIndex = 0; triangleIndex < myTriangleList.Size(); triangleIndex++)
		{
			if (RayIntersectsTriangle(aRay, myTriangleList[triangleIndex], outPos))
			{
				aOutPos = outPos;
				return triangleIndex;
			}
		}
		return -1;
	}
	CU::GrowingArray<CU::Vector3f> Navmesh::GetVertexList(const Triangle& aTriangle)
	{
		CU::GrowingArray<CU::Vector3f> vertexList(3);
		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
		{
			vertexList.Add(myVertexList[aTriangle.VertexIndex[vertexIndex]].Position);
		}
		return vertexList;
	}
	CU::GrowingArray<CU::Vector3f> Navmesh::IndexToPointList(const CU::GrowingArray<int>& aArray)
	{
		CU::GrowingArray<CU::Vector3f> pointList(aArray.Size());
		for (int i = 1; i < aArray.Size(); i++)
		{
			pointList.Add(myTriangleList[aArray[i]].Point);
		}
		return pointList;
	}
	bool Navmesh::RayIntersectsTriangle(const CU::Intersection::LineRay3D& aRay, const Triangle& aTriangle, CU::Vector3f& aOutPoint)
	{
		constexpr float EPSILON = 0.0000001f;
		CU::GrowingArray<CU::Vector3f> vertexList(GetVertexList(aTriangle));
		CU::Vector3f edge1;
		CU::Vector3f edge2;
		CU::Vector3f h;
		CU::Vector3f s;
		CU::Vector3f q;
		float a;
		float f;
		float u;
		float v;
		edge1 = vertexList[1] - vertexList[0];
		edge2 = vertexList[2] - vertexList[0];
		h = aRay.direction.Cross(edge2);
		a = edge1.Dot(h);
		if (a > -EPSILON && a < EPSILON)
		{
			return false;    // This ray is parallel to this triangle.
		}
		f = 1.0f / a;
		s = aRay.point - vertexList[0];
		u = f * (s.Dot(h));
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}
		q = s.Cross(edge1);
		v = f * aRay.direction.Dot(q);
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}
		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = f * edge2.Dot(q);
		if (t > EPSILON) // ray intersection
		{
			aOutPoint = aRay.point + aRay.direction * t;
			return true;
		}

		// This means that there is a line intersection but not a ray intersection.
		return false;
	}
}