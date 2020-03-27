#include "stdafx.h"
#include "ModelLoader.h"
#include "ShaderLoader.h"
#include "Skeleton.h"
#include "Animation.h"
#include "FBXSkeleton.h"
#include "FBXAnimation.h"

#include <Camera.hpp>
#include <DebugTools.h>

#include <d3d11.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>

#pragma comment(lib, "d3d11.lib")

std::mutex ModelAddMutex;
std::mutex ModelLoadMutex;

ModelLoader* ModelLoader::ourInstance = nullptr;
void ModelLoader::CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxModelAmount)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new ModelLoader(aDevice, aMaxModelAmount);
	}
}
ModelLoader* ModelLoader::GetInstance()
{
	return ourInstance;
}
void ModelLoader::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

ModelLoader::ModelLoader(ID3D11Device* aDevice, const unsigned int aMaxModelAmount)
	: myModelMap(aMaxModelAmount), mySkinnedModelMap(aMaxModelAmount), myCustomModelMap(aMaxModelAmount), mySkeletonMap(aMaxModelAmount),
	myLoadingThread(std::bind(&ModelLoader::LoadThreaded, this))
{
	myStopThread = false;
	myDevice = aDevice;
	myLoadCommands.Init(64);
}
ModelLoader::~ModelLoader()
{
	myStopThread = true;
	myLoadingThread.join();
}

void ModelLoader::LoadThreaded()
{
	LoadCommand modelToLoad;
	while (!myStopThread)
	{
		if (myLoadCommands.Size() > 0)
		{
			myIsLoading = true;
			ModelAddMutex.lock();
			modelToLoad = myLoadCommands.GetLast();
			if (modelToLoad.myIsAnimation && *modelToLoad.myModel == nullptr)
			{
				bool foundValid = false;
				for (int commandIndex = myLoadCommands.Size() - 1; commandIndex >= 0; commandIndex--)
				{
					if (!myLoadCommands[commandIndex].myIsAnimation || *myLoadCommands[commandIndex].myModel != nullptr)
					{
						modelToLoad = myLoadCommands[commandIndex];
						myLoadCommands.RemoveAtIndex(commandIndex);
						foundValid = true;
						break;
					}
				}
				if (!foundValid)
				{
					myLoadCommands.RemoveAll();
					continue;
				}
			}
			else
			{
				myLoadCommands.RemoveLast();
			}
			ModelAddMutex.unlock();
			if (modelToLoad.myIsAnimation)
			{
				LoadAnimations(modelToLoad.myPath, static_cast<SkinnedModel*>(*modelToLoad.myModel));
			}
			else
			{
				LoadModel(modelToLoad.myPath, modelToLoad.mySkeletonPath, modelToLoad.myModel, modelToLoad.myIsCustom);
			}
		}
		else if (myIsLoading)
		{
			myIsLoading = false;
		}
		std::this_thread::yield();
	}
}

