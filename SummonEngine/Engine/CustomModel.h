#pragma once
#include "Model.h"
#include <atomic>

struct CustomMesh
{
	CustomMesh() : myVertices(1), myIndices(1)
	{
	}
	CustomMesh(const CU::GrowingArray<CU::Vector4f>& aVertices, const CU::GrowingArray<unsigned int>& aIndices) : myVertices(aVertices), myIndices(aIndices)
	{
	}
	CU::GrowingArray<CU::Vector4f> myVertices;
	CU::GrowingArray<unsigned int> myIndices;
};

class CustomModel : public Model
{
public:
	CustomMesh& GetCustomMesh();
	const CustomMesh& GetCustomMesh() const;

	void MarkForUpdate();

protected:
	friend class Engine;
	friend class Scene;
	friend class ModelLoader;
	friend class ModelInstance;
	friend struct ModelRenderCommand;
	friend class CU::HashMap<std::string, CustomModel>;

	CustomModel();
	~CustomModel() = default;

protected:
	CustomMesh myCustomMesh;
	bool myShouldUpdateMesh;
};