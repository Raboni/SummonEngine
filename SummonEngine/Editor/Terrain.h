#pragma once
#include <GrowingArray.hpp>
#include <StaticArray.hpp>
#include <Vector.hpp>

class Terrain
{
public:
	Terrain();
	~Terrain();

	void Init();
	void LogState();

	void SetVertex(const CU::Vector3f aVertex, const int aVertexIndex);
	void MoveVertex(const CU::Vector3f aMovement, const int aVertexIndex);
	void MoveEdge(const CU::Vector3f aMovement, const int aEdgeIndex);
	void MoveTriangle(const CU::Vector3f aMovement, const int aTriangleIndex);
	void AddTriangleVertex(const CU::Vector3f aVertex, const int aTriangleIndex);
	void AddEdgeVertex(const CU::Vector3f aVertex, const int aEdgeIndex);
	void MergeVertices(const int aRemoveVertexIndex, const int aMergeVertexIndex);
	void ExtrudeTriangle(const CU::Vector3f aExtrudeVector, const CU::GrowingArray<int>& aTriangleIndexArray);
	void RemoveTriangle(const int aTriangleIndex);
	void RemoveEdge(const int aEdgeIndex);
	void RemoveVertex(const int aVertexIndex);

	int GetRayHitTriangleIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint) const;
	int GetRayHitEdgeIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint, const int aHitTriangleIndex = -1) const;
	int GetRayHitVertexIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint) const;

	CU::StaticArray<CU::Vector3f, 2> GetEdgeVertices(const int aEdgeIndex) const;
	CU::StaticArray<CU::Vector3f, 3> GetTriangleVertices(const int aTriangleIndex) const;
	CU::StaticArray<int, 2> GetEdgeVertexIndices(const int aEdgeIndex) const;
	CU::StaticArray<int, 3> GetTriangleVertexIndices(const int aTriangleIndex) const;

	const CU::GrowingArray<CU::Vector4f>& GetVertices() const;
	const CU::GrowingArray<unsigned int>& GetIndices() const;

private:
	struct Vertex
	{
	public:
		Vertex();
		void Reset();
		void AddEdge(const int aEdgeIndex);
		void RemoveEdge(const int aEdgeIndex);

		int GetEdgeCount() const;
		int GetEdgeIndex(const int aVertexEdgeIndex) const;

	private:
		int RemoveLastEdge();

	public:
		CU::GrowingArray<unsigned int> myEdgeIndexList;
		int myEdgeIndex[8];
		unsigned short myEdgeCount;
	};
	struct Edge
	{
		Edge()
		{
			myVertexIndex[0] = -1;
			myVertexIndex[1] = -1;
			myTriangleIndex[0] = -1;
			myTriangleIndex[1] = -1;
		}
		Edge(const int aVertexIndex1, const int aVertexIndex2)
		{
			myVertexIndex[0] = aVertexIndex1;
			myVertexIndex[1] = aVertexIndex2;
			myTriangleIndex[0] = -1;
			myTriangleIndex[1] = -1;
		}
		int myVertexIndex[2];
		int myTriangleIndex[2];
	};
	struct Triangle
	{
		Triangle()
		{
			myEdgeIndex[0] = -1;
			myEdgeIndex[1] = -1;
			myEdgeIndex[2] = -1;
			myVertexIndex[0] = -1;
			myVertexIndex[1] = -1;
			myVertexIndex[2] = -1;
			myIndicesIndex = -1;
		}
		Triangle(const int aEdge1, const int aEdge2, const int aEdge3)
		{
			myEdgeIndex[0] = aEdge1;
			myEdgeIndex[1] = aEdge2;
			myEdgeIndex[2] = aEdge3;
			myVertexIndex[0] = -1;
			myVertexIndex[1] = -1;
			myVertexIndex[2] = -1;
			myIndicesIndex = -1;
		}
		CU::Vector3f myPoint;
		int myEdgeIndex[3];
		int myVertexIndex[3];
		int myIndicesIndex;
	};

private:
	bool RayHitTriangleIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint, const int aTriangleIndex) const;

	int GetNewVertexIndex();
	int GetNewEdgeIndex();
	int GetNewTriangleIndex();
	void ReturnVertexIndex(const int aIndex);
	void ReturnEdgeIndex(const int aIndex);
	void ReturnTriangleIndex(const int aIndex);
	bool HasReturnedVertexIndex(const int aIndex) const;
	bool HasReturnedEdgeIndex(const int aIndex) const;
	bool HasReturnedTriangleIndex(const int aIndex) const;

	void Reset(Vertex& aVertex) const;
	void Reset(Edge& aEdge) const;
	void Reset(Triangle& aTriangle) const;

private:
	/*Rules/Assumptions:
	* Triangle.myVertexIndex are in the correct render order
	* Triangle.myEdgeIndex are in order starting with the edge of the first and the second vertex
	* Edge members are not guaranteed to be in a specific order
	* myMeshIndices only contain groups of three
	*/

	CU::GrowingArray<unsigned int> myMeshIndices;
	CU::GrowingArray<CU::Vector4f> myMeshVertices;
	CU::GrowingArray<Vertex> myVertexList;
	CU::GrowingArray<Edge> myEdgeList;
	CU::GrowingArray<Triangle> myTriangleList;
	CU::GrowingArray<int> myOpenVertexIndices;
	CU::GrowingArray<int> myOpenEdgeIndices;
	CU::GrowingArray<int> myOpenTriangleIndices;
};