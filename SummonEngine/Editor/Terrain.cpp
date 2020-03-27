#include "stdafx.h"
#include "Terrain.h"
#include <DebugTools.h>

constexpr float Epsilon = 0.000001f;

Terrain::Terrain()
{
	myMeshIndices.Init(512);
	myMeshVertices.Init(512);
	myVertexList.Init(512);
	myEdgeList.Init(512);
	myTriangleList.Init(512);
	myOpenVertexIndices.Init(32);
	myOpenEdgeIndices.Init(32);
	myOpenTriangleIndices.Init(32);
}
Terrain::~Terrain()
{
}

void Terrain::Init()
{
	myMeshIndices.Resize(6);
	myMeshIndices[0] = 0;
	myMeshIndices[1] = 1;
	myMeshIndices[2] = 2;
	myMeshIndices[3] = 0;
	myMeshIndices[4] = 2;
	myMeshIndices[5] = 3;

	myMeshVertices.Resize(4);
	myMeshVertices[0] = CU::Vector4f(1.0f, 0.0f, -1.0f, 1.0f);
	myMeshVertices[1] = CU::Vector4f(-1.0f, 0.0f, -1.0f, 1.0f);
	myMeshVertices[2] = CU::Vector4f(-1.0f, 0.0f, 1.0f, 1.0f);
	myMeshVertices[3] = CU::Vector4f(1.0f, 0.0f, 1.0f, 1.0f);

	myVertexList.Resize(4);
	myVertexList[0].Reset();
	myVertexList[0].myEdgeIndex[0] = 3;
	myVertexList[0].myEdgeIndex[1] = 4;
	myVertexList[0].myEdgeIndex[2] = 0;
	myVertexList[0].myEdgeCount = 3;
	myVertexList[1].Reset();
	myVertexList[1].myEdgeIndex[0] = 0;
	myVertexList[1].myEdgeIndex[1] = 1;
	myVertexList[1].myEdgeCount = 2;
	myVertexList[2].Reset();
	myVertexList[2].myEdgeIndex[0] = 1;
	myVertexList[2].myEdgeIndex[1] = 4;
	myVertexList[2].myEdgeIndex[2] = 2;
	myVertexList[2].myEdgeCount = 3;
	myVertexList[3].Reset();
	myVertexList[3].myEdgeIndex[0] = 2;
	myVertexList[3].myEdgeIndex[1] = 3;
	myVertexList[3].myEdgeCount = 2;

	myEdgeList.Resize(5);
	myEdgeList[0].myVertexIndex[0] = 0;
	myEdgeList[0].myVertexIndex[1] = 1;
	myEdgeList[0].myTriangleIndex[0] = 0;
	myEdgeList[1].myVertexIndex[0] = 1;
	myEdgeList[1].myVertexIndex[1] = 2;
	myEdgeList[1].myTriangleIndex[0] = 0;
	myEdgeList[2].myVertexIndex[0] = 2;
	myEdgeList[2].myVertexIndex[1] = 3;
	myEdgeList[2].myTriangleIndex[0] = 1;
	myEdgeList[3].myVertexIndex[0] = 3;
	myEdgeList[3].myVertexIndex[1] = 0;
	myEdgeList[3].myTriangleIndex[0] = 1;
	myEdgeList[4].myVertexIndex[0] = 0;
	myEdgeList[4].myVertexIndex[1] = 2;
	myEdgeList[4].myTriangleIndex[0] = 0;
	myEdgeList[4].myTriangleIndex[1] = 1;

	myTriangleList.Resize(2);
	CU::Vector3f middlePoint;
	for (int i = 0; i < 3; i++)
	{
		const int index = static_cast<int>(myMeshIndices[i]);
		myTriangleList[0].myVertexIndex[i] = index;
		middlePoint += CU::Vector3f(myMeshVertices[index]);
	}
	myTriangleList[0].myPoint = middlePoint / 3.0f;
	myTriangleList[0].myEdgeIndex[0] = 0;
	myTriangleList[0].myEdgeIndex[1] = 1;
	myTriangleList[0].myEdgeIndex[2] = 4;
	myTriangleList[0].myIndicesIndex = 0;
	middlePoint = CU::Vector3f::Zero;
	for (int i = 0; i < 3; i++)
	{
		const int index = static_cast<int>(myMeshIndices[3 + i]);
		myTriangleList[1].myVertexIndex[i] = index;
		middlePoint += CU::Vector3f(myMeshVertices[index]);
	}
	myTriangleList[1].myPoint = middlePoint / 3.0f;
	myTriangleList[1].myEdgeIndex[0] = 4;
	myTriangleList[1].myEdgeIndex[1] = 2;
	myTriangleList[1].myEdgeIndex[2] = 3;
	myTriangleList[1].myIndicesIndex = 3;

	myOpenVertexIndices.RemoveAll();
	myOpenEdgeIndices.RemoveAll();
	myOpenTriangleIndices.RemoveAll();
}

void Terrain::LogState()
{
	for (int vertexIndex = 0; vertexIndex < myVertexList.Size(); vertexIndex++)
	{
		if (HasReturnedVertexIndex(vertexIndex))
		{
			continue;
		}

		const CU::Vector3f position = myMeshVertices[vertexIndex];
		const Vertex& vertex = myVertexList[vertexIndex];
		std::string logString("Vertex");
		logString += std::to_string(vertexIndex) + " - X:" + std::to_string(position.x) + " Y:" + std::to_string(position.y) + " Z:" + std::to_string(position.z) + " Edges:";
		for (int vertexEdgeIndex = 0; vertexEdgeIndex < vertex.GetEdgeCount(); vertexEdgeIndex++)
		{
			if (vertexEdgeIndex != 0)
			{
				logString += ',';
			}
			logString += std::to_string(vertex.GetEdgeIndex(vertexEdgeIndex));
		}
		CU::Log(logString);
	}

	for (int edgeIndex = 0; edgeIndex < myEdgeList.Size(); edgeIndex++)
	{
		if (HasReturnedEdgeIndex(edgeIndex))
		{
			continue;
		}

		const Edge& edge = myEdgeList[edgeIndex];
		std::string logString("Edge");
		logString += std::to_string(edgeIndex) + " - Vertices:";
		for (int edgeVertexIndex = 0; edgeVertexIndex < 2; edgeVertexIndex++)
		{
			if (edgeVertexIndex != 0)
			{
				logString += ',';
			}
			logString += std::to_string(edge.myVertexIndex[edgeVertexIndex]);
		}
		logString += " Triangles:";
		for (int edgeTriangleIndex = 0; edgeTriangleIndex < 2; edgeTriangleIndex++)
		{
			if (edgeTriangleIndex != 0)
			{
				logString += ',';
			}
			logString += std::to_string(edge.myTriangleIndex[edgeTriangleIndex]);
		}
		CU::Log(logString);
	}

	for (int triangleIndex = 0; triangleIndex < myTriangleList.Size(); triangleIndex++)
	{
		if (HasReturnedTriangleIndex(triangleIndex))
		{
			continue;
		}

		const Triangle& triangle = myTriangleList[triangleIndex];
		std::string logString("Triangle");
		logString += std::to_string(triangleIndex) + " - Vertices:";
		for (int triangleVertexIndex = 0; triangleVertexIndex < 3; triangleVertexIndex++)
		{
			if (triangleVertexIndex != 0)
			{
				logString += ',';
			}
			logString += std::to_string(triangle.myVertexIndex[triangleVertexIndex]);
		}
		logString += " Edges:";
		for (int triangleEdgeIndex = 0; triangleEdgeIndex < 3; triangleEdgeIndex++)
		{
			if (triangleEdgeIndex != 0)
			{
				logString += ',';
			}
			logString += std::to_string(triangle.myEdgeIndex[triangleEdgeIndex]);
		}
		CU::Log(logString);
	}
}

