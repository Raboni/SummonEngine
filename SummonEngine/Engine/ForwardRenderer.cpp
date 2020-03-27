#include "stdafx.h"
#include "ForwardRenderer.h"
#include "DXFramework.h"
#include "ShaderLoader.h"
#include "Model.h"
#include "ModelRenderCommand.h"
#include "PointLight.h"
#include "PointLightRenderCommand.h"
#include "EnviromentLight.h"
#include <Camera.hpp>
#include <DebugTools.h>
#include <d3d11.h>
#include <fstream>

ForwardRenderer::ForwardRenderer(RenderSettings& aSettings) : mySettings(aSettings)
{
	myDevice = nullptr;
	myContext = nullptr;
	myFrameBuffer = nullptr;
	myObjectBuffer = nullptr;
	myEnviromentLightBuffer = nullptr;
	myPointLightBuffer = nullptr;
	myDefaultVertexShader = nullptr;
	myDefaultVertexShaderAnim = nullptr;
	myDefaultVertexShaderCustom = nullptr;
	myDefaultGeometryShaderCustom = nullptr;
	myDefaultPixelShaderCustom = nullptr;
	myDefaultPixelShader = nullptr;
}
ForwardRenderer::~ForwardRenderer()
{
}

bool ForwardRenderer::Init(DXFramework* aFramework)
{
	if (aFramework == nullptr)
	{
		return false;
	}
	myContext = aFramework->GetDeviceContext(mySettings.UseDeferredContexts);
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
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	if (FAILED(result))
	{
		return false;
	}
	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
	if (FAILED(result))
	{
		return false;
	}
	bufferDescription.ByteWidth = sizeof(EnviromentLightBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myEnviromentLightBuffer);
	if (FAILED(result))
	{
		return false;
	}
	bufferDescription.ByteWidth = sizeof(PointLightListBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myPointLightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/*VertexShader* vertexShader = ShaderLoader::GetInstance()->GetVertexShader("..\\Engine\\Shaders\\PBRVertex.hlsl");
	myDefaultVertexShader = vertexShader->GetShader();
	myDefaultInputLayout = vertexShader->GetLayout();
	PixelShader* pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\PBRPixel.cso");
	myDefaultPixelShader = pixelShader->GetShader();*/
	myDefaultVertexShader = ShaderLoader::GetInstance()->GetVertexShader("..\\Engine\\Shaders\\PBRVertex.hlsl");
	myDefaultVertexShaderAnim = ShaderLoader::GetInstance()->GetVertexShader("..\\Engine\\Shaders\\PBRVertexAnim.hlsl");
	myDefaultPixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\PBRPixel.cso");

	myDefaultVertexShaderCustom = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\CustomMeshVertex.cso");
	if (myDefaultVertexShaderCustom == nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC layout = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		myDefaultVertexShaderCustom = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\CustomMeshVertex.cso", &layout, 1);
	}
	//myDefaultGeometryShaderCustom = ShaderLoader::GetInstance()->GetGeometryShader("Shaders\\CustomMeshGeometry.cso");
	myDefaultPixelShaderCustom = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\PBRPixel.cso");

	return true;
}

void ForwardRenderer::Render(const CU::Camera* aCamera, const EnviromentLight* aLight,
	const CU::GrowingArray<ModelRenderCommand>& aModelList, const CU::GrowingArray<CU::GrowingArray<PointLightRenderCommand>>& aLightList)
{
	if (aCamera == nullptr || aLight == nullptr || aModelList.Size() == 0)
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

	EnviromentLightBufferData lightBufferData;
	lightBufferData.myDirectionalLightDirection = CU::Vector4f(aLight->GetDirection(), 0.0f);
	lightBufferData.myDirectionalLightColor = CU::Vector4f(aLight->GetColor(), 1.0f);
	lightBufferData.myEnviromentLightMipCount = aLight->GetMipLevels();
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myEnviromentLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &lightBufferData, sizeof(ObjectBufferData));
	myContext->Unmap(myEnviromentLightBuffer, 0);
	myContext->VSSetConstantBuffers(2, 1, &myEnviromentLightBuffer);
	myContext->PSSetConstantBuffers(2, 1, &myEnviromentLightBuffer);

	myContext->PSSetShaderResources(0, 1, aLight->GetCubemap());
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Model* model = nullptr;
	TextureInstance* textureArray = nullptr;
	for (int modelIndex = 0; modelIndex < aModelList.Size(); modelIndex++)
	{
		model = aModelList[modelIndex].myModel;
		textureArray = model->GetTextureArray();

		myObjectBufferData.myToWorldMatrix = aModelList[modelIndex].myTransform;
		myObjectBufferData.myScale = CU::Vector4f(aModelList[modelIndex].myScale, 1.0f);
		myObjectBufferData.myColor = aModelList[modelIndex].myColor;
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &myObjectBufferData, sizeof(ObjectBufferData));
		myContext->Unmap(myObjectBuffer, 0);
		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

		for (int lightIndex = 0; lightIndex < PointLightListSize && lightIndex < aLightList[modelIndex].Size(); lightIndex++)
		{
			const PointLightRenderCommand& light = aLightList[modelIndex][lightIndex];
			myPointLightBufferData.myPointLights[lightIndex].myPosition = CU::Vector4f(light.myPosition, 1.0f);
			myPointLightBufferData.myPointLights[lightIndex].myColor = light.myLight->GetColor();
			myPointLightBufferData.myPointLights[lightIndex].myRange = light.myLight->GetRange();
		}
		myPointLightBufferData.myCount = aLightList[modelIndex].Size();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myPointLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &myPointLightBufferData, sizeof(PointLightListBufferData));
		myContext->Unmap(myPointLightBuffer, 0);
		myContext->PSSetConstantBuffers(4, 1, &myPointLightBuffer);

		VertexShader* vertexShader = model->GetVertexShader();
		if (vertexShader == nullptr)
		{
			if (model->IsSkinned())
			{
				vertexShader = myDefaultVertexShaderAnim;
			}
			else if (model->IsCustom())
			{
				vertexShader = myDefaultVertexShaderCustom;
			}
			else
			{
				vertexShader = myDefaultVertexShader;
			}
		}
		GeometryShader* geometryShader = nullptr;
		if (model->IsCustom())
		{
			geometryShader = myDefaultGeometryShaderCustom;
		}
		PixelShader* pixelShader = model->GetPixelShader();
		if (pixelShader == nullptr)
		{
			if (model->IsCustom())
			{
				pixelShader = myDefaultPixelShaderCustom;
			}
			else
			{
				pixelShader = myDefaultPixelShader;
			}
		}

		myContext->IASetInputLayout(vertexShader->GetLayout());
		myContext->VSSetShader(vertexShader->GetShader(), nullptr, 0);
		if (geometryShader != nullptr)
		{
			myContext->GSSetShader(geometryShader->GetShader(), nullptr, 0);
		}
		else
		{
			myContext->GSSetShader(nullptr, nullptr, 0);
		}
		myContext->PSSetShaderResources(1, 1, textureArray[0].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(2, 1, textureArray[1].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(3, 1, textureArray[2].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(4, 1, textureArray[3].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(5, 1, textureArray[4].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(6, 1, textureArray[5].GetTexture()->GetSRV());
		myContext->PSSetShader(pixelShader->GetShader(), nullptr, 0);

		for (unsigned int meshIndex = 0; meshIndex < model->GetMeshCount(); meshIndex++)
		{
			const MeshData& mesh = model->GetMeshArray()[meshIndex];
			myContext->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
			myContext->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

			myContext->DrawIndexed(mesh.myIndexAmount, 0, 0);
		}
	}
	myContext->GSSetShader(nullptr, nullptr, 0);

	/*if (mySettings.UseDeferredContexts)
	{
		myContext->FinishCommandList(false, &myCommandList);
	}*/
}

void ForwardRenderer::RenderSkybox(const CU::Camera* aCamera, const EnviromentLight* aLight, const ModelRenderCommand& aSkybox)
{
	if (aCamera == nullptr || aLight == nullptr || aSkybox.myModel == nullptr)
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

	CU::Matrix4x4f skyboxTransform;
	memcpy(&skyboxTransform[12], &myFrameBufferData.myCameraPosition, sizeof(CU::Vector3f));
	myObjectBufferData.myToWorldMatrix = skyboxTransform;
	myObjectBufferData.myScale = CU::Vector4f(aSkybox.myScale, 1.0f);
	myObjectBufferData.myColor = aSkybox.myColor;
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myObjectBufferData, sizeof(ObjectBufferData));
	myContext->Unmap(myObjectBuffer, 0);
	myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
	myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

	myContext->PSSetShaderResources(0, 1, aLight->GetCubemap());

	VertexShader* vertexShader = aSkybox.myModel->GetVertexShader();
	if (vertexShader == nullptr)
	{
		CU::Log("Skybox doesn't have a vetex shader", CU::LogType::Error);
		vertexShader = myDefaultVertexShader;
	}
	PixelShader* pixelShader = aSkybox.myModel->GetPixelShader();
	if (pixelShader == nullptr)
	{
		CU::Log("Skybox doesn't have a pixel shader", CU::LogType::Error);
		pixelShader = myDefaultPixelShader;
	}

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(vertexShader->GetLayout());
	myContext->VSSetShader(vertexShader->GetShader(), nullptr, 0);
	myContext->PSSetShader(pixelShader->GetShader(), nullptr, 0);

	const MeshData& mesh = aSkybox.myModel->GetMeshArray()[0];
	myContext->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
	myContext->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	myContext->DrawIndexed(mesh.myIndexAmount, 0, 0);
}