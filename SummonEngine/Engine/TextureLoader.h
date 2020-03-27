#pragma once
#include "Texture.h"
#include <HashMap.hpp>

struct ID3D11Device;

class TextureLoader
{
public:
	static void CreateInstance(ID3D11Device* aDevice, unsigned int aMaxTextureAmount);
	static TextureLoader* GetInstance();
	static void DestroyInstance();

	Texture* GetTexture(const std::string& aPath);
	void UnloadTexture(const std::string& aPath);

private:
	TextureLoader(ID3D11Device* aDevice, unsigned int aMaxTextureAmount);
	~TextureLoader();
	Texture* LoadTexture(const std::string& aPath);

	static TextureLoader* myInstance;

	CU::HashMap<std::string, Texture> myTextureMap;
	ID3D11Device* myDevice;
};