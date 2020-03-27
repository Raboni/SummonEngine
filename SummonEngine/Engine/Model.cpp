#include "stdafx.h"
#include "Model.h"

const unsigned short Model::ourLODCount = 8;

const MeshData* Model::GetMeshArray() const
{
	return myData;
}

TextureInstance* Model::GetTextureArray()
{
	return myTexture;
}
const TextureInstance* Model::GetTextureArray() const
{
	return myTexture;
}

const std::string& Model::GetPath() const
{
	return myPath;
}

VertexShader* Model::GetVertexShader() const
{
	return myVertexShader;
}
GeometryShader* Model::GetGeometryShader() const
{
	return myGeometryShader;
}
PixelShader* Model::GetPixelShader() const
{
	return myPixelShader;
}

unsigned int Model::GetMeshCount() const
{
	return myMeshCount;
}

float Model::GetEncapsulationRange() const
{
	return myEncapsulationRange;
}

bool Model::IsAlbedoDefaultTexture() const
{
	return myIsDefaultTexture[0];
}
bool Model::IsNormalDefaultTexture() const
{
	return myIsDefaultTexture[1];
}
bool Model::IsMetalDefaultTexture() const
{
	return myIsDefaultTexture[2];
}
bool Model::IsRoughDefaultTexture() const
{
	return myIsDefaultTexture[3];
}
bool Model::IsAODefaultTexture() const
{
	return myIsDefaultTexture[4];
}
bool Model::IsEmissiveDefaultTexture() const
{
	return myIsDefaultTexture[5];
}

bool Model::IsSkinned() const
{
	return myIsSkinned;
}
bool Model::IsCustom() const
{
	return myIsCustom;
}

Model::Model()
{
	myVertexShader = nullptr;
	myGeometryShader = nullptr;
	myPixelShader = nullptr;
	myMeshCount = 1;
	myUserCount = 0;
	myEncapsulationRange = 0.0f;
	myIsDefaultTexture.reset();
	myIsSkinned = false;
	myIsCustom = false;
}

void Model::AddUser()
{
	myUserCount++;
}
void Model::RemoveUser()
{
	myUserCount--;
}
unsigned int Model::GetUserCount() const
{
	return myUserCount;
}