#include "stdafx.h"
#include "Texture.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

ID3D11ShaderResourceView** Texture::GetSRV()
{
	return &myTexture;
}

Texture::Texture()
{
	myUserCount = 0;
	myTexture = nullptr;
}
Texture::~Texture()
{
	if (myTexture != nullptr)
	{
		myTexture->Release();
	}
}

void Texture::AddUser()
{
	myUserCount++;
}

void Texture::RemoveUser()
{
	myUserCount--;
}

unsigned int Texture::GetUserCount()
{
	return myUserCount;
}