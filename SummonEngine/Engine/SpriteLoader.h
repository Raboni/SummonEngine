#pragma once
#include "Sprite.h"
#include <HashMap.hpp>

struct ID3D11Device;

class SpriteLoader
{
public:
	static void CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxSpriteAmount);
	static SpriteLoader* GetInstance();
	static void DestroyInstance();

	Sprite* GetSprite(const std::string& aPath);
	void UnloadSprite(const std::string& aPath);

private:
	SpriteLoader(ID3D11Device* aDevice, const unsigned int aMaxSpriteAmount);
	~SpriteLoader();

	Sprite* LoadSprite(const std::string& aPath);

private:
	static SpriteLoader* myInstance;

	CU::HashMap<std::string, Sprite> mySpriteMap;
	ID3D11Device* myDevice;
};