void Terrain::SetVertex(const CU::Vector3f aVertex, const int aVertexIndex)
{
	if (aVertexIndex == -1)
	{
		return;
	}
	myMeshVertices[aVertexIndex] = CU::Vector4f(aVertex, 1.0f);
}

void Terrain::MoveVertex(const CU::Vector3f aMovement, const int aVertexIndex)
{
	if (aVertexIndex == -1)
	{
		return;
	}
	myMeshVertices[aVertexIndex] += CU::Vector4f(aMovement, 0.0f);
}
void Terrain::MoveEdge(const CU::Vector3f aMovement, const int aEdgeIndex)
{
	if (aEdgeIndex == -1)
	{
		return;
	}
	const CU::StaticArray<int, 2> vertexIndices(myEdgeList[aEdgeIndex].myVertexIndex);
	myMeshVertices[vertexIndices[0]] += CU::Vector4f(aMovement, 0.0f);
	myMeshVertices[vertexIndices[1]] += CU::Vector4f(aMovement, 0.0f);
}
void Terrain::MoveTriangle(const CU::Vector3f aMovement, const int aTriangleIndex)
{
	if (aTriangleIndex == -1)
	{
		return;
	}
	const CU::StaticArray<int, 3> vertexIndices(myTriangleList[aTriangleIndex].myVertexIndex);
	myMeshVertices[vertexIndices[0]] += CU::Vector4f(aMovement, 0.0f);
	myMeshVertices[vertexIndices[1]] += CU::Vector4f(aMovement, 0.0f);
	myMeshVertices[vertexIndices[2]] += CU::Vector4f(aMovement, 0.0f);
}

void Terrain::AddTriangleVertex(const CU::Vector3f aVertex, const int aTriangleIndex)
{
	if (aTriangleIndex == -1)
	{
		return;
	}

	const int newVertexIndex = GetNewVertexIndex();
	myMeshVertices[newVertexIndex] = CU::Vector4f(aVertex, 1.0f);

	Triangle& oldTriangle = myTriangleList[aTriangleIndex];
	myMeshIndices[oldTriangle.myIndicesIndex + 2] = static_cast<unsigned int>(newVertexIndex);

	int newTriangleIndices[3];
	newTriangleIndices[0] = aTriangleIndex;
	newTriangleIndices[1] = GetNewTriangleIndex();
	newTriangleIndices[2] = GetNewTriangleIndex();

	int newEdgeIndices[3];
	for (int i = 0; i < 3; i++)
	{
		newEdgeIndices[i] = GetNewEdgeIndex();
		Edge* newEdge = &myEdgeList[newEdgeIndices[i]];

		newEdge->myVertexIndex[0] = oldTriangle.myVertexIndex[i];
		newEdge->myVertexIndex[1] = newVertexIndex;
		newEdge->myTriangleIndex[0] = newTriangleIndices[(i + 2) % 3];
		newEdge->myTriangleIndex[1] = newTriangleIndices[i % 3];

		myVertexList[newVertexIndex].AddEdge(newEdgeIndices[i]);
		myVertexList[oldTriangle.myVertexIndex[i]].AddEdge(newEdgeIndices[i]);
	}

	for (int i = 1; i < 3; i++)
	{
		if (oldTriangle.myEdgeIndex[i] != -1)
		{
			Edge& oldEdge = myEdgeList[oldTriangle.myEdgeIndex[i]];
			if (oldEdge.myTriangleIndex[0] == aTriangleIndex)
			{
				oldEdge.myTriangleIndex[0] = newTriangleIndices[i];
			}
			else if (oldEdge.myTriangleIndex[1] == aTriangleIndex)
			{
				oldEdge.myTriangleIndex[1] = newTriangleIndices[i];
			}
			else
			{
				//mesh corrupted
				CU::Log("Edge with wrong members found on terrain", CU::LogType::Error);
			}
		}
	}

	CU::Vector3f middlePoint;
	int vertexIndex[3];
	Triangle* newTriangle = nullptr;
	newTriangle = &myTriangleList[newTriangleIndices[1]];
	newTriangle->myEdgeIndex[0] = oldTriangle.myEdgeIndex[1];
	newTriangle->myEdgeIndex[1] = newEdgeIndices[2];
	newTriangle->myEdgeIndex[2] = newEdgeIndices[1];
	newTriangle->myIndicesIndex = myMeshIndices.Size();
	vertexIndex[0] = oldTriangle.myVertexIndex[1];
	vertexIndex[1] = oldTriangle.myVertexIndex[2];
	vertexIndex[2] = newVertexIndex;
	for (int i = 0; i < 3; i++)
	{
		newTriangle->myVertexIndex[i] = vertexIndex[i];
		middlePoint += CU::Vector3f(myMeshVertices[vertexIndex[i]]);
		myMeshIndices.Add(vertexIndex[i]);
	}
	newTriangle->myPoint = middlePoint / 3.0f;
	middlePoint = CU::Vector3f::Zero;

	newTriangle = &myTriangleList[newTriangleIndices[2]];
	newTriangle->myEdgeIndex[0] = oldTriangle.myEdgeIndex[2];
	newTriangle->myEdgeIndex[1] = newEdgeIndices[0];
	newTriangle->myEdgeIndex[2] = newEdgeIndices[2];
	newTriangle->myIndicesIndex = myMeshIndices.Size();
	vertexIndex[0] = oldTriangle.myVertexIndex[2];
	vertexIndex[1] = oldTriangle.myVertexIndex[0];
	vertexIndex[2] = newVertexIndex;
	for (int i = 0; i < 3; i++)
	{
		newTriangle->myVertexIndex[i] = vertexIndex[i];
		middlePoint += CU::Vector3f(myMeshVertices[vertexIndex[i]]);
		myMeshIndices.Add(vertexIndex[i]);
	}
	newTriangle->myPoint = middlePoint / 3.0f;
	middlePoint = CU::Vector3f::Zero;

	oldTriangle.myVertexIndex[2] = newVertexIndex;
	oldTriangle.myEdgeIndex[1] = newEdgeIndices[1];
	oldTriangle.myEdgeIndex[2] = newEdgeIndices[0];
	for (int i = 0; i < 3; i++)
	{
		middlePoint += CU::Vector3f(myMeshVertices[oldTriangle.myVertexIndex[i]]);
	}
	oldTriangle.myPoint = middlePoint / 3.0f;
}

