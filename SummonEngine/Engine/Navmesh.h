#pragma once
#undef min
#undef max
#include <Intersection.hpp>
#include <GrowingArray.hpp>
#include <Vector.hpp>
#include <string>

namespace Nav
{
	struct Vertex
	{
		Vertex() {}
		Vertex(const CU::Vector4f& aVector)
		{
			Position = aVector;
		}
		CU::Vector4f Position;
	};
	struct Edge
	{
		Edge()
		{
			VertexIndex[0] = -1;
			VertexIndex[1] = -1;
			TriangleIndex[0] = -1;
			TriangleIndex[1] = -1;
		}
		Edge(int aVertexIndex1, int aVertexIndex2)
		{
			VertexIndex[0] = aVertexIndex1;
			VertexIndex[1] = aVertexIndex2;
			TriangleIndex[0] = -1;
			TriangleIndex[1] = -1;
		}
		int VertexIndex[2];
		int TriangleIndex[2];
	};
	struct Triangle
	{
		Triangle()
		{
			EdgeIndex[0] = -1;
			EdgeIndex[1] = -1;
			EdgeIndex[2] = -1;
			VertexIndex[0] = -1;
			VertexIndex[1] = -1;
			VertexIndex[2] = -1;
			Fast = false;
		}
		Triangle(int aEdge1, int aEdge2, int aEdge3, bool aFast = false)
		{
			EdgeIndex[0] = aEdge1;
			EdgeIndex[1] = aEdge2;
			EdgeIndex[2] = aEdge3;
			VertexIndex[0] = -1;
			VertexIndex[1] = -1;
			VertexIndex[2] = -1;
			Fast = aFast;
		}
		CU::Vector3f Point;
		int EdgeIndex[3];
		int VertexIndex[3];
		bool Fast = false;
	};

	class Navmesh
	{
	public:
		Navmesh();
		~Navmesh();

		void Load(const std::string& aPath);
		CU::GrowingArray<CU::Vector3f> CalculatePath(int aStartIndex, int aEndIndex);
		int GetTriangleIndex(const CU::Intersection::LineRay3D& aRay);
		int GetTriangleIndex(const CU::Intersection::LineRay3D& aRay, CU::Vector3f& aOutPos);
		const CU::GrowingArray<Vertex>& GetVertexList();
		const CU::GrowingArray<Triangle>& GetTriangleList();

	private:
		friend class NavmeshRenderer;

		CU::GrowingArray<CU::Vector3f> GetVertexList(const Triangle& aTriangle);
		CU::GrowingArray<CU::Vector3f> IndexToPointList(const CU::GrowingArray<int>& aArray);
		bool RayIntersectsTriangle(const CU::Intersection::LineRay3D& aRay, const Triangle& aTriangle, CU::Vector3f& aOutPoint);

		CU::GrowingArray<Vertex> myVertexList;
		CU::GrowingArray<Edge> myEdgeList;
		CU::GrowingArray<Triangle> myTriangleList;
		CU::GrowingArray<unsigned int> myIndexList;
	};
}