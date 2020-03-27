#include "stdafx.h"
#include "TextRenderer.h"
#include "Text.h"
#include "DXFramework.h"
#include <CameraExtended.hpp>
#include <fstream>
#include <d3d11.h>

TextRenderer::TextRenderer() : myStride(sizeof(FontVertexData)), myOffset(0)
{
	myContext = nullptr;
	myDevice = nullptr;
	myFrameBuffer = nullptr;
	myObjectBuffer = nullptr;
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myInputLayout = nullptr;
}
TextRenderer::~TextRenderer()
{
}

bool TextRenderer::Init(DXFramework* aFramework)
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
	assert(!FAILED(result) && "TextRenderer failed to create FrameDataBuffer");
	if (FAILED(result))
	{
		return false;
	}
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	if (myObjectBuffer != nullptr)
	{
		myObjectBuffer->Release();
	}
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
	assert(!FAILED(result) && "TextRenderer failed to create ObjectDataBuffer");
	if (FAILED(result))
	{
		return false;
	}


	std::ifstream vsFile;
	vsFile.open("Shaders\\FontVertex.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	vsFile.close();
	result = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
	assert(!FAILED(result) && "TextRenderer failed to create VertexShader");
	if (FAILED(result))
	{
		return false;
	}

	std::ifstream psFile;
	psFile.open("Shaders\\FontPixel.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	psFile.close();
	result = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
	assert(!FAILED(result) && "TextRenderer failed to create PixelShader");
	if (FAILED(result))
	{
		return false;
	}


	CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> layout(4);
	layout.Add({ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	//layout.Add({ "COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	layout.Add({ "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 });
	result = myDevice->CreateInputLayout(layout.GetPointer(), layout.Size(), vsData.data(), vsData.size(), &myInputLayout);
	assert(!FAILED(result) && "TextRenderer failed to create layout");
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextRenderer::Render(const CU::GrowingArray<TextInstance>& aTextList, const CU::Vector2f& aScreenSize)
{
	if (aTextList.Size() == 0)
	{
		return;
	}

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myScreenSize = aScreenSize;
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	assert(!FAILED(result) && "TextRenderer failed to map FrameBufferData");
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer, 0);
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	Text* text = nullptr;
	for (unsigned short index = 0; index < aTextList.Size(); index++)
	{
		const TextInstance& command = aTextList[index];
		text = command.myTextObject;
		if (text == nullptr)
		{
			continue;
		}
		const TextData& textData = text->myData;

		myObjectBufferData.myColor = command.GetColor();
		myObjectBufferData.myPosition = CU::Vector4f(command.GetPosition(), 1.0f);
		myObjectBufferData.mySize = textData.mySize;
		myObjectBufferData.myScale = command.GetScale();
		myObjectBufferData.myPivot = command.GetPivot();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		assert(!FAILED(result) && "TextRenderer failed to map ObjectBufferData");
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &myObjectBufferData, sizeof(ObjectBufferData));
		myContext->Unmap(myObjectBuffer, 0);
		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myContext->IASetInputLayout(myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &textData.myVertexBuffer, &myStride, &myOffset);
		myContext->IASetIndexBuffer(textData.myIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

		myContext->VSSetShader(myVertexShader, nullptr, 0);

		myContext->PSSetShaderResources(0, 1, text->myFontObject.GetTexture());
		myContext->PSSetShader(myPixelShader, nullptr, 0);

		myContext->DrawIndexed(command.myTextObject->myData.myVertexCount, 0, 0);
	}
}
