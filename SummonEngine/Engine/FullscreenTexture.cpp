#include "stdafx.h"
#include "FullscreenTexture.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

FullscreenTexture::FullscreenTexture()
{
	myContext = nullptr;
	myTexture = nullptr;
	myRenderTarget = nullptr;
	myShaderResource = nullptr;
	myViewport = nullptr;
}
FullscreenTexture::~FullscreenTexture()
{
}

void FullscreenTexture::ClearTexture(const CU::Vector4f& aColor, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		myContext->ClearRenderTargetView(myRenderTarget, &aColor.x);
	}
	else
	{
		aContext->ClearRenderTargetView(myRenderTarget, &aColor.x);
	}
}
void FullscreenTexture::ClearDepth(const float aClearDepth, const unsigned int aClearStencil, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		myContext->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepth, static_cast<unsigned char>(aClearStencil));
	}
	else
	{
		aContext->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepth, static_cast<unsigned char>(aClearStencil));
	}
}

void FullscreenTexture::ReleaseTexture()
{
	if (myContext != nullptr)
	{
		myContext = nullptr;
		myTexture->Release();
		myTexture = nullptr;
		myRenderTarget->Release();
		myRenderTarget = nullptr;
		if (myShaderResource != nullptr)
		{
			myShaderResource->Release();
			myShaderResource = nullptr;
		}
		delete myViewport;
		myViewport = nullptr;
	}
}
void FullscreenTexture::ReleaseDepth()
{
	if (myContext != nullptr)
	{
		myContext = nullptr;
		myTexture->Release();
		myTexture = nullptr;
		myDepth->Release();
		myDepth = nullptr;
		if (myShaderResource != nullptr)
		{
			myShaderResource->Release();
			myShaderResource = nullptr;
		}
		delete myViewport;
		myViewport = nullptr;
	}
}

void FullscreenTexture::SetAsActiveTarget(FullscreenTexture* aDepth, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		if (aDepth != nullptr)
		{
			myContext->OMSetRenderTargets(1, &myRenderTarget, aDepth->myDepth);
		}
		else
		{
			myContext->OMSetRenderTargets(1, &myRenderTarget, nullptr);
		}
		myContext->RSSetViewports(1, myViewport);
	}
	else
	{
		if (aDepth != nullptr)
		{
			aContext->OMSetRenderTargets(1, &myRenderTarget, aDepth->myDepth);
		}
		else
		{
			aContext->OMSetRenderTargets(1, &myRenderTarget, nullptr);
		}
		aContext->RSSetViewports(1, myViewport);
	}
}
void FullscreenTexture::SetAsResourceOnSlot(const unsigned int aSlot, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		myContext->PSSetShaderResources(aSlot, 1, &myShaderResource);
	}
	else
	{
		aContext->PSSetShaderResources(aSlot, 1, &myShaderResource);
	}
}