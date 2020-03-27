#include "stdafx.h"
#include "ParticleRenderer.h"
#include "DXFramework.h"
#include <CameraExtended.hpp>
#include <d3d11.h>

ParticleRenderer::ParticleRenderer()
{
}
ParticleRenderer::~ParticleRenderer()
{
}

bool ParticleRenderer::Init(DXFramework* aFramework)
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

	ID3D11Device* device = aFramework->GetDevice();
	if (device == nullptr)
	{
		return false;
	}

	HRESULT result;

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	result = device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	if (FAILED(result))
	{
		return false;
	}
	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthDescription = { 0 };
	depthDescription.DepthEnable = true;
	depthDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDescription.DepthFunc = D3D11_COMPARISON_GREATER;
	result = device->CreateDepthStencilState(&depthDescription, nullptr);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void ParticleRenderer::Render(CU::Camera* aCamera, const CU::GrowingArray<ParticleEmitterInstance*>& aParticleList)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myToCameraMatrix = aCamera->GetViewMatrix();
	myFrameBufferData.myToProjectionMatrix = aCamera->GetProjectionMatrix();
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer, 0);
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	ParticleEmitterInstance* instance = nullptr;
	ParticleEmitter* emitter = nullptr;
	for (unsigned short index = 0; index < aParticleList.Size(); index++)
	{
		instance = aParticleList[index];
		emitter = instance->GetEmitter();

		myObjectBufferData.myToWorldMatrix = CU::Matrix4x4f();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &myObjectBufferData, sizeof(ObjectBufferData));
		myContext->Unmap(myObjectBuffer, 0);

		ParticleEmitterData emitData = emitter->GetData();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(emitData.myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, instance->GetParticleList().GetPointer(), sizeof(ParticleData) * instance->GetParticleList().Size());
		myContext->Unmap(emitData.myVertexBuffer, 0);

		myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(emitData.myTopology));
		myContext->IASetInputLayout(emitData.myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &emitData.myVertexBuffer, &emitData.myStride, &emitData.myOffset);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(emitData.myVertexShader, nullptr, 0);

		myContext->GSSetShader(emitData.myGeometryShader, nullptr, 0);

		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetShaderResources(0, 1, emitData.myTexture.GetTexture()->GetSRV());
		myContext->PSSetShader(emitData.myPixelShader, nullptr, 0);

		myContext->Draw(instance->GetParticleList().Size(), 0);
	}
	myContext->GSSetShader(nullptr, nullptr, 0);
}

void ParticleRenderer::Render(CU::Camera* aCamera, const CU::GrowingArray<ParticleEmitterRenderCommand>& aParticleList)
{
	if (aCamera == nullptr || aParticleList.Size() == 0)
	{
		return;
	}

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myToCameraMatrix = aCamera->GetViewMatrix();
	myFrameBufferData.myToProjectionMatrix = aCamera->GetProjectionMatrix();
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer, 0);
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	for (unsigned short index = 0; index < aParticleList.Size(); index++)
	{
		const ParticleEmitterRenderCommand& command = aParticleList[index];

		/*myObjectBufferData.myToWorldMatrix = CU::Matrix4x4f();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &myObjectBufferData, sizeof(ObjectBufferData));
		myContext->Unmap(myObjectBuffer, 0);*/

		ParticleEmitterData& emitData = command.myEmitter->GetData();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(emitData.myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, command.myParticleList.GetPointer(), sizeof(ParticleData) * command.myParticleList.Size());
		myContext->Unmap(emitData.myVertexBuffer, 0);

		myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(emitData.myTopology));
		myContext->IASetInputLayout(emitData.myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &emitData.myVertexBuffer, &emitData.myStride, &emitData.myOffset);

		//myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(emitData.myVertexShader, nullptr, 0);

		myContext->GSSetShader(emitData.myGeometryShader, nullptr, 0);

		//myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetShaderResources(0, 1, emitData.myTexture.GetTexture()->GetSRV());
		myContext->PSSetShader(emitData.myPixelShader, nullptr, 0);

		myContext->Draw(command.myParticleList.Size(), 0);
	}
	myContext->GSSetShader(nullptr, nullptr, 0);
}