#include "stdafx.h"
#include "TextureInstance.h"
#include "TextureLoader.h"

TextureInstance::TextureInstance()
{
	myTexture = nullptr;
	myPath = "";
}
TextureInstance::TextureInstance(const std::string& aPath)
{
	myTexture = nullptr;
	Init(aPath);
}
TextureInstance::TextureInstance(const TextureInstance& aInstance)
{
	myPath = aInstance.myPath;
	myTexture = aInstance.myTexture;
	if (myTexture != nullptr)
	{
		myTexture->AddUser();
	}
}
TextureInstance& TextureInstance::operator=(const TextureInstance& aInstance)
{
	Release();
	myPath = aInstance.myPath;
	myTexture = aInstance.myTexture;
	if (myTexture != nullptr)
	{
		myTexture->AddUser();
	}
	return *this;
}
TextureInstance::~TextureInstance()
{
	Release();
}
void TextureInstance::Init(const std::string& aPath)
{
	Release();
	myPath = aPath;
	myTexture = TextureLoader::GetInstance()->GetTexture(myPath);
	assert(myTexture != nullptr && "Unable to get texture");
}

void TextureInstance::Release()
{
	if (myTexture != nullptr)
	{
		myTexture->RemoveUser();
		if (myTexture->GetUserCount() <= 0)
		{
			TextureLoader::GetInstance()->UnloadTexture(myPath);
		}
		myTexture = nullptr;
	}
}

Texture* TextureInstance::GetTexture() const
{
	return myTexture;
}
