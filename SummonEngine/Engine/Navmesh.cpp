#include "stdafx.h"
#include "Navmesh.h"
#include <fstream>
#include <iostream>

namespace Nav
{
	bool operator==(const Edge& aEdge1, const Edge& aEdge2)
	{
		return ((aEdge1.VertexIndex[0] == aEdge2.VertexIndex[0] && aEdge1.VertexIndex[1] == aEdge2.VertexIndex[1]) ||
			(aEdge1.VertexIndex[0] == aEdge2.VertexIndex[1] && aEdge1.VertexIndex[1] == aEdge2.VertexIndex[0]));
	}

	Navmesh::Navmesh()
	{
	}
	Navmesh::~Navmesh()
	{
	}

	void Navmesh::Load(const std::string& aPath)
	{
		myVertexList.Init(32);
		myEdgeList.Init(48);
		myTriangleList.Init(32);
		myIndexList.Init(64);

		std::fstream pathListStream(aPath);
		assert(pathListStream && "Failed to open navmesh");
		if (!pathListStream)
		{
			return;
		}

		std::vector<std::string> vertexLines;
		std::vector<std::string> triangleLines;
		char path[256];
		while (!pathListStream.eof())
		{
			pathListStream.getline(path, 256);
			if (path[1] == ' ')
			{
				if (path[0] == 'v')
				{
					vertexLines.push_back(path);
					vertexLines.back() = vertexLines.back().substr(1);
				}
				else if (path[0] == 'f')
				{
					triangleLines.push_back(path);
					triangleLines.back() = triangleLines.back().substr(1);
				}
			}
		}

		std::string valString[3];
		size_t divIndex[4];
		//create vertex list
		for (int line = 0; line < vertexLines.size(); line++)
		{
			divIndex[0] = 0;
			divIndex[3] = vertexLines[line].size();
			for (int i = 1; i < 3; i++)
			{
				divIndex[i] = vertexLines[line].find_first_of(' ', divIndex[i - 1] + 1);
			}
			for (int i = 0; i < 3; i++)
			{
				valString[i] = vertexLines[line].substr(divIndex[i] + 1, divIndex[i + 1] - divIndex[i] - 1);
			}
			myVertexList.Add(CU::Vector4f(std::stof(valString[0]), std::stof(valString[1]), std::stof(valString[2]), 1.0f));
		}

		//create triangle and edge list
		for (int line = 0; line < triangleLines.size(); line++)
		{
			Triangle triangle;
			divIndex[0] = 0;
			divIndex[3] = triangleLines[line].size();
			for (int i = 1; i < 3; i++)
			{
				divIndex[i] = triangleLines[line].find_first_of(' ', divIndex[i - 1] + 1);
			}
			for (int i = 0; i < 3; i++)
			{
				valString[i] = triangleLines[line].substr(divIndex[i] + 1, divIndex[i + 1] - divIndex[i] - 1);
				myIndexList.Add(std::stoi(valString[i]) - 1);
				triangle.VertexIndex[i] = static_cast<int>(myIndexList.GetLast());
			}
				
			Edge edges[3];
			edges[0] = Edge(triangle.VertexIndex[0], triangle.VertexIndex[1]);
			edges[1] = Edge(triangle.VertexIndex[1], triangle.VertexIndex[2]);
			edges[2] = Edge(triangle.VertexIndex[2], triangle.VertexIndex[0]);
			for (int i = 0; i < 3; i++)
			{
				triangle.EdgeIndex[i] = myEdgeList.Find(edges[i]);
				if (triangle.EdgeIndex[i] == -1)
				{
					triangle.EdgeIndex[i] = myEdgeList.Size();
					myEdgeList.Add(edges[i]);
					myEdgeList.GetLast().TriangleIndex[0] = myTriangleList.Size();
				}
				else
				{
					myEdgeList[triangle.EdgeIndex[i]].TriangleIndex[1] = myTriangleList.Size();
				}
			}
			CU::GrowingArray<CU::Vector3f> vertexList(GetVertexList(triangle));
			CU::Vector3f vertexActual = (vertexList[0] + vertexList[1] + vertexList[2]) / 3.0f;
			triangle.Point = vertexActual;
			myTriangleList.Add(triangle);
		}
	}
}