Model* ModelLoader::LoadCube(const CubeType aType)
{
	HRESULT result;

	#pragma region Shape
	struct VertexDebug
	{
		float x, y, z, w;
		float r, g, b, a;
		float u, v;
	} verticesDebug[8] =
	{
		{ 0.5f, 0.5f, 0.5f,1.0f, 1,1,1,1, 1,0},
		{ 0.5f, 0.5f,-0.5f,1.0f, 1,1,0,1, 1,0},
		{ 0.5f,-0.5f, 0.5f,1.0f, 1,0,1,1, 1,1},
		{-0.5f, 0.5f, 0.5f,1.0f, 0,1,1,1, 0,0},
		{ 0.5f,-0.5f,-0.5f,1.0f, 1,0,0,1, 1,1},
		{-0.5f, 0.5f,-0.5f,1.0f, 0,1,0,1, 0,0},
		{-0.5f,-0.5f, 0.5f,1.0f, 0,0,1,1, 0,1},
		{-0.5f,-0.5f,-0.5f,1.0f, 0,0,0,1, 0,1}
	};
	struct VertexSkybox
	{
		float x, y, z, w;
	} verticesSkybox[8] =
	{
		{ 100.0f, 100.0f, 100.0f, 1.0f},
		{ 100.0f, 100.0f,-100.0f, 1.0f},
		{ 100.0f,-100.0f, 100.0f, 1.0f},
		{-100.0f, 100.0f, 100.0f, 1.0f},
		{ 100.0f,-100.0f,-100.0f, 1.0f},
		{-100.0f, 100.0f,-100.0f, 1.0f},
		{-100.0f,-100.0f, 100.0f, 1.0f},
		{-100.0f,-100.0f,-100.0f, 1.0f}
	};
	struct Vertex
	{
		CU::Vector4f pos;
		CU::Vector4f normal;
		CU::Vector4f tangent;
		CU::Vector4f binormal;
		CU::Vector2f uv;
	} vertices[8];
	vertices[0].pos = CU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	vertices[0].normal = CU::Vector4f(CU::Vector3f(1.0f, 1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[0].tangent = CU::Vector4f(CU::Vector3f(-1.0f, 1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[0].binormal = CU::Vector4f(CU::Vector3f(-1.0f, 0.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[0].uv = CU::Vector2f(1.0f, 0.0f);
	vertices[1].pos = CU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);
	vertices[1].normal = CU::Vector4f(CU::Vector3f(1.0f, 1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[1].tangent = CU::Vector4f(CU::Vector3f(-1.0f, 1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[1].binormal = CU::Vector4f(CU::Vector3f(1.0f, 0.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[1].uv = CU::Vector2f(1.0f, 0.0f);
	vertices[2].pos = CU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);
	vertices[2].normal = CU::Vector4f(CU::Vector3f(1.0f, -1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[2].tangent = CU::Vector4f(CU::Vector3f(-1.0f, -1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[2].binormal = CU::Vector4f(CU::Vector3f(-1.0f, 0.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[2].uv = CU::Vector2f(1.0f, 1.0f);
	vertices[3].pos = CU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);
	vertices[3].normal = CU::Vector4f(CU::Vector3f(-1.0f, 1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[3].tangent = CU::Vector4f(CU::Vector3f(1.0f, 1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[3].binormal = CU::Vector4f(CU::Vector3f(-1.0f, 0.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[3].uv = CU::Vector2f(0.0f, 0.0f);
	vertices[4].pos = CU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);
	vertices[4].normal = CU::Vector4f(CU::Vector3f(1.0f, -1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[4].tangent = CU::Vector4f(CU::Vector3f(-1.0f, -1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[4].binormal = CU::Vector4f(CU::Vector3f(1.0f, 0.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[4].uv = CU::Vector2f(1.0f, 1.0f);
	vertices[5].pos = CU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);
	vertices[5].normal = CU::Vector4f(CU::Vector3f(-1.0f, 1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[5].tangent = CU::Vector4f(CU::Vector3f(1.0f, 1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[5].binormal = CU::Vector4f(CU::Vector3f(1.0f, 0.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[5].uv = CU::Vector2f(0.0f, 0.0f);
	vertices[6].pos = CU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);
	vertices[6].normal = CU::Vector4f(CU::Vector3f(-1.0f, -1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[6].tangent = CU::Vector4f(CU::Vector3f(1.0f, -1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[6].binormal = CU::Vector4f(CU::Vector3f(-1.0f, 0.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[6].uv = CU::Vector2f(0.0f, 1.0f);
	vertices[7].pos = CU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);
	vertices[7].normal = CU::Vector4f(CU::Vector3f(-1.0f, -1.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[7].tangent = CU::Vector4f(CU::Vector3f(1.0f, -1.0f, 1.0f).GetNormalized(), 0.0f);
	vertices[7].binormal = CU::Vector4f(CU::Vector3f(1.0f, 0.0f, -1.0f).GetNormalized(), 0.0f);
	vertices[7].uv = CU::Vector2f(0.0f, 1.0f);

	unsigned int indices[36] =
	{
		0,2,1,
		0,1,3,
		0,3,2,
		1,2,4,
		2,3,6,
		3,1,5,
		4,5,1,
		5,6,3,
		6,4,2,
		7,6,5,
		7,5,4,
		7,4,6
	};
	unsigned int indicesSkybox[36] =
	{
		6,4,7,
		4,5,7,
		5,6,7,
		2,4,6,
		3,6,5,
		1,5,4,
		5,1,3,
		6,3,2,
		4,2,1,
		2,3,0,
		3,1,0,
		1,2,0
	};
	#pragma endregion

	#pragma region Buffers
	D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
	vertexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	vertexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
	switch (aType)
	{
	case CubeType::Normal:
		vertexBufferDescription.ByteWidth = sizeof(vertices);
		vertexSubresourceData.pSysMem = vertices;
		break;
	case CubeType::Debug:
		vertexBufferDescription.ByteWidth = sizeof(verticesDebug);
		vertexSubresourceData.pSysMem = verticesDebug;
		break;
	case CubeType::Skybox:
		vertexBufferDescription.ByteWidth = sizeof(verticesSkybox);
		vertexSubresourceData.pSysMem = verticesSkybox;
		break;
	default:
		CU::Log("Tried to load cube with invalid type", CU::LogType::Error);
		return nullptr;
	}

	ID3D11Buffer* vertexBuffer;
	result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
	assert(!FAILED(result) && "Failed to create cube vertex buffer");
	if (FAILED(result))
	{
		CU::Log("Failed to create cube vertex buffer", CU::LogType::Error);
		return nullptr;
	}

	D3D11_BUFFER_DESC indexBufferDescription = { 0 };
	indexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	indexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
	if (aType == CubeType::Skybox)
	{
		indexBufferDescription.ByteWidth = sizeof(indicesSkybox);
		indexSubresourceData.pSysMem = indicesSkybox;
	}
	else
	{
		indexBufferDescription.ByteWidth = sizeof(indices);
		indexSubresourceData.pSysMem = indices;
	}

	ID3D11Buffer* indexBuffer;
	result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
	assert(!FAILED(result) && "Failed to create cube index buffer");
	if (FAILED(result))
	{
		CU::Log("Failed to create cube index buffer", CU::LogType::Error);
		vertexBuffer->Release();
		return nullptr;
	}
	#pragma endregion

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;
	switch (aType)
	{
	case CubeType::Debug:
		vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\DebugCubeVertex.cso");
		if (vertexShader == nullptr)
		{
			std::array<D3D11_INPUT_ELEMENT_DESC, 3> layout;
			layout[0] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
			layout[1] = { "COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
			layout[2] = { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
			vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\DebugCubeVertex.cso", &layout[0], static_cast<unsigned int>(layout.size()));
		}
		pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\DebugCubePixel.cso");
		break;
	case CubeType::Skybox:
		vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\SkyboxVertex.cso");
		if (vertexShader == nullptr)
		{
			D3D11_INPUT_ELEMENT_DESC layout =
				{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
			vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\SkyboxVertex.cso", &layout, 1);
		}
		pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\SkyboxPixel.cso");
		break;
	case CubeType::Normal:
	default:
		break;
	}

	std::string path;
	switch (aType)
	{
	case CubeType::Debug:
		path = "DebugCube";
		break;
	case CubeType::Skybox:
		path = "Skybox";
		break;
	case CubeType::Normal:
	default:
		path = "Cube";
		break;
	}

	if (!myModelMap.Create(path))
	{
		CU::Log("Failed to create cube model map entry", CU::LogType::Error);
		vertexBuffer->Release();
		indexBuffer->Release();
		return nullptr;
	}
	Model* model = myModelMap.Get(path);
	if (model == nullptr)
	{
		CU::Log("Failed to find cube model map entry", CU::LogType::Error);
		vertexBuffer->Release();
		indexBuffer->Release();
		return nullptr;
	}
	model->myVertexShader = vertexShader;
	model->myGeometryShader = nullptr;
	model->myPixelShader = pixelShader;
	MeshData modelData;
	switch (aType)
	{
	case CubeType::Debug:
		modelData.myVertexAmount = sizeof(verticesDebug) / sizeof(VertexDebug);
		modelData.myIndexAmount = sizeof(indices) / sizeof(unsigned int);
		modelData.myStride = sizeof(VertexDebug);
		break;
	case CubeType::Skybox:
		modelData.myVertexAmount = sizeof(verticesSkybox) / sizeof(VertexSkybox);
		modelData.myIndexAmount = sizeof(indicesSkybox) / sizeof(unsigned int);
		modelData.myStride = sizeof(VertexSkybox);
		break;
	case CubeType::Normal:
	default:
		modelData.myVertexAmount = sizeof(vertices) / sizeof(Vertex);
		modelData.myIndexAmount = sizeof(indices) / sizeof(unsigned int);
		modelData.myStride = sizeof(Vertex);
		break;
	}
	modelData.myOffset = 0;
	modelData.myVertexBuffer = vertexBuffer;
	modelData.myIndexBuffer = indexBuffer;
	model->myData[0] = modelData;
	model->myTexture[0] = TextureInstance("Models\\Default\\albedo.dds");
	model->myTexture[1] = TextureInstance("Models\\Default\\normal.dds");
	model->myTexture[2] = TextureInstance("Models\\Default\\metalness.dds");
	model->myTexture[3] = TextureInstance("Models\\Default\\roughness.dds");
	model->myTexture[4] = TextureInstance("Models\\Default\\ambientOcclusion.dds");
	model->myTexture[5] = TextureInstance("Models\\Default\\emissive.dds");
	model->myPath = path;
	model->myEncapsulationRange = CU::Vector3f(1.0f, 1.0f, 1.0f).Length();
	model->myMeshCount = 1;
	model->myUserCount = 0;

	return model;
}

Model* ModelLoader::LoadPlane()
{
	HRESULT result;

	struct Vertex
	{
		CU::Vector4f pos;
		CU::Vector4f normal;
		CU::Vector4f tangent;
		CU::Vector4f binormal;
		CU::Vector2f uv;
	} vertices[4];
	vertices[0].pos = CU::Vector4f(1.0f, 0.0f, -1.0f, 1.0f);
	vertices[0].normal = CU::Vector4f(0.0f, 1.0f, 0.0f, 0.0f);
	vertices[0].tangent = CU::Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
	vertices[0].binormal = CU::Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
	vertices[0].uv = CU::Vector2f(0.0f, 0.0f);
	vertices[1].pos = CU::Vector4f(-1.0f, 0.0f, -1.0f, 1.0f);
	vertices[1].normal = CU::Vector4f(0.0f, 1.0f, 0.0f, 0.0f);
	vertices[1].tangent = CU::Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
	vertices[1].binormal = CU::Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
	vertices[1].uv = CU::Vector2f(1.0f, 0.0f);
	vertices[2].pos = CU::Vector4f(-1.0f, 0.0f, 1.0f, 1.0f);
	vertices[2].normal = CU::Vector4f(0.0f, 1.0f, 0.0f, 0.0f);
	vertices[2].tangent = CU::Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
	vertices[2].binormal = CU::Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
	vertices[2].uv = CU::Vector2f(1.0f, 1.0f);
	vertices[3].pos = CU::Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
	vertices[3].normal = CU::Vector4f(0.0f, 1.0f, 0.0f, 0.0f);
	vertices[3].tangent = CU::Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
	vertices[3].binormal = CU::Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
	vertices[3].uv = CU::Vector2f(0.0f, 1.0f);

	unsigned int indices[6] =
	{
		0,1,2,
		0,2,3
	};

	#pragma region Buffer
	D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
	vertexBufferDescription.ByteWidth = sizeof(vertices);
	vertexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	vertexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
	vertexSubresourceData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer;
	result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
	assert(!FAILED(result) && "Failed to create plane vertex buffer");
	if (FAILED(result))
	{
		CU::Log("Failed to create plane vertex buffer", CU::LogType::Error);
		return nullptr;
	}

	D3D11_BUFFER_DESC indexBufferDescription = { 0 };
	indexBufferDescription.ByteWidth = sizeof(indices);
	indexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	indexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
	indexSubresourceData.pSysMem = indices;

	ID3D11Buffer* indexBuffer;
	result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
	assert(!FAILED(result) && "Failed to create plane index buffer");
	if (FAILED(result))
	{
		CU::Log("Failed to create plane index buffer", CU::LogType::Error);
		vertexBuffer->Release();
		return nullptr;
	}
	#pragma endregion

	const std::string path("Plane");
	if (!myModelMap.Create(path))
	{
		CU::Log("Failed to create plane model map entry", CU::LogType::Error);
		vertexBuffer->Release();
		indexBuffer->Release();
		return nullptr;
	}
	Model* model = myModelMap.Get(path);
	if (model == nullptr)
	{
		CU::Log("Failed to find plane model map entry", CU::LogType::Error);
		vertexBuffer->Release();
		indexBuffer->Release();
		return nullptr;
	}

	MeshData modelData;
	modelData.myVertexAmount = sizeof(vertices) / sizeof(Vertex);
	modelData.myIndexAmount = sizeof(indices) / sizeof(unsigned int);
	modelData.myStride = sizeof(Vertex);
	modelData.myOffset = 0;
	modelData.myVertexBuffer = vertexBuffer;
	modelData.myIndexBuffer = indexBuffer;
	model->myData[0] = modelData;
	model->myTexture[0] = TextureInstance("Models\\Default\\albedo.dds");
	model->myTexture[1] = TextureInstance("Models\\Default\\normal.dds");
	model->myTexture[2] = TextureInstance("Models\\Default\\metalness.dds");
	model->myTexture[3] = TextureInstance("Models\\Default\\roughness.dds");
	model->myTexture[4] = TextureInstance("Models\\Default\\ambientOcclusion.dds");
	model->myTexture[5] = TextureInstance("Models\\Default\\emissive.dds");
	model->myEncapsulationRange = CU::Vector3f(1.0f, 1.0f, 1.0f).Length();
	model->myPath = path;
	model->myUserCount = 0;

	return model;
}

Model* ModelLoader::GetModel(const std::string& aPath, Model** aModel)
{
	if (aPath.length() < 1)
	{
		return nullptr;
	}
	Model* model = myModelMap.Get(aPath);
	if (model == nullptr)
	{
		ModelAddMutex.lock();
		bool found = false;
		for (int i = 0; i < myLoadCommands.Size(); i++)
		{
			if (!myLoadCommands[i].myIsAnimation && myLoadCommands[i].myModel == aModel)
			{
				//myLoadCommands.RemoveCyclicAtIndex(i);
				myLoadCommands[i].myPath = aPath;
				found = true;
				break;
			}
		}
		if (!found)
		{
			myLoadCommands.Add(LoadCommand(aPath, aModel));
		}
		ModelAddMutex.unlock();
	}
	else
	{
		model->AddUser();
	}
	return model;
}
SkinnedModel* ModelLoader::GetSkinnedModel(const std::string& aPath, const std::string& aSkeletonPath, SkinnedModel** aModel)
{
	if (aPath.length() < 1)
	{
		return nullptr;
	}
	SkinnedModel* model = mySkinnedModelMap.Get(aPath);
	if (model == nullptr)
	{
		ModelAddMutex.lock();
		bool found = false;
		for (unsigned short i = 0; i < myLoadCommands.Size(); i++)
		{
			if (!myLoadCommands[i].myIsAnimation && myLoadCommands[i].myModel == reinterpret_cast<Model**>(aModel))
			{
				//myLoadCommands.RemoveCyclicAtIndex(i);
				myLoadCommands[i].myPath = aPath;
				found = true;
				break;
			}
		}
		if (!found)
		{
			myLoadCommands.Add(LoadCommand(aPath, aSkeletonPath, reinterpret_cast<Model**>(aModel)));
		}
		ModelAddMutex.unlock();
	}
	else
	{
		model->AddUser();
	}
	return model;
}
CustomModel* ModelLoader::GetCustomModel(const std::string& aName, CustomModel** aModel)
{
	if (aName.length() < 1)
	{
		return nullptr;
	}
	CustomModel* model = myCustomModelMap.Get(aName);
	if (model == nullptr)
	{
		ModelAddMutex.lock();
		bool found = false;
		for (int i = 0; i < myLoadCommands.Size(); i++)
		{
			if (!myLoadCommands[i].myIsAnimation && myLoadCommands[i].myModel == reinterpret_cast<Model**>(aModel))
			{
				myLoadCommands[i].myPath = aName;
				found = true;
				break;
			}
		}
		if (!found)
		{
			//myLoadCommands.Add(LoadCommand(aName, reinterpret_cast<Model**>(aModel), false, true));
			LoadModel(aName, "", reinterpret_cast<Model**>(aModel), true);
			model = *aModel;
		}
		ModelAddMutex.unlock();
	}
	else
	{
		model->AddUser();
	}
	return model;
}

void ModelLoader::AddAnimations(const std::string& aPath, SkinnedModel** aModel)
{
	ModelAddMutex.lock();
	myLoadCommands.Add(LoadCommand(aPath, reinterpret_cast<Model**>(aModel), true));
	ModelAddMutex.unlock();
}

const CU::SizableArray<Animation, MaxAnimationCount>& ModelLoader::GetAnimations() const
{
	/*CU::GrowingArray<Animation*> list(8);
	for (CU::HashMap<std::string, Animation*>::Iterator i = myAnimationMap.Begin(); !i.IsEnd(); i++)
	{
		list.Add(i.GetValue());
	}*/
	return myAnimationList;
}

void ModelLoader::UpdateCustomMeshes()
{
	for (CU::HashMap<std::string, CustomModel>::Iterator it = myCustomModelMap.Begin(); it != myCustomModelMap.End(); it++)
	{
		CustomModel& model = it.GetValue();
		if (model.myShouldUpdateMesh)
		{
			const CustomMesh& mesh = model.GetCustomMesh();

			if (model.myMeshCount > 0)
			{
				model.myData[0].myVertexBuffer->Release();
				model.myData[0].myIndexBuffer->Release();
			}
			if (mesh.myVertices.Size() < 3 || mesh.myIndices.Size() < 3)
			{
				model.myMeshCount = 0;
				continue;
			}

			#pragma region Buffer
			HRESULT result;
			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			vertexBufferDescription.ByteWidth = sizeof(CU::Vector4f) * mesh.myVertices.Size();
			vertexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
			vertexSubresourceData.pSysMem = mesh.myVertices.GetPointer();

			ID3D11Buffer* vertexBuffer;
			result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			assert(!FAILED(result) && "Failed to create custom model vertex buffer");
			if (FAILED(result))
			{
				CU::Log("Failed to create custom model vertex buffer", CU::LogType::Error);
				return;
			}

			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			indexBufferDescription.ByteWidth = sizeof(unsigned int) * mesh.myIndices.Size();
			indexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
			indexSubresourceData.pSysMem = mesh.myIndices.GetPointer();

			ID3D11Buffer* indexBuffer;
			result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			assert(!FAILED(result) && "Failed to create custom model index buffer");
			if (FAILED(result))
			{
				CU::Log("Failed to create custom model index buffer", CU::LogType::Error);
				vertexBuffer->Release();
				return;
			}
			#pragma endregion

			model.myShouldUpdateMesh = false;
			model.myEncapsulationRange = CalculateEncapsulationRange(&mesh);
			model.myData[0].myVertexBuffer = vertexBuffer;
			model.myData[0].myVertexAmount = mesh.myVertices.Size();
			model.myData[0].myIndexBuffer = indexBuffer;
			model.myData[0].myIndexAmount = mesh.myIndices.Size();

			if (model.myMeshCount == 0)
			{
				model.myMeshCount = 1;
				model.myData[0].myStride = sizeof(CU::Vector4f);
				model.myData[0].myOffset = 0;
			}
		}
	}
}

void ModelLoader::LoadModel(const std::string& aPath, const std::string& aSkeletonPath, Model** outModel, const bool aCustom)
{
	const unsigned int pathLength = static_cast<unsigned int>(aPath.length());
	assert(pathLength > 0 && "missing path");
	if (pathLength <= 0)
	{
		CU::Log("Tried to load model without a path", CU::LogType::Error);
		return;
	}
	Model* model = nullptr;
	const std::string skinnedPath(aPath + aSkeletonPath);

	if (aSkeletonPath.length() > 0)
	{
		model = mySkinnedModelMap.Get(skinnedPath);
		if (model != nullptr)
		{
			model->AddUser();
			*outModel = model;
			return;
		}
	}
	else if (aCustom)
	{
		model = myCustomModelMap.Get(aPath);
		if (model != nullptr)
		{
			model->AddUser();
			*outModel = model;
			return;
		}
	}
	else
	{
		model = myModelMap.Get(aPath);
		if (model != nullptr)
		{
			model->AddUser();
			*outModel = model;
			return;
		}
		else
		{
			if (pathLength == 4 && aPath == "Cube")
			{
				model = LoadCube();
				if (model != nullptr)
				{
					model->AddUser();
				}
				*outModel = model;
				return;
			}
			else if (pathLength == 9 && aPath == "DebugCube")
			{
				model = LoadCube(CubeType::Debug);
				if (model != nullptr)
				{
					model->AddUser();
				}
				*outModel = model;
				return;
			}
			else if (pathLength == 6 && aPath == "Skybox")
			{
				model = LoadCube(CubeType::Skybox);
				if (model != nullptr)
				{
					model->AddUser();
				}
				*outModel = model;
				return;
			}
			else if (pathLength == 5 && aPath == "Plane")
			{
				model = LoadPlane();
				if (model != nullptr)
				{
					model->AddUser();
				}
				*outModel = model;
				return;
			}
		}
	}

	ModelLoadMutex.lock();
	if (aSkeletonPath.length() > 0)
	{
		const bool mapCreateSuccess = mySkinnedModelMap.Create(skinnedPath);
		assert(mapCreateSuccess && "Failed to create skinned model map entry");
		if (!mapCreateSuccess)
		{
			ModelLoadMutex.unlock();
			CU::Log("Failed to create skinned model map entry", CU::LogType::Error);
			return;
		}
		model = mySkinnedModelMap.Get(skinnedPath);
	}
	else if (aCustom)
	{
		const bool mapCreateSuccess = myCustomModelMap.Create(aPath);
		assert(mapCreateSuccess && "Failed to create custom model map entry");
		if (!mapCreateSuccess)
		{
			ModelLoadMutex.unlock();
			CU::Log("Failed to create custom model map entry", CU::LogType::Error);
			return;
		}
		model = myCustomModelMap.Get(aPath);
	}
	else
	{
		const bool mapCreateSuccess = myModelMap.Create(aPath);
		assert(mapCreateSuccess && "Failed to create model map entry");
		if (!mapCreateSuccess)
		{
			ModelLoadMutex.unlock();
			CU::Log("Failed to create model map entry", CU::LogType::Error);
			return;
		}
		model = myModelMap.Get(aPath);
	}
	ModelLoadMutex.unlock();
	assert(model != nullptr && "Could not find created model");
	if (model == nullptr)
	{
		CU::Log("Could not find created model", CU::LogType::Error);
		return;
	}

	HRESULT result;
	const std::string folderPath(aPath.substr(0, aPath.find_last_of('\\') + 1));
	LoaderModel* fbxModel = nullptr;

	if (!aCustom)
	{
		#pragma region Animation
		if (aSkeletonPath.length() > 0)
		{
			SkinnedModel* skinnedModel = static_cast<SkinnedModel*>(model);
			Skeleton* skeleton = mySkeletonMap.Get(aSkeletonPath);
			if (skeleton == nullptr)
			{
				const bool mapCreateSuccess = mySkeletonMap.Create(aSkeletonPath);
				assert(mapCreateSuccess && "Failed to create skeleton map entry");
				if (!mapCreateSuccess)
				{
					CU::Log("Failed to create skeleton map entry", CU::LogType::Error);
					return;
				}
				skeleton = mySkeletonMap.Get(aSkeletonPath);
				skeleton->myPath = aSkeletonPath;

				CU::GrowingArray<FBXAnimation> loadedAnimations;
				myFBXLoader.LoadSkeleton(skeleton->mySkeleton, loadedAnimations, aSkeletonPath);
				LoadAnimations(loadedAnimations, skeleton);
			}
			skinnedModel->mySkeleton = skeleton;
		}
		#pragma endregion

		fbxModel = myFBXLoader.LoadModel(aPath);
		for (int meshIndex = 0; meshIndex < fbxModel->myMeshes.size() && meshIndex < 8; meshIndex++)
		{
			FBXMesh* mesh = fbxModel->myMeshes[meshIndex];

			#pragma region Buffer
			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			vertexBufferDescription.ByteWidth = mesh->myVertexBufferSize * mesh->myVertexCount;
			vertexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
			vertexSubresourceData.pSysMem = mesh->myVerticies;

			ID3D11Buffer* vertexBuffer;
			result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			assert(!FAILED(result) && "Failed to create model vertex buffer");
			if (FAILED(result))
			{
				continue;
			}

			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			indexBufferDescription.ByteWidth = static_cast<unsigned int>(mesh->myIndexes.size()) * sizeof(unsigned int);
			indexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
			indexSubresourceData.pSysMem = mesh->myIndexes.data();

			ID3D11Buffer* indexBuffer;
			result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			assert(!FAILED(result) && "Failed to create model index buffer");
			if (FAILED(result))
			{
				continue;
			}
			#pragma endregion

			MeshData& meshData = model->myData[meshIndex];
			meshData.myVertexAmount = mesh->myVertexCount;
			meshData.myIndexAmount = static_cast<unsigned int>(mesh->myIndexes.size());
			meshData.myStride = mesh->myVertexBufferSize;
			meshData.myOffset = 0;
			meshData.myVertexBuffer = vertexBuffer;
			meshData.myIndexBuffer = indexBuffer;
			model->myEncapsulationRange = CalculateEncapsulationRange(mesh);
		}
		model->myMeshCount = CU::Math::Clamp<unsigned int>(static_cast<unsigned int>(fbxModel->myMeshes.size()), 0, 8);
	}
	else
	{
		model->myMeshCount = 0;
	}

	#pragma region Texture
	TextureInstance albedoTexture;
	if (!aCustom && fbxModel->myTextures[0] != "")
	{
		std::string albedoPath = folderPath + fbxModel->myTextures[0];
		if (albedoPath[albedoPath.length() - 1] == 'a' &&
			albedoPath[albedoPath.length() - 2] == 'g' &&
			albedoPath[albedoPath.length() - 3] == 't')
		{
			albedoPath.replace(albedoPath.length() - 3, 3, "dds");
			//albedoTexture = TextureInstance("Models\\Default\\albedo.dds");
		}
		albedoTexture = TextureInstance(albedoPath);
	}
	else
	{
		albedoTexture = TextureInstance("Models\\Default\\albedo.dds");
	}
	assert(albedoTexture.GetTexture() != nullptr && "Unable to get albedo texture");
	if (albedoTexture.GetTexture() == nullptr)
	{
		return;
	}

	TextureInstance roughnessTexture;
	if (!aCustom && fbxModel->myTextures[1] != "")
	{
		const std::string roughnessPath(folderPath + fbxModel->myTextures[1]);
		roughnessTexture = TextureInstance(roughnessPath);
	}
	else
	{
		roughnessTexture = TextureInstance("Models\\Default\\roughness.dds");
	}
	assert(roughnessTexture.GetTexture() != nullptr && "Unable to get normal texture");
	if (roughnessTexture.GetTexture() == nullptr)
	{
		return;
	}

	TextureInstance ambientOcclusionTexture;
	if (!aCustom && fbxModel->myTextures[2] != "")
	{
		const std::string ambientOcclusionPath(folderPath + fbxModel->myTextures[2]);
		ambientOcclusionTexture = TextureInstance(ambientOcclusionPath);
	}
	else
	{
		ambientOcclusionTexture = TextureInstance("Models\\Default\\ambientOcclusion.dds");
	}
	assert(ambientOcclusionTexture.GetTexture() != nullptr && "Unable to get normal texture");
	if (ambientOcclusionTexture.GetTexture() == nullptr)
	{
		return;
	}

	TextureInstance emissiveTexture;
	if (!aCustom && fbxModel->myTextures[3] != "")
	{
		const std::string emissivePath(folderPath + fbxModel->myTextures[3]);
		emissiveTexture = TextureInstance(emissivePath);
	}
	else
	{
		emissiveTexture = TextureInstance("Models\\Default\\emissive.dds");
	}
	assert(emissiveTexture.GetTexture() != nullptr && "Unable to get normal texture");
	if (emissiveTexture.GetTexture() == nullptr)
	{
		return;
	}

	TextureInstance normalTexture;
	if (!aCustom && fbxModel->myTextures[5] != "")
	{
		const std::string normalPath(folderPath + fbxModel->myTextures[5]);
		normalTexture = TextureInstance(normalPath);
	}
	else
	{
		normalTexture = TextureInstance("Models\\Default\\normal.dds");
	}
	assert(normalTexture.GetTexture() != nullptr && "Unable to get normal texture");
	if (normalTexture.GetTexture() == nullptr)
	{
		return;
	}

	TextureInstance metalnessTexture;
	if (!aCustom && fbxModel->myTextures[10] != "")
	{
		const std::string metalnessPath(folderPath + fbxModel->myTextures[10]);
		metalnessTexture = TextureInstance(metalnessPath);
	}
	else
	{
		metalnessTexture = TextureInstance("Models\\Default\\metalness.dds");
	}
	assert(metalnessTexture.GetTexture() != nullptr && "Unable to get normal texture");
	if (metalnessTexture.GetTexture() == nullptr)
	{
		return;
	}

	if (aCustom)
	{
		model->myIsDefaultTexture.set();
	}
	else
	{
		model->myIsDefaultTexture[0] = fbxModel->myTextures[0] == "";
		model->myIsDefaultTexture[1] = fbxModel->myTextures[1] == "";
		model->myIsDefaultTexture[2] = fbxModel->myTextures[2] == "";
		model->myIsDefaultTexture[3] = fbxModel->myTextures[3] == "";
		model->myIsDefaultTexture[4] = fbxModel->myTextures[5] == "";
		model->myIsDefaultTexture[5] = fbxModel->myTextures[10] == "";
	}

	assert(albedoTexture.GetTexture() != nullptr);
	assert(normalTexture.GetTexture() != nullptr);
	assert(metalnessTexture.GetTexture() != nullptr);
	assert(roughnessTexture.GetTexture() != nullptr);
	assert(ambientOcclusionTexture.GetTexture() != nullptr);
	assert(emissiveTexture.GetTexture() != nullptr);
	model->myTexture[0] = albedoTexture;
	model->myTexture[1] = normalTexture;
	model->myTexture[2] = metalnessTexture;
	model->myTexture[3] = roughnessTexture;
	model->myTexture[4] = ambientOcclusionTexture;
	model->myTexture[5] = emissiveTexture;
	#pragma endregion

	model->myPath = aPath;
	model->myUserCount = 0;

	model->AddUser();
	*outModel = model;
	if (!aCustom)
	{
		delete fbxModel;
	}
}

void ModelLoader::LoadAnimations(const std::string& aPath, SkinnedModel* aModel)
{
	if (aModel == nullptr)
	{
		return;
	}

	Skeleton* skeleton = aModel->mySkeleton;
	if (skeleton == nullptr)
	{
		return;
	}

	LoadAnimations(aPath, skeleton);
}
void ModelLoader::LoadAnimations(const std::string& aPath, Skeleton* aSkeleton)
{
	if (aSkeleton->myLoadedAnimationPathHashList.IsFull())
	{
		CU::Log("Skeleton loaded animation count exceeded", CU::LogType::Error);
		return;
	}

	const size_t pathHash = std::hash<std::string>{}(aPath);
	if (aSkeleton->myLoadedAnimationPathHashList.Contains(pathHash))
	{
		return;
	}
	aSkeleton->myLoadedAnimationPathHashList.Add(pathHash);

	CU::GrowingArray<FBXAnimation> loadedAnimations;
	myFBXLoader.LoadAnimations(loadedAnimations, aSkeleton->mySkeleton, aPath);
	LoadAnimations(loadedAnimations, aSkeleton);
}

void ModelLoader::LoadAnimations(const CU::GrowingArray<FBXAnimation>& aAnimationArray, Skeleton* aSkeleton)
{
	for (int index = 0; index < aAnimationArray.Size(); index++)
	{
		if (aSkeleton->myAnimations.IsFull())
		{
			CU::Log("Skeleton animation count exceeded", CU::LogType::Error);
			break;
		}
		if (aSkeleton->myAnimationIndexMap.Get(aAnimationArray[index].GetName()) != nullptr)
		{
			CU::Log("Animation on skeleton \"" + aSkeleton->myPath + "\" called \"" + aAnimationArray[index].GetName() + "\" already exists", CU::LogType::Warning);
			continue;
		}

		Animation& animation = CreateAnimationEntry(aAnimationArray[index]);
		const unsigned int animationSize = animation.GetFrameCount() * aSkeleton->GetBoneCount() * 4;
		animation.SetAnimationSize(animationSize);
		animation.SetAnimationDeltaSize(animationSize);
		const int animationIndex = myAnimationList.Size() - 1;
		if (animationIndex > 0)
		{
			const Animation& previousAnimation = myAnimationList[animationIndex - 1];
			const unsigned int animationOffset = previousAnimation.GetAnimationOffset() + previousAnimation.GetAnimationSize() + previousAnimation.GetAnimationDeltaSize();
			animation.SetAnimationOffset(animationOffset);
			animation.SetAnimationDeltaOffset(animationOffset + animationSize);
		}

		aSkeleton->myAnimations.Add(&animation);
		if (aSkeleton->myAnimationIndexMap.Create(animation.GetName()))
		{
			int& indexPointer = *aSkeleton->myAnimationIndexMap.Get(animation.GetName());
			indexPointer = aSkeleton->myAnimations.Size() - 1;
		}
	}
}

Animation& ModelLoader::CreateAnimationEntry(const FBXAnimation& aAnimation)
{
	Animation& animation = myAnimationList.Add();
	animation.myBoneTransforms = aAnimation.GetBoneTransformLists();
	animation.myBoneDeltas = aAnimation.GetBoneDeltaLists();
	animation.myName = aAnimation.GetName();
	animation.myTicksPerSecond = aAnimation.GetTicksPerSecond();
	animation.myDuration = aAnimation.GetDuration();

	return animation;
}

float ModelLoader::CalculateEncapsulationRange(const FBXMesh* aMesh) const
{
	if (aMesh == nullptr || aMesh->myVertexCount < 1)
	{
		return 0.0f;
	}
	char* vertexChunk = aMesh->myVerticies;
	const unsigned int vertexSize = aMesh->myVertexBufferSize;
	float furthestDistanceSqr = 0.0f;
	int furthestVertexIndex = -1;

	for (int vertexIndex = 0; vertexIndex < aMesh->myVertexCount; vertexIndex++)
	{
		const CU::Vector3f pos(*reinterpret_cast<CU::Vector3f*>(&vertexChunk[vertexIndex * vertexSize]));
		const float lengthSqr = pos.LengthSqr();
		if (furthestVertexIndex == -1 || lengthSqr > furthestDistanceSqr)
		{
			furthestVertexIndex = vertexIndex;
			furthestDistanceSqr = lengthSqr;
		}
	}

	const CU::Vector3f furthestVertex(*reinterpret_cast<CU::Vector3f*>(&vertexChunk[furthestVertexIndex * vertexSize]));
	return furthestVertex.Length();
}

float ModelLoader::CalculateEncapsulationRange(const CustomMesh* aMesh) const
{
	if (aMesh == nullptr || aMesh->myVertices.Size() < 1)
	{
		return 0.0f;
	}

	float furthestDistanceSqr = 0.0f;
	int furthestVertexIndex = -1;
	for (int vertexIndex = 0; vertexIndex < aMesh->myVertices.Size(); vertexIndex++)
	{
		const CU::Vector3f pos(aMesh->myVertices[vertexIndex]);
		const float lengthSqr = pos.LengthSqr();
		if (furthestVertexIndex == -1 || lengthSqr > furthestDistanceSqr)
		{
			furthestVertexIndex = vertexIndex;
			furthestDistanceSqr = lengthSqr;
		}
	}

	return aMesh->myVertices[furthestVertexIndex].Length();
}

void ModelLoader::UnloadModel(const std::string& aPath)
{
	ModelLoadMutex.lock();
	Model* model = myModelMap.Get(aPath);
	assert(model != nullptr && "Could not find model to unload");
	if (model == nullptr)
	{
		return;
	}

	model->myData[0].myIndexBuffer->Release();
	model->myData[0].myVertexBuffer->Release();
	model->myTexture[0].Release();
	model->myTexture[1].Release();
	model->myTexture[2].Release();
	model->myTexture[3].Release();
	model->myTexture[4].Release();
	model->myTexture[5].Release();

	myModelMap.Remove(aPath);
	ModelLoadMutex.unlock();
}

void ModelLoader::UnloadModel(const std::string& aPath, const std::string& aSkeletonPath)
{
	ModelLoadMutex.lock();
	const std::string skinnedPath(aPath + aSkeletonPath);
	SkinnedModel* model = mySkinnedModelMap.Get(skinnedPath);
	assert(model != nullptr && "Could not find model to unload");
	if (model == nullptr)
	{
		return;
	}

	model->myData[0].myIndexBuffer->Release();
	model->myData[0].myVertexBuffer->Release();
	model->myTexture[0].Release();
	model->myTexture[1].Release();
	model->myTexture[2].Release();
	model->myTexture[3].Release();
	model->myTexture[4].Release();
	model->myTexture[5].Release();

	mySkinnedModelMap.Remove(skinnedPath);
	ModelLoadMutex.unlock();
}

bool ModelLoader::IsLoading() const
{
	return myIsLoading || myLoadCommands.Size() > 0;
}