void Terrain::AddEdgeVertex(const CU::Vector3f aVertex, const int aEdgeIndex)
{
	if (aEdgeIndex == -1)
	{
		return;
	}

	const int newVertexIndex = GetNewVertexIndex();
	myMeshVertices[newVertexIndex] = CU::Vector4f(aVertex, 1.0f);

	const int newSplitEdgeIndex = GetNewEdgeIndex();
	Edge& newSplitEdge = myEdgeList[newSplitEdgeIndex];
	newSplitEdge.myVertexIndex[0] = -1;
	newSplitEdge.myVertexIndex[1] = newVertexIndex;
	newSplitEdge.myTriangleIndex[0] = -1;
	Edge& oldSplitEdge = myEdgeList[aEdgeIndex];
	newSplitEdge.myTriangleIndex[1] = oldSplitEdge.myTriangleIndex[1];

	Vertex& newVertex = myVertexList[newVertexIndex];
	newVertex.AddEdge(aEdgeIndex);
	newVertex.AddEdge(newSplitEdgeIndex);

	bool hasFixedSplitEdgeVertex = false;
	for (int edgeTriangleIndex = 0; edgeTriangleIndex < 2; edgeTriangleIndex++)
	{
		const int oldTriangleIndex = oldSplitEdge.myTriangleIndex[edgeTriangleIndex];
		if (oldTriangleIndex == -1)
		{
			continue;
		}
		Triangle& oldTriangle = myTriangleList[oldTriangleIndex];

		int oldTriangleSplitEdgeIndex = -1;
		for (int i = 0; i < 3; i++)
		{
			if (oldTriangle.myEdgeIndex[i] == aEdgeIndex)
			{
				oldTriangleSplitEdgeIndex = i;
				break;
			}
		}
		if (oldTriangleSplitEdgeIndex == -1)
		{
			//mesh corrupted
			CU::Log("Triangle/edge with wrong members found on terrain", CU::LogType::Error);
			if (edgeTriangleIndex == 0)
			{
				//a small chance this will save the mesh from being corrupted
				ReturnVertexIndex(newVertexIndex);
				ReturnEdgeIndex(newSplitEdgeIndex);
			}
			return;
		}
		if (!hasFixedSplitEdgeVertex)
		{
			hasFixedSplitEdgeVertex = true;
			const int lostVertexIndex = oldTriangle.myVertexIndex[oldTriangleSplitEdgeIndex];
			myVertexList[lostVertexIndex].RemoveEdge(aEdgeIndex);
			myVertexList[lostVertexIndex].AddEdge(newSplitEdgeIndex);

			newSplitEdge.myVertexIndex[0] = lostVertexIndex;
			if (oldSplitEdge.myVertexIndex[0] == lostVertexIndex)
			{
				oldSplitEdge.myVertexIndex[0] = newVertexIndex;
			}
			else
			{
				oldSplitEdge.myVertexIndex[1] = newVertexIndex;
			}
		}
		const int newTriangleIndex = GetNewTriangleIndex();
		if (edgeTriangleIndex == 0)
		{
			newSplitEdge.myTriangleIndex[0] = newTriangleIndex;
		}
		else if (edgeTriangleIndex == 1)
		{
			oldSplitEdge.myTriangleIndex[1] = newTriangleIndex;
		}
		
		myMeshIndices[oldTriangle.myIndicesIndex + oldTriangleSplitEdgeIndex] = static_cast<unsigned int>(newVertexIndex);

		const int oldTriangleLostBorderEdgeIndex = (oldTriangleSplitEdgeIndex + 2) % 3;
		const int notSplitVertexIndex = oldTriangle.myVertexIndex[oldTriangleLostBorderEdgeIndex];
		const int newEdgeIndex = GetNewEdgeIndex();
		Edge& newEdge = myEdgeList[newEdgeIndex];
		newEdge.myVertexIndex[0] = notSplitVertexIndex;
		newEdge.myVertexIndex[1] = newVertexIndex;
		newEdge.myTriangleIndex[0] = oldTriangleIndex;
		newEdge.myTriangleIndex[1] = newTriangleIndex;

		newVertex.AddEdge(newEdgeIndex);
		myVertexList[notSplitVertexIndex].AddEdge(newEdgeIndex);

		Edge& lostBorderEdge = myEdgeList[oldTriangle.myEdgeIndex[oldTriangleLostBorderEdgeIndex]];
		if (lostBorderEdge.myTriangleIndex[0] == oldTriangleIndex)
		{
			lostBorderEdge.myTriangleIndex[0] = newTriangleIndex;
		}
		else if (lostBorderEdge.myTriangleIndex[1] == oldTriangleIndex)
		{
			lostBorderEdge.myTriangleIndex[1] = newTriangleIndex;
		}
		else
		{
			//mesh corrupted
			CU::Log("Edge with wrong members found on terrain", CU::LogType::Error);
		}

		int vertexIndex[3];
		Triangle& newTriangle = myTriangleList[newTriangleIndex];
		newTriangle.myEdgeIndex[0] = newEdgeIndex;
		newTriangle.myEdgeIndex[1] = oldTriangle.myEdgeIndex[oldTriangleLostBorderEdgeIndex];
		if (edgeTriangleIndex == 0)
		{
			newTriangle.myEdgeIndex[2] = newSplitEdgeIndex;
		}
		else
		{
			newTriangle.myEdgeIndex[2] = oldTriangle.myEdgeIndex[oldTriangleSplitEdgeIndex];
		}
		newTriangle.myIndicesIndex = myMeshIndices.Size();
		vertexIndex[0] = newVertexIndex;
		for (int i = 1; i < 3; i++)
		{
			vertexIndex[i] = oldTriangle.myVertexIndex[(oldTriangleSplitEdgeIndex + 1 + i) % 3];
		}
		
		CU::Vector3f middlePoint;
		for (int i = 0; i < 3; i++)
		{
			newTriangle.myVertexIndex[i] = vertexIndex[i];
			middlePoint += CU::Vector3f(myMeshVertices[vertexIndex[i]]);
			myMeshIndices.Add(vertexIndex[i]);
		}
		newTriangle.myPoint = middlePoint / 3.0f;

		if (edgeTriangleIndex != 0)
		{
			oldTriangle.myEdgeIndex[oldTriangleSplitEdgeIndex] = newSplitEdgeIndex;
		}
		oldTriangle.myEdgeIndex[oldTriangleLostBorderEdgeIndex] = newEdgeIndex;
		oldTriangle.myVertexIndex[oldTriangleSplitEdgeIndex] = newVertexIndex;
		middlePoint = CU::Vector3f::Zero;
		for (int i = 0; i < 3; i++)
		{
			middlePoint += CU::Vector3f(myMeshVertices[oldTriangle.myVertexIndex[i]]);
		}
		oldTriangle.myPoint = middlePoint / 3.0f;
	}
	if (!hasFixedSplitEdgeVertex)
	{
		ReturnVertexIndex(newVertexIndex);
		ReturnEdgeIndex(newSplitEdgeIndex);
	}
}

