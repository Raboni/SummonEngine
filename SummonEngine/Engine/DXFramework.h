#pragma once
#include "WindowHandler.h"
#include "RenderSettings.h"
#include <GrowingArray.hpp>

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;

class DXFramework
{
public:
	DXFramework(RenderSettings& aSettings);
	~DXFramework();

	bool Init(WindowHandler& aWindowHandler);
	void BeginFrame();
	void EndFrame();

	ID3D11DeviceContext* GetDeviceContext(const bool aDeferred = false);
	ID3D11Device* GetDevice();
	ID3D11Texture2D* GetBackbufferTexture();

private:
	RenderSettings& mySettings;
	CU::GrowingArray<ID3D11DeviceContext*> myDeferredContexts;
	IDXGISwapChain* mySwapChain;
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myDeviceContext;
};