#include "stdafx.h"
#include "Sprite.h"
#include <d3d11.h>

ID3D11ShaderResourceView** Sprite::GetSRV()
{
	return &myTexture;
}

const SpriteData& Sprite::GetData() const
{
	return myData;
}

Sprite::Sprite()
{
	myUserCount = 0;
	myTexture = nullptr;
}
Sprite::~Sprite()
{
	if (myTexture != nullptr)
	{
		myTexture->Release();
	}
}

void Sprite::AddUser()
{
	myUserCount++;
}

void Sprite::RemoveUser()
{
	myUserCount--;
}

const unsigned int Sprite::GetUserCount() const
{
	return myUserCount;
}