void Terrain::MergeVertices(const int aRemoveVertexIndex, const int aMergeVertexIndex)
{
	if (aRemoveVertexIndex == -1 || aMergeVertexIndex == -1)
	{
		return;
	}

	CU::GrowingArray<int> thirdPartyVertexIndexList(8);
	CU::GrowingArray<int> thirdPartyEdgeIndexList(8);
	const CU::Vector4f mergePosition(myMeshVertices[aMergeVertexIndex]);
	Vertex& mergeVertex = myVertexList[aMergeVertexIndex];
	Vertex& removeVertex = myVertexList[aRemoveVertexIndex];
	for (int mergeVertexEdgeIndex = 0; mergeVertexEdgeIndex < mergeVertex.GetEdgeCount(); mergeVertexEdgeIndex++)
	{
		const int mergeEdgeIndex = mergeVertex.GetEdgeIndex(mergeVertexEdgeIndex);
		if (mergeEdgeIndex == -1)
		{
			continue;
		}

		Edge& mergeEdge = myEdgeList[mergeEdgeIndex];
		const int otherMergeEdgeVertexIndex = mergeEdge.myVertexIndex[0] == aMergeVertexIndex ? 1 : 0;
		const int otherMergeVertexIndex = mergeEdge.myVertexIndex[otherMergeEdgeVertexIndex];
		if (otherMergeVertexIndex == aRemoveVertexIndex)
		{
			thirdPartyEdgeIndexList.Add(mergeEdgeIndex);
			continue;
		}

		for (int removeVertexEdgeIndex = 0; removeVertexEdgeIndex < removeVertex.GetEdgeCount(); removeVertexEdgeIndex++)
		{
			const int removeEdgeIndex = removeVertex.GetEdgeIndex(removeVertexEdgeIndex);
			if (removeEdgeIndex == -1)
			{
				continue;
			}

			Edge& removeEdge = myEdgeList[removeEdgeIndex];
			const int otherRemoveVertexIndex = removeEdge.myVertexIndex[0] == aRemoveVertexIndex ? removeEdge.myVertexIndex[1] : removeEdge.myVertexIndex[0];
			if (otherRemoveVertexIndex == otherMergeVertexIndex)
			{
				thirdPartyVertexIndexList.Add(otherMergeVertexIndex);
				thirdPartyEdgeIndexList.Add(removeEdgeIndex);
				thirdPartyEdgeIndexList.Add(mergeEdgeIndex);
			}
		}
	}

	//Remove all triangles consisting of ONLY thirdPartyVertexIndexList or merge vertices.
	//Then replace every instance of aRemoveVertexIndex with aMergeVertexIndex (remember to check for duplicated edges).
	for (int index = 0; index < thirdPartyEdgeIndexList.Size(); index++)
	{
		const int edgeIndex = thirdPartyEdgeIndexList[index];
		if (HasReturnedEdgeIndex(edgeIndex))
		{
			continue;
		}

		Edge& edge = myEdgeList[edgeIndex];
		for (int edgeTriangleIndex = 0; edgeTriangleIndex < 2; edgeTriangleIndex++)
		{
			const int triangleIndex = edge.myTriangleIndex[edgeTriangleIndex];
			if (triangleIndex == -1)
			{
				continue;
			}

			Triangle& triangle = myTriangleList[triangleIndex];
			bool shouldRemoveTriangle = true;
			for (int triangleVertexIndex = 0; triangleVertexIndex < 3; triangleVertexIndex++)
			{
				const int vertexIndex = triangle.myVertexIndex[triangleVertexIndex];
				if (vertexIndex == aRemoveVertexIndex || vertexIndex == aMergeVertexIndex)
				{
					continue;
				}
				if (thirdPartyVertexIndexList.Find(vertexIndex) == -1)
				{
					shouldRemoveTriangle = false;
					break;
				}
			}
			if (shouldRemoveTriangle)
			{
				RemoveTriangle(triangleIndex);
			}
		}
	}

	if (HasReturnedVertexIndex(aRemoveVertexIndex))
	{
		return;
	}
	else if (HasReturnedVertexIndex(aMergeVertexIndex))
	{
		myMeshVertices[aRemoveVertexIndex] = mergePosition;
		return;
	}

	//replace every instance of aRemoveVertexIndex in triangles and edges with aMergeVertexIndex
	for (int vertexEdgeIndex = 0; vertexEdgeIndex < removeVertex.GetEdgeCount(); vertexEdgeIndex++)
	{
		const int edgeIndex = removeVertex.GetEdgeIndex(vertexEdgeIndex);
		if (edgeIndex == -1)
		{
			continue;
		}

		Edge& edge = myEdgeList[edgeIndex];
		//find replacement edge
		int replacementEdgeIndex = -1;
		const int otherVertexIndex = edge.myVertexIndex[0] == aRemoveVertexIndex ? edge.myVertexIndex[1] : edge.myVertexIndex[0];
		if (otherVertexIndex == -1)
		{
			continue;
		}
		for (int mergeVertexEdgeIndex = 0; mergeVertexEdgeIndex < mergeVertex.GetEdgeCount(); mergeVertexEdgeIndex++)
		{
			const int mergeEdgeIndex = mergeVertex.GetEdgeIndex(mergeVertexEdgeIndex);
			if (mergeEdgeIndex == -1)
			{
				continue;
			}

			Edge& mergeEdge = myEdgeList[mergeEdgeIndex];
			if (mergeEdge.myVertexIndex[0] == otherVertexIndex || mergeEdge.myVertexIndex[1] == otherVertexIndex)
			{
				replacementEdgeIndex = mergeEdgeIndex;
				break;
			}
		}

		if (replacementEdgeIndex != -1)
		{
			myVertexList[otherVertexIndex].RemoveEdge(edgeIndex);
			myVertexList[otherVertexIndex].AddEdge(replacementEdgeIndex);
		}

		//fix triangles
		for (int edgeTriangleIndex = 0; edgeTriangleIndex < 2; edgeTriangleIndex++)
		{
			const int triangleIndex = edge.myTriangleIndex[edgeTriangleIndex];
			if (triangleIndex == -1)
			{
				continue;
			}

			Triangle& triangle = myTriangleList[triangleIndex];
			for (int triangleVertexIndex = 0; triangleVertexIndex < 3; triangleVertexIndex++)
			{
				if (triangle.myVertexIndex[triangleVertexIndex] == aRemoveVertexIndex)
				{
					triangle.myVertexIndex[triangleVertexIndex] = aMergeVertexIndex;
					myMeshIndices[triangle.myIndicesIndex + triangleVertexIndex] = aMergeVertexIndex;
					break;
				}
			}

			if (replacementEdgeIndex != -1)
			{
				for (int triangleEdgeIndex = 0; triangleEdgeIndex < 3; triangleEdgeIndex++)
				{
					if (triangle.myEdgeIndex[triangleEdgeIndex] == edgeIndex)
					{
						triangle.myEdgeIndex[triangleEdgeIndex] = replacementEdgeIndex;
						break;
					}
				}
			}
		}

		if (replacementEdgeIndex != -1)
		{
			ReturnEdgeIndex(edgeIndex);
		}
		else
		{
			if (edge.myVertexIndex[0] == aRemoveVertexIndex)
			{
				edge.myVertexIndex[0] = aMergeVertexIndex;
			}
			else
			{
				edge.myVertexIndex[1] = aMergeVertexIndex;
			}
		}
	}
	ReturnVertexIndex(aRemoveVertexIndex);
}

