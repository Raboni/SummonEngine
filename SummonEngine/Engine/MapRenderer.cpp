#include "stdafx.h"
#include "MapRenderer.h"
#include "DXFramework.h"
#include "Sprite.h"
#include "FullscreenTexture.h"
#include <Camera.hpp>
#include <fstream>
#include <string>
#include <d3d11.h>

MapRenderer::MapRenderer()
{
	myContext = nullptr;
	myDevice = nullptr;
	myVertexBuffer = nullptr;
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myInputLayout = nullptr;
	myMapTexture = nullptr;
}
MapRenderer::~MapRenderer()
{
}

bool MapRenderer::Init(DXFramework* aFramework)
{
	if (aFramework == nullptr)
	{
		return false;
	}
	myContext = aFramework->GetDeviceContext();
	assert(myContext != nullptr && "MapRenderer failed to get DeviceContext");
	if (myContext == nullptr)
	{
		return false;
	}
	myDevice = aFramework->GetDevice();
	assert(myDevice != nullptr && "MapRenderer failed to get Device");
	if (myDevice == nullptr)
	{
		return false;
	}

	HRESULT result;
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.ByteWidth = sizeof(MapRenderCommand) - sizeof(Sprite*);
	if (myVertexBuffer != nullptr)
	{
		myVertexBuffer->Release();
	}
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myVertexBuffer);
	assert(!FAILED(result) && "MapRenderer failed to create VertexBuffer");
	if (FAILED(result))
	{
		return false;
	}

	std::ifstream file;
	std::string data;

	file.open("Shaders\\MapGeometry.cso", std::ios::binary);
	data = { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	result = myDevice->CreateGeometryShader(data.data(), data.size(), nullptr, &myGeometryShader);
	assert(!FAILED(result) && "MapRenderer failed to create GeometryShader");
	if (FAILED(result))
	{
		return false;
	}
	file.close();

	file.open("Shaders\\MapPixel.cso", std::ios::binary);
	data = { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	result = myDevice->CreatePixelShader(data.data(), data.size(), nullptr, &myPixelShader);
	assert(!FAILED(result) && "MapRenderer failed to create PixelShader");
	if (FAILED(result))
	{
		return false;
	}
	file.close();

	file.open("Shaders\\MapVertex.cso", std::ios::binary);
	data = { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	result = myDevice->CreateVertexShader(data.data(), data.size(), nullptr, &myVertexShader);
	assert(!FAILED(result) && "MapRenderer failed to create VertexShader");
	if (FAILED(result))
	{
		return false;
	}
	file.close();

	CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> layout(8);
	layout.Add({ "POSITION_SCREEN", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "SIZE_SCREEN", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "POSITION_WORLD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "SIZE_WORLD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "FOCUS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "ROTATION", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "ZOOM", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	result = myDevice->CreateInputLayout(layout.GetPointer(), layout.Size(), data.data(), data.size(), &myInputLayout);
	assert(!FAILED(result) && "MapRenderer failed to create InputLayout");
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void MapRenderer::Render(const MapRenderCommand& aCommand)
{
	if (aCommand.mySprite == nullptr && myMapTexture == nullptr)
	{
		return;
	}
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myVertexBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	assert(!FAILED(result) && "MapRenderer failed to map vertex buffer");
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &aCommand.myPositionScreen, sizeof(MapRenderCommand) - sizeof(Sprite*));
	myContext->Unmap(myVertexBuffer, 0);

	unsigned int stride = sizeof(MapRenderCommand) - sizeof(Sprite*);
	unsigned int offset = 0;
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	myContext->IASetInputLayout(myInputLayout);
	myContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	myContext->VSSetConstantBuffers(0, 1, &myVertexBuffer);
	myContext->VSSetShader(myVertexShader, nullptr, 0);
	myContext->GSSetShader(myGeometryShader, nullptr, 0);
	if (myMapTexture != nullptr)
	{
		myMapTexture->SetAsResourceOnSlot(0);
	}
	else
	{
		myContext->PSSetShaderResources(0, 1, aCommand.mySprite->GetSRV());
	}
	myContext->PSSetShader(myPixelShader, nullptr, 0);
	myContext->Draw(1, 0);

	myContext->GSSetShader(nullptr, nullptr, 0);
}

void MapRenderer::SetMapTexture(FullscreenTexture* aMapTexture)
{
	myMapTexture = aMapTexture;
}