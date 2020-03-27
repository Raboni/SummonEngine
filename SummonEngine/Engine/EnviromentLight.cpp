#include "stdafx.h"
#include "EnviromentLight.h"
#include "DDSTextureLoader.h"
#include <DebugTools.h>
#include <codecvt>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

EnviromentLight::EnviromentLight()
{
	myMipLevels = 0;
	myCubemap = nullptr;
}
EnviromentLight::EnviromentLight(const std::string& aCubemapPath, ID3D11Device* aDevice)
{
	Init(aCubemapPath, aDevice);
}

void EnviromentLight::Init(const std::string& aCubemapPath, ID3D11Device* aDevice)
{
	myPath = aCubemapPath;
	const std::wstring cubemapPathW(myPath.begin(), myPath.end());
	const HRESULT result = DirectX::CreateDDSTextureFromFile(aDevice, cubemapPathW.c_str(), nullptr, &myCubemap);
	assert(!FAILED(result) && "Failed to create enviroment light cubemap");
	if (FAILED(result))
	{
		CU::Log("Failed to create enviroment light cubemap", CU::LogType::Error);
		return;
	}

	ID3D11Resource* cubeResource;
	myCubemap->GetResource(&cubeResource);
	ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(cubeResource);
	D3D11_TEXTURE2D_DESC cubeDesc;
	cubeTexture->GetDesc(&cubeDesc);
	myMipLevels = cubeDesc.MipLevels;
}

void EnviromentLight::SetColor(const CU::Vector3f& aColor)
{
	myColor = aColor;
}
void EnviromentLight::SetDirection(const CU::Vector3f& aDirection)
{
	myDirection = aDirection;
}

const CU::Vector3f& EnviromentLight::GetColor() const
{
	return myColor;
}
const CU::Vector3f& EnviromentLight::GetDirection() const
{
	return myDirection;
}
const std::string& EnviromentLight::GetPath() const
{
	return myPath;
}
ID3D11ShaderResourceView* const* EnviromentLight::GetCubemap() const
{
	return &myCubemap;
}

const int EnviromentLight::GetMipLevels() const
{
	return myMipLevels;
}