void Terrain::ExtrudeTriangle(const CU::Vector3f aExtrudeVector, const CU::GrowingArray<int>& aTriangleIndexArray)
{
	CU::GrowingArray<int> oldVertexNewVertexConnectedEdgeIndexArray(96);
	for (int arrayIndex = 0; arrayIndex < aTriangleIndexArray.Size(); arrayIndex++)
	{
		const int triangleIndex = aTriangleIndexArray[arrayIndex];
		if (triangleIndex == -1)
		{
			continue;
		}

		Triangle& extrudedTriangle = myTriangleList[triangleIndex];
		for (int triangleEdgeIndex = 0; triangleEdgeIndex < 3; triangleEdgeIndex++)
		{
			const int oldEdgeIndex = extrudedTriangle.myEdgeIndex[triangleEdgeIndex];
			if (oldEdgeIndex == -1)
			{
				continue;
			}

			Edge& oldEdge = myEdgeList[oldEdgeIndex];
			int edgeTriangleIndexToReplace = -1;
			if (oldEdge.myTriangleIndex[0] == -1 || aTriangleIndexArray.Find(oldEdge.myTriangleIndex[0]) == -1)
			{
				edgeTriangleIndexToReplace = 1;
			}
			else if (oldEdge.myTriangleIndex[1] == -1 || aTriangleIndexArray.Find(oldEdge.myTriangleIndex[1]) == -1)
			{
				edgeTriangleIndexToReplace = 0;
			}

			if (edgeTriangleIndexToReplace != -1)
			{
				const int newEdgeIndex = GetNewEdgeIndex();
				extrudedTriangle.myEdgeIndex[triangleEdgeIndex] = newEdgeIndex;
				Edge& newEdge = myEdgeList[newEdgeIndex];
				Reset(newEdge);
				newEdge.myTriangleIndex[edgeTriangleIndexToReplace] = oldEdge.myTriangleIndex[edgeTriangleIndexToReplace];//could be the same as triangleIndex
				oldEdge.myTriangleIndex[edgeTriangleIndexToReplace] = -1;//will later be replaced with new triangle

				//extrude edge
				int newExtrudeEdgeIndices[2];
				newExtrudeEdgeIndices[0] = -1;
				newExtrudeEdgeIndices[1] = -1;
				for (int edgeVertexIndex = 0; edgeVertexIndex < 2; edgeVertexIndex++)
				{
					const int oldVertexIndex = oldEdge.myVertexIndex[edgeVertexIndex];
					if (oldVertexIndex == -1)
					{
						//mesh corrupted
						newEdge.myVertexIndex[edgeVertexIndex] = -1;
						CU::Log("Edge with invalid vertex index found", CU::LogType::Error);
						continue;
					}

					const int foundOldVertex = oldVertexNewVertexConnectedEdgeIndexArray.Find(oldVertexIndex);
					if (foundOldVertex != -1)
					{
						const int foundNewVertexIndex = oldVertexNewVertexConnectedEdgeIndexArray[foundOldVertex + 1];
						newEdge.myVertexIndex[edgeVertexIndex] = foundNewVertexIndex;
						myVertexList[foundNewVertexIndex].AddEdge(newEdgeIndex);
						newExtrudeEdgeIndices[edgeVertexIndex] = oldVertexNewVertexConnectedEdgeIndexArray[foundOldVertex + 2];
						continue;
					}

					const int newVertexIndex = GetNewVertexIndex();
					myMeshVertices[newVertexIndex] = myMeshVertices[oldVertexIndex];
					newEdge.myVertexIndex[edgeVertexIndex] = newVertexIndex;

					for (int extrudedListIndex = 0; extrudedListIndex < aTriangleIndexArray.Size(); extrudedListIndex++)
					{
						const int extrudedListTriangleIndex = aTriangleIndexArray[extrudedListIndex];
						Triangle& extrudedListTriangle = myTriangleList[extrudedListTriangleIndex];
						for (int triangleVertexIndex = 0; triangleVertexIndex < 3; triangleVertexIndex++)
						{
							if (extrudedListTriangle.myVertexIndex[triangleVertexIndex] == oldVertexIndex)
							{
								extrudedListTriangle.myVertexIndex[triangleVertexIndex] = newVertexIndex;
								myMeshIndices[extrudedListTriangle.myIndicesIndex + triangleVertexIndex] = newVertexIndex;
								break;
							}
						}
						for (int extrudedListTriangleEdgeIndex = 0; extrudedListTriangleEdgeIndex < 3; extrudedListTriangleEdgeIndex++)
						{
							const int extrudedEdgeIndex = extrudedListTriangle.myEdgeIndex[extrudedListTriangleEdgeIndex];
							Edge& extrudedListTriangleEdge = myEdgeList[extrudedEdgeIndex];
							int replaceEdgeVertexIndex = -1;
							if (extrudedListTriangleEdge.myVertexIndex[0] == oldVertexIndex)
							{
								replaceEdgeVertexIndex = 0;
							}
							else if (extrudedListTriangleEdge.myVertexIndex[1] == oldVertexIndex)
							{
								replaceEdgeVertexIndex = 1;
							}

							if (replaceEdgeVertexIndex != -1)
							{
								if (extrudedListTriangleEdge.myTriangleIndex[0] != -1 && aTriangleIndexArray.Find(extrudedListTriangleEdge.myTriangleIndex[0]) != -1 &&
									extrudedListTriangleEdge.myTriangleIndex[1] != -1 && aTriangleIndexArray.Find(extrudedListTriangleEdge.myTriangleIndex[1]) != -1)
								{
									extrudedListTriangleEdge.myVertexIndex[replaceEdgeVertexIndex] = newVertexIndex;
								}
							}
						}
					}

					newExtrudeEdgeIndices[edgeVertexIndex] = GetNewEdgeIndex();
					Edge& newExtrudeEdge = myEdgeList[newExtrudeEdgeIndices[edgeVertexIndex]];
					newExtrudeEdge.myVertexIndex[0] = oldVertexIndex;
					newExtrudeEdge.myVertexIndex[1] = newVertexIndex;
					newExtrudeEdge.myTriangleIndex[0] = -1;
					newExtrudeEdge.myTriangleIndex[1] = -1;

					Vertex& newVertex = myVertexList[newVertexIndex];
					newVertex.Reset();
					newVertex.AddEdge(newEdgeIndex);
					newVertex.AddEdge(newExtrudeEdgeIndices[edgeVertexIndex]);
					Vertex& oldVertex = myVertexList[oldVertexIndex];
					for (int vertexEdgeIndex = 0; vertexEdgeIndex < oldVertex.GetEdgeCount(); vertexEdgeIndex++)
					{
						const int edgeIndexToCheck = oldVertex.GetEdgeIndex(vertexEdgeIndex);
						if (edgeIndexToCheck == -1)
						{
							continue;
						}

						Edge& edgeToCheck = myEdgeList[edgeIndexToCheck];
						if (aTriangleIndexArray.Find(edgeToCheck.myTriangleIndex[0]) != -1 && aTriangleIndexArray.Find(edgeToCheck.myTriangleIndex[1]) != -1)
						{
							oldVertex.RemoveEdge(edgeIndexToCheck);
							newVertex.AddEdge(edgeIndexToCheck);
						}
					}
					oldVertex.AddEdge(newExtrudeEdgeIndices[edgeVertexIndex]);

					oldVertexNewVertexConnectedEdgeIndexArray.Add(oldVertexIndex);
					oldVertexNewVertexConnectedEdgeIndexArray.Add(newVertexIndex);
					oldVertexNewVertexConnectedEdgeIndexArray.Add(newExtrudeEdgeIndices[edgeVertexIndex]);
				}

				if (newExtrudeEdgeIndices[0] != -1 && newExtrudeEdgeIndices[1] != -1)
				{
					Edge& firstExtrudeEdge = myEdgeList[newExtrudeEdgeIndices[0]];
					Edge& secondExtrudeEdge = myEdgeList[newExtrudeEdgeIndices[1]];
					const int firstExtrudeTriangleIndex = GetNewTriangleIndex();
					const int secondExtrudeTriangleIndex = GetNewTriangleIndex();
					Triangle& firstExtrudeTriangle = myTriangleList[firstExtrudeTriangleIndex];
					Triangle& secondExtrudeTriangle = myTriangleList[secondExtrudeTriangleIndex];
					Reset(firstExtrudeTriangle);
					Reset(secondExtrudeTriangle);

					int firstEdgeTriangleVertexIndex = -1;
					int secondEdgeTriangleVertexIndex = -1;
					for (int triangleVertexIndex = 0; triangleVertexIndex < 3; triangleVertexIndex++)
					{
						if (extrudedTriangle.myVertexIndex[triangleVertexIndex] == firstExtrudeEdge.myVertexIndex[1])
						{
							firstEdgeTriangleVertexIndex = triangleVertexIndex;
						}
						else if (extrudedTriangle.myVertexIndex[triangleVertexIndex] == secondExtrudeEdge.myVertexIndex[1])
						{
							secondEdgeTriangleVertexIndex = triangleVertexIndex;
						}
					}
					const bool firstEdgeFirst = secondEdgeTriangleVertexIndex == (firstEdgeTriangleVertexIndex + 1) % 3;

					if (firstEdgeFirst)
					{
						firstExtrudeTriangle.myVertexIndex[0] = firstExtrudeEdge.myVertexIndex[0];
						firstExtrudeTriangle.myVertexIndex[1] = secondExtrudeEdge.myVertexIndex[0];
						firstExtrudeTriangle.myVertexIndex[2] = secondExtrudeEdge.myVertexIndex[1];
						secondExtrudeTriangle.myVertexIndex[0] = firstExtrudeEdge.myVertexIndex[0];
						secondExtrudeTriangle.myVertexIndex[1] = secondExtrudeEdge.myVertexIndex[1];
						secondExtrudeTriangle.myVertexIndex[2] = firstExtrudeEdge.myVertexIndex[1];
						firstExtrudeEdge.myTriangleIndex[1] = secondExtrudeTriangleIndex;
						secondExtrudeEdge.myTriangleIndex[0] = firstExtrudeTriangleIndex;
					}
					else
					{
						firstExtrudeTriangle.myVertexIndex[0] = secondExtrudeEdge.myVertexIndex[0];
						firstExtrudeTriangle.myVertexIndex[1] = firstExtrudeEdge.myVertexIndex[0];
						firstExtrudeTriangle.myVertexIndex[2] = firstExtrudeEdge.myVertexIndex[1];
						secondExtrudeTriangle.myVertexIndex[0] = secondExtrudeEdge.myVertexIndex[0];
						secondExtrudeTriangle.myVertexIndex[1] = firstExtrudeEdge.myVertexIndex[1];
						secondExtrudeTriangle.myVertexIndex[2] = secondExtrudeEdge.myVertexIndex[1];
						firstExtrudeEdge.myTriangleIndex[0] = firstExtrudeTriangleIndex;
						secondExtrudeEdge.myTriangleIndex[1] = secondExtrudeTriangleIndex;
					}

					firstExtrudeTriangle.myIndicesIndex = myMeshIndices.Size();
					for (int triangleVertexIndex = 0; triangleVertexIndex < 3; triangleVertexIndex++)
					{
						myMeshIndices.Add(firstExtrudeTriangle.myVertexIndex[triangleVertexIndex]);
					}
					secondExtrudeTriangle.myIndicesIndex = myMeshIndices.Size();
					for (int triangleVertexIndex = 0; triangleVertexIndex < 3; triangleVertexIndex++)
					{
						myMeshIndices.Add(secondExtrudeTriangle.myVertexIndex[triangleVertexIndex]);
					}

					const int divideExtrudeEdgeIndex = GetNewEdgeIndex();
					Edge& divideExtrudeEdge = myEdgeList[divideExtrudeEdgeIndex];
					divideExtrudeEdge.myVertexIndex[0] = firstExtrudeEdge.myVertexIndex[0];
					divideExtrudeEdge.myVertexIndex[1] = secondExtrudeTriangle.myVertexIndex[1];
					divideExtrudeEdge.myTriangleIndex[0] = firstExtrudeTriangleIndex;
					divideExtrudeEdge.myTriangleIndex[1] = secondExtrudeTriangleIndex;

					firstExtrudeTriangle.myEdgeIndex[0] = oldEdgeIndex;
					firstExtrudeTriangle.myEdgeIndex[1] = newExtrudeEdgeIndices[static_cast<int>(firstEdgeFirst)];
					firstExtrudeTriangle.myEdgeIndex[2] = divideExtrudeEdgeIndex;
					secondExtrudeTriangle.myEdgeIndex[0] = divideExtrudeEdgeIndex;
					secondExtrudeTriangle.myEdgeIndex[1] = newEdgeIndex;
					secondExtrudeTriangle.myEdgeIndex[2] = newExtrudeEdgeIndices[(static_cast<int>(firstEdgeFirst) + 1) % 2];
					oldEdge.myTriangleIndex[edgeTriangleIndexToReplace] = firstExtrudeTriangleIndex;
					newEdge.myTriangleIndex[(edgeTriangleIndexToReplace + 1) % 2] = secondExtrudeTriangleIndex;
				}
			}
		}
	}

	for (int arrayIndex = 0; arrayIndex < aTriangleIndexArray.Size(); arrayIndex++)
	{
		const int triangleIndex = aTriangleIndexArray[arrayIndex];
		if (triangleIndex == -1)
		{
			continue;
		}

		MoveTriangle(aExtrudeVector, triangleIndex);
	}
}

