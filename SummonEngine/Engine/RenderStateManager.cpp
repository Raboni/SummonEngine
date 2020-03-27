#include "stdafx.h"
#include "RenderStateManager.h"
#include "DXFramework.h"
#include <DebugTools.h>
#include <d3d11.h>

RenderStateManager::RenderStateManager()
{
	std::memset(&myBlendStates[0], 0, sizeof(ID3D11BlendState*) * myBlendStates.size());
	std::memset(&myDepthStencilStates[0], 0, sizeof(ID3D11DepthStencilState*) * myDepthStencilStates.size());
	std::memset(&myRasterizerStates[0], 0, sizeof(ID3D11RasterizerState*) * myRasterizerStates.size());
	std::memset(&mySamplerStates[0], 0, sizeof(ID3D11SamplerState*) * mySamplerStates.size());
	myContext = nullptr;
	myCurrentBlendState = 0;
	myCurrentDepthStencilState = 0;
	myCurrentRasterizerState = 0;
	myCurrentSamplerState = 0;
}

bool RenderStateManager::Init(DXFramework* aFramework)
{
	if (aFramework == nullptr)
	{
		return false;
	}

	myContext = aFramework->GetDeviceContext();
	ID3D11Device* device = aFramework->GetDevice();
	CreateBlendStates(device);
	CreateDepthStencilStates(device);
	CreateRasterizerStates(device);
	CreateSamplerStates(device);

	return true;
}

void RenderStateManager::SetBlendState(const BlendState aState, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		if (myCurrentBlendState != static_cast<int>(aState))
		{
			myCurrentBlendState = static_cast<int>(aState);
			myContext->OMSetBlendState(myBlendStates[static_cast<int>(aState)], nullptr, 0xffffffff);
		}
	}
	else
	{
		aContext->OMSetBlendState(myBlendStates[static_cast<int>(aState)], nullptr, 0xffffffff);
	}
}
void RenderStateManager::SetDepthStencilState(const DepthStencilState aState, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		if (myCurrentDepthStencilState != static_cast<int>(aState))
		{
			myCurrentDepthStencilState = static_cast<int>(aState);
			myContext->OMSetDepthStencilState(myDepthStencilStates[static_cast<int>(aState)], 0);
		}
	}
	else
	{
		aContext->OMSetDepthStencilState(myDepthStencilStates[static_cast<int>(aState)], 0);
	}
}
void RenderStateManager::SetRasterizerState(const RasterizerState aState, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		if (myCurrentRasterizerState != static_cast<int>(aState))
		{
			myCurrentRasterizerState = static_cast<int>(aState);
			myContext->RSSetState(myRasterizerStates[static_cast<int>(aState)]);
		}
	}
	else
	{
		aContext->RSSetState(myRasterizerStates[static_cast<int>(aState)]);
	}
}
void RenderStateManager::SetSamplerState(const SamplerState aState, const unsigned int aSlot, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		myContext->VSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
		//myContext->CSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
		//myContext->HSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
		//myContext->DSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
		myContext->GSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
		myContext->PSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
	}
	else
	{
		aContext->VSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
		aContext->GSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
		aContext->PSSetSamplers(aSlot, 1, &mySamplerStates[static_cast<int>(aState)]);
	}
}

void RenderStateManager::SetStatesDefault(ID3D11DeviceContext* aContext)
{
	SetBlendState(BlendState::Disable, aContext);
	SetDepthStencilState(DepthStencilState::Default, aContext);
	SetRasterizerState(RasterizerState::Default, aContext);
	SetSamplerState(SamplerState::Trilinear, 0, aContext);
	SetSamplerState(SamplerState::Trilinear, 1, aContext);
	myCurrentBlendState = 0;
	myCurrentDepthStencilState = 0;
	myCurrentRasterizerState = 0;
	myCurrentSamplerState = 0;
}

