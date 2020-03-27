#pragma once
#include "FullscreenTexture.h"
#include "GBuffer.h"
#include <dxgiformat.h>
#include <Vector2.hpp>

class DXFramework;

class FullscreenTextureFactory
{
public:
	FullscreenTextureFactory();
	~FullscreenTextureFactory();

	bool Init(DXFramework* aFramework);

	FullscreenTexture CreateTexture(const CU::Vector2f& aSize, DXGI_FORMAT aFormat);
	FullscreenTexture CreateTexture(ID3D11Texture2D* aTexture);
	FullscreenTexture CreateDepth(const CU::Vector2f& aSize, DXGI_FORMAT aFormat);
	GBuffer CreateGBuffer(const CU::Vector2f& aSize);

private:
	DXFramework* myFramework;
};