void Terrain::RemoveTriangle(const int aTriangleIndex)
{
	if (HasReturnedTriangleIndex(aTriangleIndex))
	{
		return;
	}

	Triangle& triangleToRemove = myTriangleList[aTriangleIndex];
	if (triangleToRemove.myIndicesIndex != -1)
	{
		myMeshIndices.RemoveCyclicAtIndex(triangleToRemove.myIndicesIndex + 2);
		myMeshIndices.RemoveCyclicAtIndex(triangleToRemove.myIndicesIndex + 1);
		myMeshIndices.RemoveCyclicAtIndex(triangleToRemove.myIndicesIndex);
		for (int triangleIndex = 0; triangleIndex < myTriangleList.Size(); triangleIndex++)
		{
			if (myTriangleList[triangleIndex].myIndicesIndex == myMeshIndices.Size())
			{
				myTriangleList[triangleIndex].myIndicesIndex = triangleToRemove.myIndicesIndex;
				break;
			}
		}
	}

	for (int triangleEdgeIndex = 0; triangleEdgeIndex < 3; triangleEdgeIndex++)
	{
		const int edgeIndex = triangleToRemove.myEdgeIndex[triangleEdgeIndex];
		if (edgeIndex == -1)
		{
			continue;
		}

		Edge& edge = myEdgeList[edgeIndex];
		if (edge.myTriangleIndex[0] == aTriangleIndex)
		{
			edge.myTriangleIndex[0] = -1;
		}
		else if (edge.myTriangleIndex[1] == aTriangleIndex)
		{
			edge.myTriangleIndex[1] = -1;
		}

		if (edge.myTriangleIndex[0] == -1 && edge.myTriangleIndex[1] == -1)
		{
			for (int edgeVertexIndex = 0; edgeVertexIndex < 2; edgeVertexIndex++)
			{
				const int vertexIndex = edge.myVertexIndex[edgeVertexIndex];
				if (vertexIndex == -1)
				{
					continue;
				}

				myVertexList[vertexIndex].RemoveEdge(edgeIndex);
				if (myVertexList[vertexIndex].GetEdgeCount() == 0)
				{
					RemoveVertex(vertexIndex);
				}
			}
			edge.myVertexIndex[0] = -1;
			edge.myVertexIndex[1] = -1;
			ReturnEdgeIndex(edgeIndex);
		}
	}
	triangleToRemove.myEdgeIndex[0] = -1;
	triangleToRemove.myEdgeIndex[1] = -1;
	triangleToRemove.myEdgeIndex[2] = -1;
	triangleToRemove.myVertexIndex[0] = -1;
	triangleToRemove.myVertexIndex[1] = -1;
	triangleToRemove.myVertexIndex[2] = -1;
	triangleToRemove.myIndicesIndex = -1;
	ReturnTriangleIndex(aTriangleIndex);
}

void Terrain::RemoveEdge(const int aEdgeIndex)
{
	if (HasReturnedEdgeIndex(aEdgeIndex))
	{
		return;
	}

	Edge& edgeToRemove = myEdgeList[aEdgeIndex];
	for (int edgeTriangleIndex = 0; edgeTriangleIndex < 2; edgeTriangleIndex++)
	{
		const int triangleIndex = edgeToRemove.myTriangleIndex[edgeTriangleIndex];
		if (triangleIndex == -1)
		{
			continue;
		}

		RemoveTriangle(triangleIndex);
	}
	for (int edgeVertexIndex = 0; edgeVertexIndex < 2; edgeVertexIndex++)
	{
		const int vertexIndex = edgeToRemove.myVertexIndex[edgeVertexIndex];
		if (vertexIndex == -1)
		{
			continue;
		}

		Vertex& vertex = myVertexList[vertexIndex];
		vertex.RemoveEdge(aEdgeIndex);
		if (vertex.myEdgeCount == 0)
		{
			RemoveVertex(vertexIndex);
		}
	}
}

void Terrain::RemoveVertex(const int aVertexIndex)
{
	if (HasReturnedVertexIndex(aVertexIndex))
	{
		return;
	}

	Vertex& vertexToRemove = myVertexList[aVertexIndex];
	for (int vertexEdgeIndex = 0; vertexEdgeIndex < vertexToRemove.myEdgeCount; vertexEdgeIndex++)
	{
		const int edgeIndex = vertexToRemove.myEdgeIndex[vertexEdgeIndex];
		if (edgeIndex == -1)
		{
			continue;
		}

		const Edge& edge = myEdgeList[edgeIndex];
		if (edge.myVertexIndex[0] == aVertexIndex || edge.myVertexIndex[1] == aVertexIndex)
		{
			RemoveEdge(edgeIndex);
		}
	}
	if (vertexToRemove.myEdgeIndexList.Size() > 0)
	{
		for (int listIndex = 0; listIndex < vertexToRemove.myEdgeIndexList.Size(); listIndex++)
		{
			const int edgeIndex = vertexToRemove.myEdgeIndexList[listIndex];
			if (edgeIndex == -1)
			{
				continue;
			}

			const Edge& edge = myEdgeList[edgeIndex];
			if (edge.myVertexIndex[0] == aVertexIndex || edge.myVertexIndex[1] == aVertexIndex)
			{
				RemoveEdge(edgeIndex);
			}
		}
	}
	ReturnVertexIndex(aVertexIndex);
}

