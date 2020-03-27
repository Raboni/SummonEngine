#include "stdafx.h"
#include "GBuffer.h"
#include "FullscreenTexture.h"
#include <d3d11.h>

GBuffer::GBuffer()
{
	myContext = nullptr;
	myViewport = nullptr;
	memset(&myTextures, 0, static_cast<int>(GBufferTextureType::Count));
	memset(&myRenderTargets, 0, static_cast<int>(GBufferTextureType::Count));
	memset(&myShaderResources, 0, static_cast<int>(GBufferTextureType::Count));
}
GBuffer::~GBuffer()
{
}

void GBuffer::ClearTextures(const CU::Vector4f& aColor, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		for (int index = 0; index < static_cast<int>(GBufferTextureType::Count); index++)
		{
			myContext->ClearRenderTargetView(myRenderTargets[index], &aColor.x);
		}
	}
	else
	{
		for (int index = 0; index < static_cast<int>(GBufferTextureType::Count); index++)
		{
			aContext->ClearRenderTargetView(myRenderTargets[index], &aColor.x);
		}
	}
}

void GBuffer::SetAsActiveTarget(FullscreenTexture* aDepth, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		if (aDepth != nullptr)
		{
			myContext->OMSetRenderTargets(static_cast<unsigned int>(GBufferTextureType::Count), &myRenderTargets[0], aDepth->myDepth);
		}
		else
		{
			myContext->OMSetRenderTargets(static_cast<unsigned int>(GBufferTextureType::Count), &myRenderTargets[0], nullptr);
		}
		myContext->RSSetViewports(1, myViewport);
	}
	else
	{
		if (aDepth != nullptr)
		{
			aContext->OMSetRenderTargets(static_cast<unsigned int>(GBufferTextureType::Count), &myRenderTargets[0], aDepth->myDepth);
		}
		else
		{
			aContext->OMSetRenderTargets(static_cast<unsigned int>(GBufferTextureType::Count), &myRenderTargets[0], nullptr);
		}
		aContext->RSSetViewports(1, myViewport);
	}
}

void GBuffer::SetAsResourceOnSlot(GBufferTextureType aResourceType, unsigned int aSlot, ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		myContext->PSSetShaderResources(aSlot, 1, &myShaderResources[static_cast<unsigned int>(aResourceType)]);
	}
	else
	{
		aContext->PSSetShaderResources(aSlot, 1, &myShaderResources[static_cast<unsigned int>(aResourceType)]);
	}
}
void GBuffer::SetAllAsResources(ID3D11DeviceContext* aContext)
{
	if (aContext == nullptr)
	{
		myContext->PSSetShaderResources(1, static_cast<unsigned int>(GBufferTextureType::Count), &myShaderResources[0]);
	}
	else
	{
		aContext->PSSetShaderResources(1, static_cast<unsigned int>(GBufferTextureType::Count), &myShaderResources[0]);
	}
}

void GBuffer::Release()
{
	if (myContext != nullptr)
	{
		for (int index = 0; index < static_cast<int>(GBufferTextureType::Count); index++)
		{
			myTextures[index]->Release();
			myRenderTargets[index]->Release();
			myShaderResources[index]->Release();
		}
		myContext = nullptr;
		delete myViewport;
		myViewport = nullptr;
	}
}
