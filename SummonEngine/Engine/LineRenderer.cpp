#include "stdafx.h"
#include "LineRenderer.h"
#include "DXFramework.h"
#include "ShaderLoader.h"
#include <Camera.hpp>
#include <fstream>
#include <d3d11.h>

LineRenderer::LineRenderer()
{
	myDevice = nullptr;
	myContext = nullptr;
	myFrameBuffer = nullptr;
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myInputLayout = nullptr;
}
LineRenderer::~LineRenderer()
{
}

const bool LineRenderer::Init(DXFramework* aFramework)
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
	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	assert(!FAILED(result) && "Line renderer failed to create FrameBuffer");
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
	vertexBufferDescription.ByteWidth = sizeof(LineRenderCommand) * 2000;
	vertexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	result = myDevice->CreateBuffer(&vertexBufferDescription, nullptr, &myVertexBuffer);
	assert(!FAILED(result) && "Line renderer failed to create VertexBuffer");
	if (FAILED(result))
	{
		return false;
	}

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;
	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\LineVertex.cso");
	if (vertexShader == nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC layout = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\LineVertex.cso", &layout, 1);
	}
	myVertexShader = vertexShader->GetShader();
	myInputLayout = vertexShader->GetLayout();

	pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\LinePixel.cso");
	myPixelShader = pixelShader->GetShader();

	return true;
}

void LineRenderer::Render(CU::Camera* aCamera, const CU::GrowingArray<LineRenderCommand>& aLineList)
{
	if (aCamera == nullptr || aLineList.Size() == 0)
	{
		return;
	}

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myToCameraMatrix = aCamera->GetViewMatrix();
	myFrameBufferData.myToProjectionMatrix = aCamera->GetProjectionMatrix();
	myFrameBufferData.myCameraPosition = CU::Vector4f(aCamera->GetPosition(), 1.0f);
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer, 0);
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedVertexResource;
	result = myContext->Map(myVertexBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource);
	if (FAILED(result))
	{
		return;
	}
	memcpy(mappedVertexResource.pData, aLineList.GetPointer(), aLineList.Size() * sizeof(LineRenderCommand));
	myContext->Unmap(myVertexBuffer, 0);

	const unsigned int stride = sizeof(CU::Vector4f);
	const unsigned int offset = 0;
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	myContext->IASetInputLayout(myInputLayout);
	myContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	myContext->VSSetShader(myVertexShader, nullptr, 0);
	myContext->PSSetShader(myPixelShader, nullptr, 0);
	myContext->Draw(aLineList.Size() * 2, 0);
}
