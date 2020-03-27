#include "stdafx.h"
#include "DXFramework.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "ModelLoader.h"
#include "LightLoader.h"
#include "SpriteLoader.h"
#include "ParticleEmitterLoader.h"
#include "TextLoader.h"
#include <d3d11.h>
#include <memory>
#include <Matrix4x4.hpp>
#include <Vector4.hpp>

#pragma comment(lib, "d3d11.lib")

DXFramework::DXFramework(RenderSettings& aSettings) : mySettings(aSettings)
{
	mySwapChain = nullptr;
	myDevice = nullptr;
	myDeviceContext = nullptr;
	myDeferredContexts.Init(16);
}
DXFramework::~DXFramework()
{
	TextLoader::DestroyInstance();
	SpriteLoader::DestroyInstance();
	ParticleEmitterLoader::DestroyInstance();
	LightLoader::DestroyInstance();
	ModelLoader::DestroyInstance();
	ShaderLoader::DestroyInstance();
	TextureLoader::DestroyInstance();

	mySwapChain->Release();
	myDevice->Release();
	myDeviceContext->Release();
}

bool DXFramework::Init(WindowHandler& aWindowHandler)
{
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDescription = {};
	swapChainDescription.BufferCount = 1;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.OutputWindow = aWindowHandler.GetWindow();
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.Windowed = !mySettings.Fullscreen;

	/*unsigned int flags = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
	#endif
	std::array<D3D_FEATURE_LEVEL, 6> featureLevels =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_1
	};

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, &featureLevels[0], static_cast<unsigned int>(featureLevels.size()), D3D11_SDK_VERSION,
		&swapChainDescription, &mySwapChain, &myDevice, nullptr, &myDeviceContext);*/
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&swapChainDescription, &mySwapChain, &myDevice, nullptr, &myDeviceContext);
	if (FAILED(result))
	{
		return false;
	}

	TextureLoader::CreateInstance(myDevice, 32);
	ShaderLoader::CreateInstance(myDevice, 16);
	ModelLoader::CreateInstance(myDevice, 16);
	LightLoader::CreateInstance(myDevice, 32);
	ParticleEmitterLoader::CreateInstance(myDevice, 4);
	SpriteLoader::CreateInstance(myDevice, 16);
	TextLoader::CreateInstance(myDevice, 128);
	return true;
}

void DXFramework::BeginFrame()
{
}
void DXFramework::EndFrame()
{
	HRESULT result = mySwapChain->Present(static_cast<unsigned int>(mySettings.VSync), 0);
	if (FAILED(result))
	{
		result = myDevice->GetDeviceRemovedReason();
	}
	assert(!FAILED(result) && "Cannot present swapchain");
}

ID3D11DeviceContext* DXFramework::GetDeviceContext(const bool aDeferred)
{
	if (!aDeferred)
	{
		return myDeviceContext;
	}
	ID3D11DeviceContext*& context = myDeferredContexts.Add();
	myDevice->CreateDeferredContext(0, &context);
	return context;
}

ID3D11Device* DXFramework::GetDevice()
{
	return myDevice;
}

ID3D11Texture2D* DXFramework::GetBackbufferTexture()
{
	HRESULT result;
	ID3D11Texture2D* backbufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbufferTexture);
	assert(!FAILED(result) && "Failed to get backbuffer texture");
	if (FAILED(result))
	{
		return nullptr;
	}
	return backbufferTexture;
}
