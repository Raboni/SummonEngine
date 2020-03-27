#include "stdafx.h"
#include "NavmeshRenderer.h"
#include "DXFramework.h"
#include "ShaderLoader.h"
#include <Camera.hpp>
#include <fstream>
#include <d3d11.h>

namespace Nav
{
	NavmeshRenderer::NavmeshRenderer()
	{
		myDevice = nullptr;
		myContext = nullptr;
		myFrameBuffer = nullptr;
		myVertexShader = nullptr;
		myPixelShader = nullptr;
		myInputLayout = nullptr;
	}
	NavmeshRenderer::~NavmeshRenderer()
	{
	}

	bool NavmeshRenderer::Init(DXFramework* aFramework)
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

		myTexture.Init("Models\\Default\\albedo.dds");

		HRESULT result;

		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = sizeof(FrameBufferData);
		result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
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
		if (FAILED(result))
		{
			return false;
		}
		vsFile.close();

		std::ifstream psFile;
		psFile.open("Shaders\\NavmeshPixel.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
		if (FAILED(result))
		{
			return false;
		}
		psFile.close();

		CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> layout(2);
		layout.Add({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		result = myDevice->CreateInputLayout(layout.GetPointer(), layout.Size(), vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			return false;
		}*/

		return true;
	}

	void NavmeshRenderer::Render(CU::Camera* aCamera, const CU::GrowingArray<Nav::Navmesh>& aNavmeshList)
	{
		if (aCamera == nullptr || aNavmeshList.Size() == 0)
		{
			return;
		}

		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		myFrameBufferData.myToCameraMatrix = aCamera->GetViewMatrix();
		myFrameBufferData.myToProjectionMatrix = aCamera->GetProjectionMatrix();
		myFrameBufferData.myCameraPosition = CU::Vector4f(aCamera->GetPosition(), 1.0f);
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

		for (int meshIndex = 0; meshIndex < aNavmeshList.Size(); meshIndex++)
		{
			const Nav::Navmesh& navmesh = aNavmeshList[meshIndex];
			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			vertexBufferDescription.ByteWidth = navmesh.myVertexList.Size() * sizeof(Nav::Vertex);
			vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
			vertexSubresourceData.pSysMem = navmesh.myVertexList.GetPointer();

			ID3D11Buffer* vertexBuffer;
			result = myDevice->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			if (FAILED(result))
			{
				return;
			}

			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			indexBufferDescription.ByteWidth = navmesh.myIndexList.Size() * sizeof(unsigned int);
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
			indexSubresourceData.pSysMem = navmesh.myIndexList.GetPointer();

			ID3D11Buffer* indexBuffer;
			result = myDevice->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			if (FAILED(result))
			{
				return;
			}



			unsigned int stride = sizeof(Nav::Vertex);
			unsigned int offset = 0;
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			myContext->IASetInputLayout(myInputLayout);
			myContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			myContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->VSSetShader(myVertexShader, nullptr, 0);
			//myContext->PSSetShaderResources(1, 1, myTexture.GetTexture()->GetSRV());
			myContext->PSSetShader(myPixelShader, nullptr, 0);

			myContext->DrawIndexed(navmesh.myIndexList.Size(), 0, 0);
		}
	}
}