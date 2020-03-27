#pragma once
#include "Texture.h"
#include <string>

class TextureInstance
{
public:
	TextureInstance();
	TextureInstance(const std::string& aPath);
	TextureInstance(const TextureInstance& aInstance);
	TextureInstance& operator=(const TextureInstance& aInstance);
	~TextureInstance();
	void Init(const std::string& aPath);
	void Release();

	Texture* GetTexture() const;

private:
	std::string myPath;
	Texture* myTexture;
};