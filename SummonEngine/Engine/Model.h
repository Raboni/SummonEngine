#pragma once
#include "TextureInstance.h"
#include <HashMap.hpp>
#include <string>
#include <bitset>

struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
class ModelLoader;
class VertexShader;
class GeometryShader;
class PixelShader;

struct MeshData
{
	MeshData()
	{
		myVertexBuffer = nullptr;
		myIndexBuffer = nullptr;
		myVertexAmount = 0;
		myIndexAmount = 0;
		myStride = 0;
		myOffset = 0;
	}
	MeshData(const MeshData& aData)
	{
		myVertexBuffer = aData.myVertexBuffer;
		myIndexBuffer = aData.myIndexBuffer;
		myVertexAmount = aData.myVertexAmount;
		myIndexAmount = aData.myIndexAmount;
		myStride = aData.myStride;
		myOffset = aData.myOffset;
	}
	MeshData& operator=(const MeshData& aData)
	{
		myVertexBuffer = aData.myVertexBuffer;
		myIndexBuffer = aData.myIndexBuffer;
		myVertexAmount = aData.myVertexAmount;
		myIndexAmount = aData.myIndexAmount;
		myStride = aData.myStride;
		myOffset = aData.myOffset;
		return *this;
	}
	ID3D11Buffer* myVertexBuffer = nullptr;
	ID3D11Buffer* myIndexBuffer = nullptr;
	unsigned int myVertexAmount = 0;
	unsigned int myIndexAmount = 0;
	unsigned int myStride = 0;
	unsigned int myOffset = 0;
};

class Model
{
public:
	const MeshData* GetMeshArray() const;
	TextureInstance* GetTextureArray();
	const TextureInstance* GetTextureArray() const;
	const std::string& GetPath() const;
	VertexShader* GetVertexShader() const;
	GeometryShader* GetGeometryShader() const;
	PixelShader* GetPixelShader() const;
	unsigned int GetMeshCount() const;
	float GetEncapsulationRange() const;
	bool IsAlbedoDefaultTexture() const;
	bool IsNormalDefaultTexture() const;
	bool IsMetalDefaultTexture() const;
	bool IsRoughDefaultTexture() const;
	bool IsAODefaultTexture() const;
	bool IsEmissiveDefaultTexture() const;
	bool IsSkinned() const;
	bool IsCustom() const;

protected:
	friend class Engine;
	friend class Scene;
	friend class ModelLoader;
	friend class ModelInstance;
	friend struct ModelRenderCommand;
	friend class CU::HashMap<std::string, Model>;

	Model();
	~Model() = default;

	void AddUser();
	void RemoveUser();
	unsigned int GetUserCount() const;

protected:
	static const unsigned short ourLODCount;

	MeshData myData[8];
	TextureInstance myTexture[6];
	std::string myPath;
	VertexShader* myVertexShader;
	GeometryShader* myGeometryShader;
	PixelShader* myPixelShader;
	unsigned int myMeshCount;
	unsigned int myUserCount;
	float myEncapsulationRange;
	std::bitset<6> myIsDefaultTexture;
	bool myIsSkinned;
	bool myIsCustom;
};