int Terrain::GetRayHitTriangleIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint) const
{
	for (int triangleIndex = 0; triangleIndex < myTriangleList.Size(); triangleIndex++)
	{
		if (RayHitTriangleIndex(aRayStart, aRayVector, outHitPoint, triangleIndex))
		{
			return triangleIndex;
		}
	}
	return -1;
}

int Terrain::GetRayHitEdgeIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint, const int aHitTriangleIndex) const
{
	CU::Vector3f triangleHitPoint;
	int hitTriangle = -1;
	if (aHitTriangleIndex != -1 && RayHitTriangleIndex(aRayStart, aRayVector, triangleHitPoint, aHitTriangleIndex))
	{
		hitTriangle = aHitTriangleIndex;
	}
	else
	{
		hitTriangle = GetRayHitTriangleIndex(aRayStart, aRayVector, triangleHitPoint);
	}
	if (hitTriangle == -1)
	{
		return -1;
	}

	CU::Vector3f closestEdgePoint;
	int closestEdgeIndex = -1;
	float closestDistance = 0.0f;
	for (int triangleEdgeIndex = 0; triangleEdgeIndex < 3; triangleEdgeIndex++)
	{
		const int edgeIndex = myTriangleList[hitTriangle].myEdgeIndex[triangleEdgeIndex];
		if (edgeIndex == -1 || HasReturnedEdgeIndex(edgeIndex))
		{
			continue;
		}

		const Edge& edge = myEdgeList[edgeIndex];
		const CU::Vector3f edgeVertex(myMeshVertices[edge.myVertexIndex[0]]);
		CU::Vector3f edgeDirection(edgeVertex - CU::Vector3f(myMeshVertices[edge.myVertexIndex[1]]));
		edgeDirection.Normalize();
		const CU::Vector3f toHitPoint(triangleHitPoint - edgeVertex);
		const CU::Vector3f closestPoint(edgeVertex + edgeDirection * edgeDirection.Dot(toHitPoint));
		const float distanceToClosestPoint = (triangleHitPoint - closestPoint).LengthSqr();
		if (distanceToClosestPoint < 0.03f && (closestEdgeIndex == -1 || distanceToClosestPoint < closestDistance))
		{
			closestDistance = distanceToClosestPoint;
			closestEdgeIndex = edgeIndex;
			closestEdgePoint = closestPoint;
		}
	}
	if (closestEdgeIndex == -1)
	{
		return -1;
	}

	outHitPoint = closestEdgePoint;
	return closestEdgeIndex;
}

int Terrain::GetRayHitVertexIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint) const
{
	const CU::Vector3f rayDirection(aRayVector.GetNormalized());
	int closestVertexIndex = -1;
	float closestDistance = 0.0f;
	for (int vertexIndex = 0; vertexIndex < myMeshVertices.Size(); vertexIndex++)
	{
		if (HasReturnedVertexIndex(vertexIndex))
		{
			continue;
		}

		const CU::Vector3f vertex(myMeshVertices[vertexIndex]);
		const float dot = rayDirection.Dot(CU::Vector3f(myMeshVertices[vertexIndex]) - aRayStart);
		const CU::Vector3f closestPoint(aRayStart + rayDirection * dot);
		const float distanceToClosestPoint = (vertex - closestPoint).LengthSqr();
		if (distanceToClosestPoint < 0.03f && (closestVertexIndex == -1 || distanceToClosestPoint < closestDistance))
		{
			closestDistance = distanceToClosestPoint;
			closestVertexIndex = vertexIndex;
		}
	}
	if (closestVertexIndex == -1)
	{
		return -1;
	}

	outHitPoint = myMeshVertices[closestVertexIndex];
	return closestVertexIndex;
}

CU::StaticArray<CU::Vector3f, 2> Terrain::GetEdgeVertices(const int aEdgeIndex) const
{
	if (aEdgeIndex == -1)
	{
		return CU::StaticArray<CU::Vector3f, 2>();
	}

	const Edge& edge = myEdgeList[aEdgeIndex];
	CU::StaticArray<CU::Vector3f, 2> vertexArray;
	vertexArray[0] = myMeshVertices[edge.myVertexIndex[0]];
	vertexArray[1] = myMeshVertices[edge.myVertexIndex[1]];
	return vertexArray;
}
CU::StaticArray<CU::Vector3f, 3> Terrain::GetTriangleVertices(const int aTriangleIndex) const
{
	if (aTriangleIndex == -1)
	{
		return CU::StaticArray<CU::Vector3f, 3>();
	}

	const Triangle& triangle = myTriangleList[aTriangleIndex];
	CU::StaticArray<CU::Vector3f, 3> vertexArray;
	vertexArray[0] = myMeshVertices[triangle.myVertexIndex[0]];
	vertexArray[1] = myMeshVertices[triangle.myVertexIndex[1]];
	vertexArray[2] = myMeshVertices[triangle.myVertexIndex[2]];
	return vertexArray;
}

CU::StaticArray<int, 2> Terrain::GetEdgeVertexIndices(const int aEdgeIndex) const
{
	if (aEdgeIndex == -1)
	{
		return CU::StaticArray<int, 2>();
	}
	return CU::StaticArray<int, 2>(myEdgeList[aEdgeIndex].myVertexIndex);
}
CU::StaticArray<int, 3> Terrain::GetTriangleVertexIndices(const int aTriangleIndex) const
{
	if (aTriangleIndex == -1)
	{
		return CU::StaticArray<int, 3>();
	}
	return CU::StaticArray<int, 3>(myTriangleList[aTriangleIndex].myVertexIndex);
}

const CU::GrowingArray<CU::Vector4f>& Terrain::GetVertices() const
{
	return myMeshVertices;
}
const CU::GrowingArray<unsigned int>& Terrain::GetIndices() const
{
	return myMeshIndices;
}

bool Terrain::RayHitTriangleIndex(const CU::Vector3f aRayStart, const CU::Vector3f aRayVector, CU::Vector3f& outHitPoint, const int aTriangleIndex) const
{
	if (HasReturnedTriangleIndex(aTriangleIndex))
	{
		return false;
	}

	const Triangle& triangle = myTriangleList[aTriangleIndex];
	const CU::Vector3f vertices[3] = { myMeshVertices[triangle.myVertexIndex[0]], myMeshVertices[triangle.myVertexIndex[1]], myMeshVertices[triangle.myVertexIndex[2]] };

	//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	const CU::Vector3f edge1(vertices[1] - vertices[0]);
	const CU::Vector3f edge2(vertices[2] - vertices[0]);
	const CU::Vector3f h(aRayVector.Cross(edge2));
	const float a = edge1.Dot(h);
	if (a > -Epsilon && a < Epsilon)
	{
		return false;//This ray is parallel to this triangle
	}

	const CU::Vector3f s(aRayStart - vertices[0]);
	const float f = 1.0f / a;
	const float u = f * s.Dot(h);
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	const CU::Vector3f q(s.Cross(edge1));
	const float v = f * aRayVector.Dot(q);
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	//At this stage we can compute t to find out where the intersection point is on the line
	const float t = f * edge2.Dot(q);
	if (t < Epsilon || t > 1 - Epsilon)
	{
		return false;
	}

	outHitPoint = aRayStart + aRayVector * t;
	return true;
}

