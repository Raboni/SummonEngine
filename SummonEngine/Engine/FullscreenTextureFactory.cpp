#include "stdafx.h"
#include "FullscreenTextureFactory.h"
#include "DXFramework.h"
#include <d3d11.h>

FullscreenTextureFactory::FullscreenTextureFactory()
{
	myFramework = nullptr;
}
FullscreenTextureFactory::~FullscreenTextureFactory()
{
}

bool FullscreenTextureFactory::Init(DXFramework* aFramework)
{
	myFramework = aFramework;
	return aFramework != nullptr;
}

FullscreenTexture FullscreenTextureFactory::CreateTexture(const CU::Vector2f& aSize, DXGI_FORMAT aFormat)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC textureDescription = { 0 };
	textureDescription.Width = static_cast<unsigned int>(aSize.x);
	textureDescription.Height = static_cast<unsigned int>(aSize.y);
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = aFormat;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;
	textureDescription.Usage = D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescription.CPUAccessFlags = 0;
	textureDescription.MiscFlags = 0;

	ID3D11Texture2D* texture = nullptr;
	result = myFramework->GetDevice()->CreateTexture2D(&textureDescription, nullptr, &texture);
	assert(!FAILED(result) && "Fullscreen texture creation failed");
	if (FAILED(result))
	{
		return FullscreenTexture();
	}

	FullscreenTexture returnTexture = CreateTexture(texture);

	ID3D11ShaderResourceView* shaderResource = nullptr;
	result = myFramework->GetDevice()->CreateShaderResourceView(texture, nullptr, &shaderResource);
	assert(!FAILED(result) && "Fullscreen texture creation failed");
	if (FAILED(result))
	{
		return FullscreenTexture();
	}

	returnTexture.myShaderResource = shaderResource;
	return returnTexture;
}

FullscreenTexture FullscreenTextureFactory::CreateTexture(ID3D11Texture2D* aTexture)
{
	HRESULT result;
	ID3D11RenderTargetView* renderTarget = nullptr;
	result = myFramework->GetDevice()->CreateRenderTargetView(aTexture, nullptr, &renderTarget);
	assert(!FAILED(result) && "Fullscreen texture creation failed");
	if (FAILED(result))
	{
		return FullscreenTexture();
	}

	D3D11_VIEWPORT* viewport = nullptr;
	if (aTexture)
	{
		D3D11_TEXTURE2D_DESC textureDescription;
		aTexture->GetDesc(&textureDescription);
		viewport = new D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(textureDescription.Width), static_cast<float>(textureDescription.Height), 0.0f, 1.0f });
	}

	FullscreenTexture returnTexture;
	returnTexture.myContext = myFramework->GetDeviceContext();
	returnTexture.myTexture = aTexture;
	returnTexture.myRenderTarget = renderTarget;
	returnTexture.myViewport = viewport;
	return returnTexture;
}

FullscreenTexture FullscreenTextureFactory::CreateDepth(const CU::Vector2f& aSize, DXGI_FORMAT aFormat)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC textureDescription = { 0 };
	textureDescription.Width = static_cast<unsigned int>(aSize.x);
	textureDescription.Height = static_cast<unsigned int>(aSize.y);
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = aFormat;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;
	textureDescription.Usage = D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDescription.CPUAccessFlags = 0;
	textureDescription.MiscFlags = 0;

	ID3D11Texture2D* texture = nullptr;
	result = myFramework->GetDevice()->CreateTexture2D(&textureDescription, nullptr, &texture);
	assert(!FAILED(result) && "Fullscreen texture depth creation failed");
	if (FAILED(result))
	{
		return FullscreenTexture();
	}

	ID3D11DepthStencilView* depth = nullptr;
	result = myFramework->GetDevice()->CreateDepthStencilView(texture, nullptr, &depth);
	assert(!FAILED(result) && "Fullscreen texture depth creation failed");
	if (FAILED(result))
	{
		return FullscreenTexture();
	}

	D3D11_VIEWPORT* viewport = new D3D11_VIEWPORT({ 0.0f, 0.0f, aSize.x, aSize.y, 0.0f, 1.0f });

	FullscreenTexture returnDepth;
	returnDepth.myContext = myFramework->GetDeviceContext();
	returnDepth.myTexture = texture;
	returnDepth.myDepth = depth;
	returnDepth.myViewport = viewport;
	return returnDepth;
}

GBuffer FullscreenTextureFactory::CreateGBuffer(const CU::Vector2f& aSize)
{
	HRESULT result;

	std::array<DXGI_FORMAT, static_cast<int>(GBufferTextureType::Count)> textureFormats;
	textureFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	textureFormats[1] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM;
	textureFormats[2] = DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
	textureFormats[3] = DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
	textureFormats[4] = DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
	textureFormats[5] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	textureFormats[6] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;

	std::array<ID3D11Texture2D*, static_cast<int>(GBufferTextureType::Count)> textureArray;
	std::array<ID3D11RenderTargetView*, static_cast<int>(GBufferTextureType::Count)> renderTargetArray;
	std::array<ID3D11ShaderResourceView*, static_cast<int>(GBufferTextureType::Count)> shaderResourceArray;

	{
		D3D11_TEXTURE2D_DESC textureDescription;
		ID3D11Texture2D* texture;
		ID3D11RenderTargetView* renderTarget;
		ID3D11ShaderResourceView* shaderResource;
		for (int textureIndex = 0; textureIndex < static_cast<int>(GBufferTextureType::Count); textureIndex++)
		{
			textureDescription = { 0 };
			textureDescription.Width = static_cast<unsigned int>(aSize.x);
			textureDescription.Height = static_cast<unsigned int>(aSize.y);
			textureDescription.MipLevels = 1;
			textureDescription.ArraySize = 1;
			textureDescription.Format = textureFormats[textureIndex];
			textureDescription.SampleDesc.Count = 1;
			textureDescription.SampleDesc.Quality = 0;
			textureDescription.Usage = D3D11_USAGE_DEFAULT;
			textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDescription.CPUAccessFlags = 0;
			textureDescription.MiscFlags = 0;

			texture = nullptr;
			result = myFramework->GetDevice()->CreateTexture2D(&textureDescription, nullptr, &texture);
			assert(!FAILED(result) && "GBuffer texture creation failed");
			textureArray[textureIndex] = texture;

			renderTarget = nullptr;
			result = myFramework->GetDevice()->CreateRenderTargetView(texture, nullptr, &renderTarget);
			assert(!FAILED(result) && "GBuffer render target creation failed");
			renderTargetArray[textureIndex] = renderTarget;

			shaderResource = nullptr;
			result = myFramework->GetDevice()->CreateShaderResourceView(texture, nullptr, &shaderResource);
			assert(!FAILED(result) && "GBuffer shader resource creation failed");
			shaderResourceArray[textureIndex] = shaderResource;
		}
	}

	D3D11_VIEWPORT* viewport = new D3D11_VIEWPORT({ 0.0f, 0.0f, aSize.x, aSize.y, 0.0f, 1.0f });

	GBuffer returnBuffer;
	returnBuffer.myContext = myFramework->GetDeviceContext();
	returnBuffer.myTextures = textureArray;
	returnBuffer.myRenderTargets = renderTargetArray;
	returnBuffer.myShaderResources = shaderResourceArray;
	returnBuffer.myViewport = viewport;
	return returnBuffer;
}
