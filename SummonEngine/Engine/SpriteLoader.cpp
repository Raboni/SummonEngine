#include "stdafx.h"
#include "SpriteLoader.h"
#include "DDSTextureLoader.h"
#include "SpriteInstance.h"
#include <fstream>
#include <codecvt>
#include <mutex>
#include <winnt.h>

std::mutex SpriteMutex;

SpriteLoader* SpriteLoader::myInstance = nullptr;
void SpriteLoader::CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxSpriteAmount)
{
	if (myInstance == nullptr)
	{
		myInstance = new SpriteLoader(aDevice, aMaxSpriteAmount);
	}
}
SpriteLoader* SpriteLoader::GetInstance()
{
	return myInstance;
}
void SpriteLoader::DestroyInstance()
{
	if (myInstance != nullptr)
	{
		delete myInstance;
		myInstance = nullptr;
	}
}

Sprite* SpriteLoader::GetSprite(const std::string& aPath)
{
	SpriteMutex.lock();
	Sprite* sprite = mySpriteMap.Get(aPath);
	if (sprite == nullptr)
	{
		sprite = LoadSprite(aPath);
	}
	sprite->AddUser();
	SpriteMutex.unlock();
	return sprite;
}

void SpriteLoader::UnloadSprite(const std::string& aPath)
{
	SpriteMutex.lock();
	Sprite* sprite = mySpriteMap.Get(aPath);
	assert(sprite != nullptr && "Could not find sprite to unload");
	if (sprite == nullptr)
	{
		return;
	}

	mySpriteMap.Remove(aPath);
	SpriteMutex.unlock();

	sprite->myTexture->Release();
	sprite->myTexture = nullptr;
	sprite->myData.myVertexShader->Release();
	sprite->myData.myVertexShader = nullptr;
	sprite->myData.myPixelShader->Release();
	sprite->myData.myPixelShader = nullptr;
	sprite->myData.myInputLayout->Release();
	sprite->myData.myInputLayout = nullptr;
}

SpriteLoader::SpriteLoader(ID3D11Device* aDevice, const unsigned int aMaxSpriteAmount) : mySpriteMap(aMaxSpriteAmount)
{
	myDevice = aDevice;
}
SpriteLoader::~SpriteLoader()
{
}

Sprite* SpriteLoader::LoadSprite(const std::string& aPath)
{
	HRESULT result;
	if (!mySpriteMap.Create(aPath))
	{
		return nullptr;
	}
	Sprite* sprite = mySpriteMap.Get(aPath);
	assert(sprite != nullptr && "Could not find created texture");
	if (sprite == nullptr)
	{
		return sprite;
	}

	/*#pragma region Buffer
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.ByteWidth = sizeof(SpriteRenderData);
	bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

	ID3D11Buffer* vertexBuffer;
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &vertexBuffer);
	assert(!FAILED(result) && "Failed to create sprite vertex buffer");
	if (FAILED(result))
	{
		return nullptr;
	}
	#pragma endregion*/

	#pragma region Shader
	std::ifstream vsFile;
	vsFile.open("Shaders\\SpriteVertex.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	assert(!FAILED(result) && "Failed to create sprite vertex shader");
	if (FAILED(result))
	{
		return nullptr;
	}
	vsFile.close();

	std::ifstream gsFile;
	gsFile.open("Shaders\\SpriteGeometry.cso", std::ios::binary);
	std::string gsData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
	ID3D11GeometryShader* geometryShader;
	result = myDevice->CreateGeometryShader(gsData.data(), gsData.size(), nullptr, &geometryShader);
	assert(!FAILED(result) && "Failed to create sprite geometry shader");
	if (FAILED(result))
	{
		return nullptr;
	}
	gsFile.close();

	std::ifstream psFile;
	psFile.open("Shaders\\SpritePixel.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ID3D11PixelShader* pixelShader;
	result = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Failed to create sprite pixel shader");
	if (FAILED(result))
	{
		return nullptr;
	}
	psFile.close();
	#pragma endregion

	#pragma region Layout
	ID3D11InputLayout* inputLayout;
	CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> layout(8);
	layout.Add({ "COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "SIZE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "PIVOT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "ROTATION", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	result = myDevice->CreateInputLayout(layout.GetPointer(), layout.Size(), vsData.data(), vsData.size(), &inputLayout);
	assert(!FAILED(result) && "Failed to create sprite input layout");
	if (FAILED(result))
	{
		return nullptr;
	}
	#pragma endregion

	std::wstring spritePath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(aPath);
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	result = DirectX::CreateDDSTextureFromFile(myDevice, spritePath.c_str(), nullptr, &shaderResourceView);
	assert(!FAILED(result) && "Failed to create sprite texture");
	if (FAILED(result))
	{
		return nullptr;
	}

	SpriteData data;
	data.myVertexShader = vertexShader;
	data.myGeometryShader = geometryShader;
	data.myPixelShader = pixelShader;
	data.myInputLayout = inputLayout;
	data.myOffset = 0;
	data.myStride = sizeof(SpriteRenderData);
	data.myTopology = static_cast<unsigned short>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	sprite->myData = data;
	sprite->myTexture = shaderResourceView;
	sprite->myUserCount = 0;
	return sprite;
}