bool RenderStateManager::CreateBlendStates(ID3D11Device* aDevice)
{
	HRESULT result;

	D3D11_BLEND_DESC alphaDesc = { 0 };
	alphaDesc.RenderTarget[0].BlendEnable = true;
	alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC additativeDesc = { 0 };
	additativeDesc.RenderTarget[0].BlendEnable = true;
	additativeDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	additativeDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additativeDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additativeDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additativeDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	additativeDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	additativeDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* alphaState;
	result = aDevice->CreateBlendState(&alphaDesc, &alphaState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create blend state", CU::LogType::Error);
		return false;
	}
	ID3D11BlendState* additativeState;
	result = aDevice->CreateBlendState(&additativeDesc, &additativeState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create blend state", CU::LogType::Error);
		return false;
	}

	myBlendStates[static_cast<int>(BlendState::Disable)] = nullptr;
	myBlendStates[static_cast<int>(BlendState::Alphablend)] = alphaState;
	myBlendStates[static_cast<int>(BlendState::Addativeblend)] = additativeState;
	return true;
}
bool RenderStateManager::CreateDepthStencilStates(ID3D11Device* aDevice)
{
	HRESULT result;

	D3D11_DEPTH_STENCIL_DESC readOnlyDesc = { 0 };
	readOnlyDesc.DepthEnable = true;
	readOnlyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDesc.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDesc.StencilEnable = false;

	D3D11_DEPTH_STENCIL_DESC readWriteEqualDepthDesc = { 0 };
	readWriteEqualDepthDesc.DepthEnable = true;
	readWriteEqualDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	readWriteEqualDepthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	readWriteEqualDepthDesc.StencilEnable = false;

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = false;

	ID3D11DepthStencilState* readOnlyState = nullptr;
	result = aDevice->CreateDepthStencilState(&readOnlyDesc, &readOnlyState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create depth stencil state", CU::LogType::Error);
		return false;
	}
	ID3D11DepthStencilState* readWriteEqualDepthStencilState = nullptr;
	result = aDevice->CreateDepthStencilState(&readWriteEqualDepthDesc, &readWriteEqualDepthStencilState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create depth stencil state", CU::LogType::Error);
		return false;
	}
	ID3D11DepthStencilState* ignoreDepthState = nullptr;
	result = aDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &ignoreDepthState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create depth stencil state", CU::LogType::Error);
		return false;
	}

	myDepthStencilStates[static_cast<int>(DepthStencilState::Default)] = nullptr;
	myDepthStencilStates[static_cast<int>(DepthStencilState::ReadOnly)] = readOnlyState;
	myDepthStencilStates[static_cast<int>(DepthStencilState::ReadWriteLessEqual)] = readWriteEqualDepthStencilState;
	myDepthStencilStates[static_cast<int>(DepthStencilState::IgnoreDepth)] = ignoreDepthState;
	return true;
}
bool RenderStateManager::CreateRasterizerStates(ID3D11Device* aDevice)
{
	HRESULT result;

	D3D11_RASTERIZER_DESC wireframeDesc = {};
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.DepthClipEnable = true;

	ID3D11RasterizerState* wireframeState;
	result = aDevice->CreateRasterizerState(&wireframeDesc, &wireframeState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create rasterizer state", CU::LogType::Error);
		return false;
	}

	myRasterizerStates[static_cast<int>(RasterizerState::Default)] = nullptr;
	myRasterizerStates[static_cast<int>(RasterizerState::Wireframe)] = wireframeState;
	return true;
}
bool RenderStateManager::CreateSamplerStates(ID3D11Device* aDevice)
{
	HRESULT result;

	D3D11_SAMPLER_DESC pointDesc = {};
	pointDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pointDesc.MinLOD = -FLT_MAX;
	pointDesc.MaxLOD = FLT_MAX;

	D3D11_SAMPLER_DESC wrappedDesc = {};
	wrappedDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	wrappedDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	wrappedDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	wrappedDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	wrappedDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	wrappedDesc.MinLOD = -FLT_MAX;
	wrappedDesc.MaxLOD = FLT_MAX;

	ID3D11SamplerState* pointState;
	result = aDevice->CreateSamplerState(&pointDesc, &pointState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create sampler state", CU::LogType::Error);
		return false;
	}
	ID3D11SamplerState* wrappedState;
	result = aDevice->CreateSamplerState(&wrappedDesc, &wrappedState);
	if (FAILED(result))
	{
		CU::Log("RenderStateManager failed to create sampler state", CU::LogType::Error);
		return false;
	}

	mySamplerStates[static_cast<int>(SamplerState::Trilinear)] = nullptr;
	mySamplerStates[static_cast<int>(SamplerState::Point)] = pointState;
	mySamplerStates[static_cast<int>(SamplerState::Wrapped)] = wrappedState;
	return true;
}
