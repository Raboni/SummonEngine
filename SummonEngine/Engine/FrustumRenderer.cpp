#include "stdafx.h"
#include "FrustumRenderer.h"
#include "DXFramework.h"
#include "ShaderLoader.h"
#include <Camera.hpp>
#include <fstream>
#include <d3d11.h>

FrustumRenderer::FrustumRenderer()
{
	myDevice = nullptr;
	myContext = nullptr;
	myFrameBuffer = nullptr;
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myInputLayout = nullptr;
	
	myPointList.Init(8);
	myIndexList.Init(72);

	//inside top
	myIndexList.Add(2);
	myIndexList.Add(6);
	myIndexList.Add(1);
	myIndexList.Add(6);
	myIndexList.Add(5);
	myIndexList.Add(1);

	//inside back
	myIndexList.Add(1);
	myIndexList.Add(5);
	myIndexList.Add(0);
	myIndexList.Add(5);
	myIndexList.Add(4);
	myIndexList.Add(0);

	//inside right
	myIndexList.Add(2);
	myIndexList.Add(1);
	myIndexList.Add(0);
	myIndexList.Add(3);
	myIndexList.Add(2);
	myIndexList.Add(0);

	//inside bottom
	myIndexList.Add(0);
	myIndexList.Add(4);
	myIndexList.Add(3);
	myIndexList.Add(4);
	myIndexList.Add(7);
	myIndexList.Add(3);

	//inside left
	myIndexList.Add(5);
	myIndexList.Add(6);
	myIndexList.Add(4);
	myIndexList.Add(6);
	myIndexList.Add(7);
	myIndexList.Add(4);

	//inside front
	/*myIndexList.Add(7);
	myIndexList.Add(2);
	myIndexList.Add(3);
	myIndexList.Add(7);
	myIndexList.Add(6);
	myIndexList.Add(2);*/

	//outside right
	myIndexList.Add(0);
	myIndexList.Add(1);
	myIndexList.Add(2);
	myIndexList.Add(0);
	myIndexList.Add(2);
	myIndexList.Add(3);

	//outside top
	myIndexList.Add(1);
	myIndexList.Add(6);
	myIndexList.Add(2);
	myIndexList.Add(1);
	myIndexList.Add(5);
	myIndexList.Add(6);

	//outside back
	myIndexList.Add(0);
	myIndexList.Add(5);
	myIndexList.Add(1);
	myIndexList.Add(0);
	myIndexList.Add(4);
	myIndexList.Add(5);

	//outside bottom
	myIndexList.Add(3);
	myIndexList.Add(4);
	myIndexList.Add(0);
	myIndexList.Add(3);
	myIndexList.Add(7);
	myIndexList.Add(4);

	//outside left
	myIndexList.Add(4);
	myIndexList.Add(6);
	myIndexList.Add(5);
	myIndexList.Add(4);
	myIndexList.Add(7);
	myIndexList.Add(6);

	//outside front
	myIndexList.Add(3);
	myIndexList.Add(2);
	myIndexList.Add(7);
	myIndexList.Add(2);
	myIndexList.Add(6);
	myIndexList.Add(7);
}
FrustumRenderer::~FrustumRenderer()
{
}

