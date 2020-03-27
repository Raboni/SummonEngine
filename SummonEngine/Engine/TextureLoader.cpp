#include "stdafx.h"
#include "TextureLoader.h"
#include "DDSTextureLoader.h"
#include <codecvt>
#include <mutex>

std::mutex TextureMutex;

TextureLoader* TextureLoader::myInstance = nullptr;
void TextureLoader::CreateInstance(ID3D11Device* aDevice, unsigned int aMaxTextureAmount)
{
	if (myInstance == nullptr)
	{
		myInstance = new TextureLoader(aDevice, aMaxTextureAmount);
	}
}

TextureLoader* TextureLoader::GetInstance()
{
	return myInstance;
}

void TextureLoader::DestroyInstance()
{
	if (myInstance != nullptr)
	{
		delete myInstance;
		myInstance = nullptr;
	}
}

Texture* TextureLoader::GetTexture(const std::string& aPath)
{
	TextureMutex.lock();
	Texture* texture = myTextureMap.Get(aPath);
	if (texture == nullptr)
	{
		texture = LoadTexture(aPath);
	}
	texture->AddUser();
	TextureMutex.unlock();
	return texture;
}

Texture* TextureLoader::LoadTexture(const std::string& aPath)
{
	std::wstring texturePath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(aPath);
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	HRESULT result = DirectX::CreateDDSTextureFromFile(myDevice, texturePath.c_str(), nullptr, &shaderResourceView);
	assert(!FAILED(result) && "Texture creation failed");
	if (FAILED(result))
	{
		return nullptr;
	}
	
	bool createdInMap = myTextureMap.Create(aPath);
	assert(createdInMap && "Could not create new texture in hashmap");
	if (!createdInMap)
	{
		shaderResourceView->Release();
		return nullptr;
	}
	Texture* texture = myTextureMap.Get(aPath);
	assert(texture != nullptr && "Could not find created texture");
	if (texture == nullptr)
	{
		shaderResourceView->Release();
		return texture;
	}
	texture->myTexture = shaderResourceView;
	return texture;
}

void TextureLoader::UnloadTexture(const std::string& aPath)
{
	TextureMutex.lock();
	Texture* texture = myTextureMap.Get(aPath);
	assert(texture != nullptr && "Could not find texture to unload");
	if (texture == nullptr)
	{
		return;
	}

	myTextureMap.Remove(aPath);
	TextureMutex.unlock();

	texture->myTexture->Release();
	texture->myTexture = nullptr;
}

TextureLoader::TextureLoader(ID3D11Device* aDevice, unsigned int aMaxTextureAmount) : myTextureMap(aMaxTextureAmount)
{
	myDevice = aDevice;
}
TextureLoader::~TextureLoader()
{
}