int Terrain::GetNewVertexIndex()
{
	if (myOpenVertexIndices.Size() > 0)
	{
		const int newIndex = myOpenVertexIndices.GetLast();
		myOpenVertexIndices.RemoveLast();
		return newIndex;
	}
	else
	{
		myVertexList.Add().Reset();
		myMeshVertices.Add();
		return myMeshVertices.Size() - 1;
	}
}
int Terrain::GetNewEdgeIndex()
{
	if (myOpenEdgeIndices.Size() > 0)
	{
		const int newIndex = myOpenEdgeIndices.GetLast();
		myOpenEdgeIndices.RemoveLast();
		return newIndex;
	}
	else
	{
		myEdgeList.Add();
		return myEdgeList.Size() - 1;
	}
}
int Terrain::GetNewTriangleIndex()
{
	if (myOpenTriangleIndices.Size() > 0)
	{
		const int newIndex = myOpenTriangleIndices.GetLast();
		myOpenTriangleIndices.RemoveLast();
		return newIndex;
	}
	else
	{
		myTriangleList.Add();
		return myTriangleList.Size() - 1;
	}
}

void Terrain::ReturnVertexIndex(const int aIndex)
{
	if (aIndex < 0)
	{
		return;
	}

	const int lastIndex = myMeshVertices.Size() - 1;
	if (aIndex > lastIndex)
	{
		return;
	}
	
	if (aIndex == lastIndex)
	{
		myVertexList.RemoveLast();
		myMeshVertices.RemoveLast();

		int foundNextLastIndex = myOpenVertexIndices.Find(myMeshVertices.Size() - 1);
		while (foundNextLastIndex != -1)
		{
			myVertexList.RemoveLast();
			myMeshVertices.RemoveLast();

			myOpenVertexIndices.RemoveCyclicAtIndex(foundNextLastIndex);
			foundNextLastIndex = myOpenVertexIndices.Find(myMeshVertices.Size() - 1);
		}
	}
	else
	{
		myOpenVertexIndices.Add(aIndex);
	}
}
void Terrain::ReturnEdgeIndex(const int aIndex)
{
	if (aIndex < 0)
	{
		return;
	}

	const int lastIndex = myEdgeList.Size() - 1;
	if (aIndex > lastIndex)
	{
		return;
	}

	if (aIndex == lastIndex)
	{
		myEdgeList.RemoveLast();

		int foundNextLastIndex = myOpenEdgeIndices.Find(myEdgeList.Size() - 1);
		while (foundNextLastIndex != -1)
		{
			myEdgeList.RemoveLast();

			myOpenEdgeIndices.RemoveCyclicAtIndex(foundNextLastIndex);
			foundNextLastIndex = myOpenEdgeIndices.Find(myEdgeList.Size() - 1);
		}
	}
	else
	{
		myOpenEdgeIndices.Add(aIndex);
	}
}
void Terrain::ReturnTriangleIndex(const int aIndex)
{
	if (aIndex < 0)
	{
		return;
	}

	const int lastIndex = myTriangleList.Size() - 1;
	if (aIndex > lastIndex)
	{
		return;
	}

	if (aIndex == lastIndex)
	{
		myTriangleList.RemoveLast();

		int foundNextLastIndex = myOpenTriangleIndices.Find(myTriangleList.Size() - 1);
		while (foundNextLastIndex != -1)
		{
			myTriangleList.RemoveLast();

			myOpenTriangleIndices.RemoveCyclicAtIndex(foundNextLastIndex);
			foundNextLastIndex = myOpenTriangleIndices.Find(myTriangleList.Size() - 1);
		}
	}
	else
	{
		myOpenTriangleIndices.Add(aIndex);
	}
}

bool Terrain::HasReturnedVertexIndex(const int aIndex) const
{
	if (aIndex < 0)
	{
		return true;
	}
	if (aIndex >= myVertexList.Size())
	{
		return true;
	}

	return myOpenVertexIndices.Find(aIndex) != -1;
}
bool Terrain::HasReturnedEdgeIndex(const int aIndex) const
{
	if (aIndex < 0)
	{
		return true;
	}
	if (aIndex >= myEdgeList.Size())
	{
		return true;
	}

	return myOpenEdgeIndices.Find(aIndex) != -1;
}
bool Terrain::HasReturnedTriangleIndex(const int aIndex) const
{
	if (aIndex < 0)
	{
		return true;
	}
	if (aIndex >= myTriangleList.Size())
	{
		return true;
	}

	return myOpenTriangleIndices.Find(aIndex) != -1;
}

void Terrain::Reset(Vertex& aVertex) const
{
	aVertex.Reset();
}
void Terrain::Reset(Edge& aEdge) const
{
	aEdge.myTriangleIndex[0] = -1;
	aEdge.myTriangleIndex[1] = -1;
	aEdge.myVertexIndex[0] = -1;
	aEdge.myVertexIndex[1] = -1;
}
void Terrain::Reset(Triangle& aTriangle) const
{
	aTriangle.myEdgeIndex[0] = -1;
	aTriangle.myEdgeIndex[1] = -1;
	aTriangle.myEdgeIndex[2] = -1;
	aTriangle.myVertexIndex[0] = -1;
	aTriangle.myVertexIndex[1] = -1;
	aTriangle.myVertexIndex[2] = -1;
	aTriangle.myIndicesIndex = -1;
}

Terrain::Vertex::Vertex()
{
	Reset();
}
void Terrain::Vertex::Reset()
{
	myEdgeIndexList = CU::GrowingArray<unsigned int>();
	myEdgeIndex[0] = -1;
	myEdgeIndex[1] = -1;
	myEdgeIndex[2] = -1;
	myEdgeIndex[3] = -1;
	myEdgeIndex[4] = -1;
	myEdgeIndex[5] = -1;
	myEdgeIndex[6] = -1;
	myEdgeIndex[7] = -1;
	myEdgeCount = 0;
}
void Terrain::Vertex::AddEdge(const int aEdgeIndex)
{
	if (myEdgeCount >= 8)
	{
		if (!myEdgeIndexList.IsInitialized())
		{
			myEdgeIndexList.Init(4);
		}
		myEdgeIndexList.Add(aEdgeIndex);
	}
	else
	{
		myEdgeIndex[myEdgeCount] = aEdgeIndex;
		myEdgeCount++;
	}
}
void Terrain::Vertex::RemoveEdge(const int aEdgeIndex)
{
	bool found = false;
	for (int vertexEdgeIndex = 0; vertexEdgeIndex < myEdgeCount; vertexEdgeIndex++)
	{
		if (myEdgeIndex[vertexEdgeIndex] == aEdgeIndex)
		{
			myEdgeIndex[vertexEdgeIndex] = RemoveLastEdge();
			found = true;
			break;
		}
	}
	if (!found && myEdgeIndexList.Size() > 0)
	{
		myEdgeIndexList.RemoveCyclic(aEdgeIndex);
	}
}

int Terrain::Vertex::GetEdgeCount() const
{
	return myEdgeCount + myEdgeIndexList.Size();
}
int Terrain::Vertex::GetEdgeIndex(const int aVertexEdgeIndex) const
{
	if (aVertexEdgeIndex < 0)
	{
		return -1;
	}

	if (aVertexEdgeIndex < myEdgeCount)
	{
		return myEdgeIndex[aVertexEdgeIndex];
	}
	else if (aVertexEdgeIndex < 8)
	{
		return -1;
	}

	const int listIndex = aVertexEdgeIndex - 8;
	if (listIndex < myEdgeIndexList.Size())
	{
		return myEdgeIndexList[listIndex];
	}
	return -1;
}

int Terrain::Vertex::RemoveLastEdge()
{
	if (myEdgeCount == 0)
	{
		return -1;
	}

	if (myEdgeCount >= 8 && myEdgeIndexList.Size() > 0)
	{
		const int lastEdge = myEdgeIndexList.GetLast();
		myEdgeIndexList.RemoveLast();
		return lastEdge;
	}
	else
	{
		myEdgeCount--;
		return myEdgeIndex[myEdgeCount];
	}
}