bool FrustumRenderer::Init(DXFramework* aFramework)
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
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	assert(!FAILED(result) && "Frustum renderer failed to create FrameBuffer");
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
	vertexBufferDescription.ByteWidth = sizeof(CU::Vector4f) * 8;
	vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myDevice->CreateBuffer(&vertexBufferDescription, nullptr, &myVertexBuffer);
	assert(!FAILED(result) && "Frustum renderer failed to create VertexBuffer");
	if (FAILED(result))
	{
		return false;
	}
	D3D11_BUFFER_DESC indexBufferDescription = { 0 };
	indexBufferDescription.ByteWidth = sizeof(unsigned int) * myIndexList.Size();
	indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subresourceData = { 0 };
	subresourceData.pSysMem = myIndexList.GetPointer();
	result = myDevice->CreateBuffer(&indexBufferDescription, &subresourceData, &myIndexBuffer);
	assert(!FAILED(result) && "Frustum renderer failed to create IndexBuffer");
	if (FAILED(result))
	{
		return false;
	}

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;
	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\NavmeshVertex.cso");
	if (vertexShader == nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC layout = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\NavmeshVertex.cso", &layout, 1);
	}
	myVertexShader = vertexShader->GetShader();
	myInputLayout = vertexShader->GetLayout();

	pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\NavmeshPixel.cso");
	myPixelShader = pixelShader->GetShader();

	/*std::ifstream vsFile;
	vsFile.open("Shaders\\NavmeshVertex.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
	assert(!FAILED(result) && "Line renderer failed to create VertexShader");
	if (FAILED(result))
	{
		return false;
	}
	vsFile.close();

	std::ifstream psFile;
	psFile.open("Shaders\\NavmeshPixel.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	result = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
	assert(!FAILED(result) && "Frustum renderer failed to create PixelShader");
	if (FAILED(result))
	{
		return false;
	}
	psFile.close();

	CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> layout(2);
	layout.Add({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	result = myDevice->CreateInputLayout(layout.GetPointer(), layout.Size(), vsData.data(), vsData.size(), &myInputLayout);
	assert(!FAILED(result) && "Frustum renderer failed to create InputLayout");
	if (FAILED(result))
	{
		return false;
	}*/

	return true;
}

void FrustumRenderer::Render(CU::Camera* aViewCamera, CU::Camera* aFrustumCamera, RenderStateManager& aStateManager)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myToCameraMatrix = aViewCamera->GetViewMatrix();
	myFrameBufferData.myToProjectionMatrix = aViewCamera->GetProjectionMatrix();
	myFrameBufferData.myCameraPosition = CU::Vector4f(aViewCamera->GetPosition(), 1.0f);
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer, 0);
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	CU::Matrix4x4f InverseFrustumProjection = aFrustumCamera->GetProjectionMatrix().GetInverse();
	CU::Matrix4x4f FrustumMatrix = aFrustumCamera->GetSpace();
	myPointList.RemoveAll();

	myPointList.Add((CU::Vector4f(1.0f, -1.0f, -1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);
	myPointList.Add((CU::Vector4f(1.0f, 1.0f, -1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);
	myPointList.Add((CU::Vector4f(1.0f, 1.0f, 1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);
	myPointList.Add((CU::Vector4f(1.0f, -1.0f, 1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);
	myPointList.Add((CU::Vector4f(-1.0f, -1.0f, -1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);
	myPointList.Add((CU::Vector4f(-1.0f, 1.0f, -1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);
	myPointList.Add((CU::Vector4f(-1.0f, 1.0f, 1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);
	myPointList.Add((CU::Vector4f(-1.0f, -1.0f, 1.0f, 1.0f) * InverseFrustumProjection) * FrustumMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedVertexResource;
	result = myContext->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource);
	if (FAILED(result))
	{
		return;
	}
	memcpy(mappedVertexResource.pData, myPointList.GetPointer(), sizeof(CU::Vector4f) * myPointList.Size());
	myContext->Unmap(myVertexBuffer, 0);

	unsigned int stride = sizeof(CU::Vector4f);
	unsigned int offset = 0;
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(myInputLayout);
	myContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	myContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	myContext->VSSetShader(myVertexShader, nullptr, 0);
	myContext->PSSetShader(myPixelShader, nullptr, 0);
	aStateManager.SetRasterizerState(RasterizerState::Default);
	myContext->DrawIndexed(myIndexList.Size(), 0, 0);
	aStateManager.SetRasterizerState(RasterizerState::Wireframe);
	myContext->DrawIndexed(myIndexList.Size(), 0, 0);
	aStateManager.SetRasterizerState(RasterizerState::Default);
}
