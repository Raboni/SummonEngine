#include "stdafx.h"
#include "SpriteRenderer.h"
#include "DXFramework.h"
#include <CameraExtended.hpp>
#include <d3d11.h>

SpriteRenderer::SpriteRenderer()
{
	myContext = nullptr;
	myDevice = nullptr;
	myFrameBuffer = nullptr;
	myVertexBuffer = nullptr;
	mySpriteMaxCount = 1;
}
SpriteRenderer::~SpriteRenderer()
{
}

bool SpriteRenderer::Init(DXFramework* aFramework)
{
	if (aFramework == nullptr)
	{
		return false;
	}
	myContext = aFramework->GetDeviceContext();
	if (myContext == nullptr)
	{
		return false;
	}
	myDevice = aFramework->GetDevice();
	if (myDevice == nullptr)
	{
		return false;
	}

	HRESULT result;
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	if (myFrameBuffer != nullptr)
	{
		myFrameBuffer->Release();
	}
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	assert(!FAILED(result) && "SpriteRenderer failed to create FrameDataBuffer");
	if (FAILED(result))
	{
		return false;
	}

	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.ByteWidth = sizeof(SpriteRenderData) * mySpriteMaxCount;
	mySpriteMaxCount = 1;
	if (myVertexBuffer != nullptr)
	{
		myVertexBuffer->Release();
	}
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myVertexBuffer);
	assert(!FAILED(result) && "SpriteRenderer failed to create VertexBuffer");
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SpriteRenderer::Render(const CU::GrowingArray<SpriteRenderCommand>& aSpriteList, const CU::Vector2f& aScreenSize)
{
	if (aSpriteList.Size() == 0)
	{
		return;
	}

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myScreenSize = aScreenSize;
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer, 0);
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	unsigned int currentSpriteMaxCount = 0;
	for (int spriteIndex = 0; spriteIndex < aSpriteList.Size(); spriteIndex++)
	{
		unsigned int spriteCount = static_cast<unsigned int>(aSpriteList[spriteIndex].myData.Size());
		if (spriteCount > currentSpriteMaxCount)
		{
			currentSpriteMaxCount = spriteCount;
		}
	}
	if (currentSpriteMaxCount > mySpriteMaxCount)
	{
		if (mySpriteMaxCount == 0)
		{
			mySpriteMaxCount = 1;
		}
		while (currentSpriteMaxCount > mySpriteMaxCount)
		{
			mySpriteMaxCount *= 2;
		}
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.ByteWidth = sizeof(SpriteRenderData) * mySpriteMaxCount;
		myVertexBuffer->Release();
		result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myVertexBuffer);
		assert(!FAILED(result) && "SpriteRenderer failed to recreate vertex buffer");
		if (FAILED(result))
		{
			myVertexBuffer = nullptr;
			mySpriteMaxCount = 0;
		}
	}

	Sprite* sprite = nullptr;
	for (unsigned short index = 0; index < aSpriteList.Size(); index++)
	{
		const SpriteRenderCommand& command = aSpriteList[index];
		sprite = command.mySprite;

		const SpriteData& spriteData = sprite->GetData();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myVertexBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, command.myData.GetPointer(), sizeof(SpriteRenderData) * command.myData.Size());
		myContext->Unmap(myVertexBuffer, 0);

		myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(spriteData.myTopology));
		myContext->IASetInputLayout(spriteData.myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &spriteData.myStride, &spriteData.myOffset);

		myContext->VSSetShader(spriteData.myVertexShader, nullptr, 0);

		myContext->GSSetShader(spriteData.myGeometryShader, nullptr, 0);

		myContext->PSSetShaderResources(0, 1, sprite->GetSRV());
		myContext->PSSetShader(spriteData.myPixelShader, nullptr, 0);

		myContext->Draw(command.myData.Size(), 0);
	}
	myContext->GSSetShader(nullptr, nullptr, 0);
}
