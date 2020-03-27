#include "stdafx.h"
#include "FullscreenRenderer.h"
#include "DXFramework.h"
#include "EngineProxy.h"
#include "ShaderLoader.h"
#include <fstream>
#include <string>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

FullscreenRenderer::FullscreenRenderer()
{
	std::memset(&myPixelShaders[0], 0, sizeof(ID3D11PixelShader*) * myPixelShaders.size());
	myContext = nullptr;
	myVertexShader = nullptr;
	myFrameBuffer = nullptr;
}
FullscreenRenderer::~FullscreenRenderer()
{
}

const bool FullscreenRenderer::Init(DXFramework* aFramework)
{
	if (aFramework == nullptr)
	{
		return false;
	}
	myContext = aFramework->GetDeviceContext();

	HRESULT result;
	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\FullscreenVertex.cso");
	if (vertexShader == nullptr)
	{
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\FullscreenVertex.cso");
	}
	myVertexShader = vertexShader->GetShader();
	/*std::ifstream vsFile;
	vsFile.open("Shaders\\FullscreenVertex.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = aFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	assert(!FAILED(result) && "FullscreenRenderer vertex shader creation failed");
	if (FAILED(result))
	{
		return false;
	}
	vsFile.close();
	myVertexShader = vertexShader;*/

	std::array<std::string, static_cast<unsigned int>(FullscreenShaderType::Count)> myShaderPaths;
	myShaderPaths[static_cast<unsigned int>(FullscreenShaderType::Copy)] = "Shaders\\FullscreenCopy.cso";
	myShaderPaths[static_cast<unsigned int>(FullscreenShaderType::Luminance)] = "Shaders\\FullscreenLuminance.cso";
	myShaderPaths[static_cast<unsigned int>(FullscreenShaderType::GaussianHorizontal)] = "Shaders\\FullscreenGaussianHorizontal.cso";
	myShaderPaths[static_cast<unsigned int>(FullscreenShaderType::GaussianVertical)] = "Shaders\\FullscreenGaussianVertical.cso";
	myShaderPaths[static_cast<unsigned int>(FullscreenShaderType::Bloom)] = "Shaders\\FullscreenBloom.cso";
	myShaderPaths[static_cast<unsigned int>(FullscreenShaderType::FXAA)] = "Shaders\\FullscreenFXAA.cso";

	for (unsigned int shaderType = 0; shaderType < myShaderPaths.size(); shaderType++)
	{
		pixelShader = ShaderLoader::GetInstance()->GetPixelShader(myShaderPaths[shaderType]);
		myPixelShaders[shaderType] = pixelShader->GetShader();
		/*std::ifstream psFile;
		psFile.open(myShaderPaths[shaderType], std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		ID3D11PixelShader* pixelShader;
		result = aFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
		assert(!FAILED(result) && "FullscreenRenderer pixel shader creation failed");
		if (FAILED(result))
		{
			return false;
		}
		psFile.close();
		myPixelShaders[shaderType] = pixelShader;*/
	}

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.ByteWidth = sizeof(unsigned int) * 4;
	if (myFrameBuffer != nullptr)
	{
		myFrameBuffer->Release();
	}
	result = aFramework->GetDevice()->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	assert(!FAILED(result) && "FullscreenRenderer failed to create FrameDataBuffer");
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void FullscreenRenderer::Render(const FullscreenShaderType aEffect)
{
	if (aEffect == FullscreenShaderType::FXAA)
	{
		CU::Vector2i windowSize = EngineProxy::GetWindowSize();
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &windowSize, sizeof(CU::Vector2i));
		myContext->Unmap(myFrameBuffer, 0);
		myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	}

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);

	myContext->VSSetShader(myVertexShader, nullptr, 0);
	myContext->PSSetShader(myPixelShaders[static_cast<unsigned int>(aEffect)], nullptr, 0);

	myContext->Draw(3, 0);
}
