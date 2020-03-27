#include "stdafx.h"
#include "DeferredRenderer.h"
#include "DXFramework.h"
#include "ShaderLoader.h"
#include "Model.h"
#include "SkinnedModel.h"
#include "ModelRenderCommand.h"
#include "EnviromentLight.h"
#include "PointLight.h"
#include "PointLightRenderCommand.h"
#include "ModelLoader.h"
#include "Animation.h"

#include <SizableArray.h>
#include <Camera.hpp>
#include <fstream>
#include <d3d11.h>

DeferredRenderer::DeferredRenderer(RenderSettings& aSettings) : mySettings(aSettings)
{
	myContext = nullptr;
	myFrameBuffer = nullptr;
	myObjectBuffer = nullptr;
	myInstanceBuffer = nullptr;
	myBoneBuffer = nullptr;
	myFullscreenShader = nullptr;
	myDefaultVertexShader = nullptr;
	myDefaultVertexShaderAnim = nullptr;
	myDefaultVertexShaderCustom = nullptr;
	myDefaultVertexShaderInstanced = nullptr;
	myDefaultVertexShaderInstancedAnim = nullptr;
	myDefaultVertexShaderInstancedCustom = nullptr;
	myDefaultGeometryShaderCustom = nullptr;
	myGBufferShader = nullptr;
	myEnviromentLightShader = nullptr;
	myPointLightShader = nullptr;
	myInstanceMaxCount = 320;
	myCreatedAnimationCount = 0;
	myHasCreatedAnimations = false;
}

bool DeferredRenderer::Init(DXFramework* aFramework)
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
	std::ifstream shaderFile;
	std::string shaderData;

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
	bufferDescription.ByteWidth = sizeof(BoneBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myBoneBuffer);
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
	bufferDescription.ByteWidth = sizeof(PointLightBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myPointLightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.ByteWidth = sizeof(AnimObjectBufferData) * myInstanceMaxCount;
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myInstanceBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDescription;
	textureDescription.Width = 512;
	textureDescription.Height = 512;
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;
	textureDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	textureDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	//textureDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DECODER;
	textureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	textureDescription.MiscFlags = 0;
	result = myDevice->CreateTexture2D(&textureDescription, nullptr, &myAnimationDataTexture);
	if (FAILED(result))
	{
		return false;
	}
	result = myDevice->CreateShaderResourceView(myAnimationDataTexture, nullptr, &myAnimationDataTextureView);
	if (FAILED(result))
	{
		return false;
	}

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\FullscreenVertex.cso");
	if (vertexShader == nullptr)
	{
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\FullscreenVertex.cso");

		if (vertexShader == nullptr)
		{
			CU::Log("Failed to get vertex shader: FullscreenVertex", CU::LogType::Error);
			return false;
		}
	}
	myFullscreenShader = vertexShader->GetShader();

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\PBRVertex.cso");
	if (vertexShader == nullptr)
	{
		std::array<D3D11_INPUT_ELEMENT_DESC, 5> layout;
		layout[0] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[1] = { "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[2] = { "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[3] = { "BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[4] = { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\PBRVertex.cso", &layout[0], static_cast<unsigned int>(layout.size()));

		if (vertexShader == nullptr)
		{
			CU::Log("Failed to get vertex shader: PBRVertex", CU::LogType::Error);
			return false;
		}
	}
	myDefaultVertexShader = vertexShader;
	//myDefaultVertexShader = vertexShader->GetShader();
	//myDefaultInputLayout = vertexShader->GetLayout();

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\PBRVertexAnim.cso");
	if (vertexShader == nullptr)
	{
		std::array<D3D11_INPUT_ELEMENT_DESC, 7> layout;
		layout[0] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[1] = { "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[2] = { "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[3] = { "BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[4] = { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[5] = { "BONEID", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[6] = { "BONEWEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\PBRVertexAnim.cso", &layout[0], static_cast<unsigned int>(layout.size()));

		if (vertexShader == nullptr)
		{
			CU::Log("Failed to get vertex shader: PBRVertexAnim", CU::LogType::Error);
			return false;
		}
	}
	myDefaultVertexShaderAnim = vertexShader;
	//myDefaultVertexShaderAnim = vertexShader->GetShader();
	//myDefaultInputLayoutAnim = vertexShader->GetLayout();

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\CustomMeshVertex.cso");
	if (vertexShader == nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC layout = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\CustomMeshVertex.cso", &layout, 1);

		if (vertexShader == nullptr)
		{
			CU::Log("Failed to get vertex shader: CustomMeshVertex", CU::LogType::Error);
			return false;
		}
	}
	myDefaultVertexShaderCustom = vertexShader;

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\PBRVertexInstanced.cso");
	if (vertexShader == nullptr)
	{
		std::array<D3D11_INPUT_ELEMENT_DESC, 11> layout;
		layout[0] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[1] = { "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[2] = { "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[3] = { "BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[4] = { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[5] = { "MODELMATRIX", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[6] = { "MODELMATRIX", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[7] = { "MODELMATRIX", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[8] = { "MODELMATRIX", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[9] = { "MODELSCALE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[10] = { "MODELCOLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\PBRVertexInstanced.cso", &layout[0], static_cast<unsigned int>(layout.size()));

		if (vertexShader == nullptr)
		{
			CU::Log("Failed to get vertex shader: PBRVertexInstanced", CU::LogType::Error);
			return false;
		}
	}
	myDefaultVertexShaderInstanced = vertexShader;
	//myDefaultVertexShaderInstanced = vertexShader->GetShader();
	//myDefaultInputLayoutInstanced = vertexShader->GetLayout();

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\PBRVertexInstancedAnim.cso");
	if (vertexShader == nullptr)
	{
		std::array<D3D11_INPUT_ELEMENT_DESC, 15> layout;
		layout[0] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[1] = { "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[2] = { "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[3] = { "BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[4] = { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[5] = { "BONEID", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[6] = { "BONEWEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };

		layout[7] = { "MODELMATRIX", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[8] = { "MODELMATRIX", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[9] = { "MODELMATRIX", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[10] = { "MODELMATRIX", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[11] = { "MODELSCALE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[12] = { "MODELCOLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[13] = { "ANIMDATA", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[14] = { "BLEND", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\PBRVertexInstancedAnim.cso", &layout[0], static_cast<unsigned int>(layout.size()));

		if (vertexShader == nullptr)
		{
			CU::Log("Failed to get vertex shader: PBRVertexInstancedAnim", CU::LogType::Error);
			return false;
		}
	}
	myDefaultVertexShaderInstancedAnim = vertexShader;
	//myDefaultVertexShaderInstancedAnim = vertexShader->GetShader();
	//myDefaultInputLayoutInstancedAnim = vertexShader->GetLayout();

	vertexShader = ShaderLoader::GetInstance()->GetVertexShader("Shaders\\CustomMeshVertexInstanced.cso");
	if (vertexShader == nullptr)
	{
		std::array<D3D11_INPUT_ELEMENT_DESC, 7> layout;
		layout[0] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
		layout[1] = { "MODELMATRIX", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[2] = { "MODELMATRIX", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[3] = { "MODELMATRIX", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[4] = { "MODELMATRIX", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[5] = { "MODELSCALE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		layout[6] = { "MODELCOLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		vertexShader = ShaderLoader::GetInstance()->LoadVertexShader("Shaders\\CustomMeshVertexInstanced.cso", &layout[0], static_cast<unsigned int>(layout.size()));

		if (vertexShader == nullptr)
		{
			CU::Log("Failed to get vertex shader: CustomMeshVertexInstanced", CU::LogType::Error);
			return false;
		}
	}
	myDefaultVertexShaderInstancedCustom = vertexShader;

	myDefaultGeometryShaderCustom = ShaderLoader::GetInstance()->GetGeometryShader("Shaders\\CustomMeshGeometry.cso");
	if (myDefaultGeometryShaderCustom == nullptr)
	{
		CU::Log("Failed to get geometry shader: CustomMeshGeometry", CU::LogType::Error);
		return false;
	}

	pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\GBufferPixel.cso");
	if (pixelShader == nullptr)
	{
		CU::Log("Failed to get pixel shader: GBufferPixel", CU::LogType::Error);
		return false;
	}
	myGBufferShader = pixelShader->GetShader();

	pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\PBRLightEnviroment.cso");
	if (pixelShader == nullptr)
	{
		CU::Log("Failed to get pixel shader: PBRLightEnviroment", CU::LogType::Error);
		return false;
	}
	myEnviromentLightShader = pixelShader->GetShader();

	pixelShader = ShaderLoader::GetInstance()->GetPixelShader("Shaders\\PBRLightPoint.cso");
	if (pixelShader == nullptr)
	{
		CU::Log("Failed to get pixel shader: PBRLightPoint", CU::LogType::Error);
		return false;
	}
	myPointLightShader = pixelShader->GetShader();

	return true;
}

void DeferredRenderer::GenerateGBuffer(const CU::Camera* aCamera, const CU::GrowingArray<ModelRenderCommand>& aModelList)
{
	if (aCamera == nullptr || aModelList.Size() == 0)
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

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	BoneBufferData boneBufferData;
	TextureInstance defaultAlbedo("Models\\Default\\albedo.dds");
	Model* model = nullptr;
	TextureInstance* textureArray = nullptr;
	for (int modelIndex = 0; modelIndex < aModelList.Size(); modelIndex++)
	{
		model = aModelList[modelIndex].myModel;
		if (model == nullptr)
		{
			continue;
		}
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

		const bool isSkinned = model->IsSkinned();
		const bool isCustom = model->IsCustom();
		if (isSkinned)
		{
			const SkinnedModel* skinnedModel = static_cast<SkinnedModel*>(model);
			const CU::GrowingArray<CU::Matrix4x4f> bones(skinnedModel->GetBoneTransforms(aModelList[modelIndex].myAnimationIndex, aModelList[modelIndex].myAnimationTime));
			int boneCount = bones.Size();
			if (boneCount > 100)
			{
				boneCount = 100;
			}
			for (int boneIndex = 0; boneIndex < boneCount; boneIndex++)
			{
				boneBufferData.myBoneTransforms[boneIndex] = bones[boneIndex];
			}
			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = myContext->Map(myBoneBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				return;
			}
			memcpy(bufferData.pData, &boneBufferData, sizeof(BoneBufferData));
			myContext->Unmap(myBoneBuffer, 0);
			myContext->VSSetConstantBuffers(4, 1, &myBoneBuffer);
			myContext->PSSetConstantBuffers(4, 1, &myBoneBuffer);
		}

		VertexShader* vertexShader = model->GetVertexShader();
		if (vertexShader == nullptr)
		{
			if (isSkinned)
			{
				vertexShader = myDefaultVertexShaderAnim;
			}
			else if (isCustom)
			{
				vertexShader = myDefaultVertexShaderCustom;
			}
			else
			{
				vertexShader = myDefaultVertexShader;
			}
		}

		GeometryShader* geometryShader = model->GetGeometryShader();
		if (geometryShader == nullptr && isCustom)
		{
			geometryShader = myDefaultGeometryShaderCustom;
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
		if (aModelList[modelIndex].myUseAlbedo)
		{
			myContext->PSSetShaderResources(1, 1, textureArray[0].GetTexture()->GetSRV());
		}
		else
		{
			myContext->PSSetShaderResources(1, 1, defaultAlbedo.GetTexture()->GetSRV());
		}
		//myContext->PSSetShaderResources(1, 1, textureArray[0].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(2, 1, textureArray[1].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(3, 1, textureArray[2].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(4, 1, textureArray[3].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(5, 1, textureArray[4].GetTexture()->GetSRV());
		myContext->PSSetShaderResources(6, 1, textureArray[5].GetTexture()->GetSRV());
		myContext->PSSetShader(myGBufferShader, nullptr, 0);

		for (unsigned int meshIndex = 0; meshIndex < model->GetMeshCount(); meshIndex++)
		{
			const MeshData& mesh = model->GetMeshArray()[meshIndex];

			myContext->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
			myContext->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

			myContext->DrawIndexed(mesh.myIndexAmount, 0, 0);
		}
	}

	/*if (mySettings.UseDeferredContexts)
	{
		myContext->FinishCommandList(false, &myCommandList);
	}*/
}

void DeferredRenderer::GenerateGBufferInstanced(const CU::Camera* aCamera, const CU::GrowingArray<ModelRenderCommand>& aModelList, const CU::GrowingArray<int>& aModelStartIndexList)
{
	if (aCamera == nullptr || aModelList.Size() == 0 || aModelStartIndexList.Size() == 0)
	{
		return;
	}

	if (!myHasCreatedAnimations || ModelLoader::GetInstance()->GetAnimations().Size() != myCreatedAnimationCount)
	{
		myHasCreatedAnimations = true;
		CreateAnimationDataTexture();
	}

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myToCameraMatrix = aCamera->GetViewMatrix();
	myFrameBufferData.myToProjectionMatrix = aCamera->GetProjectionMatrix();
	myFrameBufferData.myCameraPosition = CU::Vector4f(aCamera->GetPosition(), 1.0f);
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	assert(!FAILED(result) && "Failed to map frame buffer");
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer, 0);
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	BoneBufferData boneBufferData;
	CU::GrowingArray<AnimObjectBufferData> instanceDataList(aModelList.Size());
	unsigned int strides[2] = { 0, sizeof(AnimObjectBufferData) };
	unsigned int offsets[2] = { 0, 0 };
	ID3D11Buffer* bufferPointers[2];
	Model* model = nullptr;
	TextureInstance* textureArray = nullptr;
	unsigned int instanceCount = 0;
	int modelStartIndex = -1;
	for (int modelIndex = 0; modelIndex < aModelStartIndexList.Size(); modelIndex++)
	{
		if (modelIndex == aModelStartIndexList.Size() - 1)
		{
			instanceCount = aModelList.Size() - aModelStartIndexList[modelIndex];
		}
		else
		{
			instanceCount = aModelStartIndexList[modelIndex + 1] - aModelStartIndexList[modelIndex];
		}
		modelStartIndex = aModelStartIndexList[modelIndex];
		model = aModelList[modelStartIndex].myModel;
		if (model == nullptr)
		{
			continue;
		}
		textureArray = model->GetTextureArray();

		const bool isSkinned = model->IsSkinned();
		const bool isCustom = model->IsCustom();
		if (isSkinned)
		{
			const SkinnedModel* skinnedModel = static_cast<SkinnedModel*>(model);
			instanceDataList.Resize(instanceCount);
			for (int instanceIndex = 0; instanceIndex < static_cast<int>(instanceCount); instanceIndex++)
			{
				const int currentInstanceModelIndex = instanceIndex + aModelStartIndexList[modelIndex];
				memcpy(&instanceDataList[instanceIndex].myToWorldMatrix, &aModelList[currentInstanceModelIndex].myTransform, sizeof(CU::Matrix4x4f));
				instanceDataList[instanceIndex].myScale = CU::Vector4f(aModelList[currentInstanceModelIndex].myScale, 1.0f);
				instanceDataList[instanceIndex].myColor = aModelList[currentInstanceModelIndex].myColor;

				const int animationIndex = aModelList[currentInstanceModelIndex].myAnimationIndex;
				const float animationTime = aModelList[currentInstanceModelIndex].myAnimationTime;
				instanceDataList[instanceIndex].myAnimationOffset = skinnedModel->GetAnimationOffset(animationIndex);
				instanceDataList[instanceIndex].myAnimationDeltaOffset = skinnedModel->GetAnimationDeltaOffset(animationIndex);
				instanceDataList[instanceIndex].myFrameOffset = skinnedModel->GetFrameOffset(animationIndex, animationTime, instanceDataList[instanceIndex].myBlendPercent);
			}

			myContext->VSSetShaderResources(8, 1, &myAnimationDataTextureView);
		}
		else
		{
			instanceDataList.Resize(instanceCount);
			for (int instanceIndex = 0; instanceIndex < static_cast<int>(instanceCount); instanceIndex++)
			{
				const int currentInstanceModelIndex = instanceIndex + aModelStartIndexList[modelIndex];
				memcpy(&instanceDataList[instanceIndex].myToWorldMatrix, &aModelList[currentInstanceModelIndex].myTransform, sizeof(CU::Matrix4x4f));
				instanceDataList[instanceIndex].myScale = CU::Vector4f(aModelList[currentInstanceModelIndex].myScale, 1.0f);
				instanceDataList[instanceIndex].myColor = aModelList[currentInstanceModelIndex].myColor;
			}
		}

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myInstanceBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		assert(!FAILED(result) && "Failed to map instance buffer");
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, instanceDataList.GetPointer(), sizeof(AnimObjectBufferData) * instanceDataList.Size());
		myContext->Unmap(myInstanceBuffer, 0);

		VertexShader* vertexShader = model->GetVertexShader();
		if (vertexShader == nullptr)
		{
			if (isSkinned)
			{
				vertexShader = myDefaultVertexShaderInstancedAnim;
			}
			else if (isCustom)
			{
				vertexShader = myDefaultVertexShaderInstancedCustom;
			}
			else
			{
				vertexShader = myDefaultVertexShaderInstanced;
			}
		}

		GeometryShader* geometryShader = model->GetGeometryShader();
		if (geometryShader == nullptr && isCustom)
		{
			geometryShader = myDefaultGeometryShaderCustom;
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
		myContext->PSSetShader(myGBufferShader, nullptr, 0);

		for (unsigned int meshIndex = 0; meshIndex < model->GetMeshCount(); meshIndex++)
		{
			const MeshData& mesh = model->GetMeshArray()[meshIndex];

			strides[0] = mesh.myStride;
			offsets[0] = mesh.myOffset;
			bufferPointers[0] = mesh.myVertexBuffer;
			bufferPointers[1] = myInstanceBuffer;

			myContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
			myContext->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

			myContext->DrawIndexedInstanced(mesh.myIndexAmount, instanceCount, 0, 0, 0);
		}
	}

	/*if (mySettings.UseDeferredContexts)
	{
		myContext->FinishCommandList(false, &myCommandListInstanced);
	}*/
}

void DeferredRenderer::RenderEnvLight(const CU::Camera* aCamera, const EnviromentLight* aEnviromentLight)
{
	if (aCamera == nullptr || aEnviromentLight == nullptr)
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
	lightBufferData.myDirectionalLightDirection = CU::Vector4f(aEnviromentLight->GetDirection(), 0.0f);
	lightBufferData.myDirectionalLightColor = CU::Vector4f(aEnviromentLight->GetColor(), 1.0f);
	lightBufferData.myEnviromentLightMipCount = aEnviromentLight->GetMipLevels();
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = myContext->Map(myEnviromentLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		return;
	}
	memcpy(bufferData.pData, &lightBufferData, sizeof(EnviromentLightBufferData));
	myContext->Unmap(myEnviromentLightBuffer, 0);
	myContext->VSSetConstantBuffers(2, 1, &myEnviromentLightBuffer);
	myContext->PSSetConstantBuffers(2, 1, &myEnviromentLightBuffer);

	myContext->PSSetShaderResources(0, 1, aEnviromentLight->GetCubemap());

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);

	myContext->GSSetShader(nullptr, nullptr, 0);

	myContext->VSSetShader(myFullscreenShader, nullptr, 0);
	myContext->PSSetShader(myEnviromentLightShader, nullptr, 0);

	myContext->Draw(3, 0);

	/*if (mySettings.UseDeferredContexts)
	{
		myContext->FinishCommandList(false, &myCommandListEnviromentLight);
	}*/
}

void DeferredRenderer::RenderPointLight(const CU::Camera* aCamera, const CU::GrowingArray<PointLightRenderCommand>& aLightList)
{
	if (aCamera == nullptr || aLightList.Size() == 0)
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

	PointLightBufferData lightBufferData;
	for (int lightIndex = 0; lightIndex < aLightList.Size(); lightIndex++)
	{
		lightBufferData.myPosition = CU::Vector4f(aLightList[lightIndex].myPosition, 1.0f);
		lightBufferData.myColor = aLightList[lightIndex].myLight->GetColor();
		lightBufferData.myRange = aLightList[lightIndex].myLight->GetRange();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myPointLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &lightBufferData, sizeof(PointLightBufferData));
		myContext->Unmap(myPointLightBuffer, 0);
		myContext->VSSetConstantBuffers(3, 1, &myPointLightBuffer);
		myContext->PSSetConstantBuffers(3, 1, &myPointLightBuffer);

		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myContext->IASetInputLayout(nullptr);
		myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);

		myContext->GSSetShader(nullptr, nullptr, 0);

		myContext->VSSetShader(myFullscreenShader, nullptr, 0);
		myContext->PSSetShader(myPointLightShader, nullptr, 0);

		myContext->Draw(3, 0);
	}

	/*if (mySettings.UseDeferredContexts)
	{
		myContext->FinishCommandList(false, &myCommandListPointLight);
	}*/
}

void DeferredRenderer::UpdateInstanceMaxCount(const unsigned int aCurrentModelCount)
{
	if (aCurrentModelCount > myInstanceMaxCount)
	{
		while (aCurrentModelCount > myInstanceMaxCount)
		{
			myInstanceMaxCount *= 2;
		}
		HRESULT result;
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.ByteWidth = sizeof(AnimObjectBufferData) * myInstanceMaxCount;
		myInstanceBuffer->Release();
		result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myInstanceBuffer);
		assert(!FAILED(result) && "Failed to recreate instance buffer");
		if (FAILED(result))
		{
			myInstanceBuffer = nullptr;
			myInstanceMaxCount = 0;
		}
	}
}

void DeferredRenderer::CreateAnimationDataTexture()
{
	const CU::SizableArray<Animation, MaxAnimationCount>& animList = ModelLoader::GetInstance()->GetAnimations();
	if (animList.Size() > 0)
	{
		CU::GrowingArray<CU::Matrix4x4f> bones(512);
		for (int animIndex = 0; animIndex < animList.Size(); animIndex++)
		{
			const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& boneTransforms = animList[animIndex].GetBoneTransformLists();
			for (int tickIndex = 0; tickIndex < boneTransforms.Size(); tickIndex++)
			{
				for (int boneIndex = 0; boneIndex < boneTransforms[tickIndex].Size(); boneIndex++)
				{
					bones.Add(boneTransforms[tickIndex][boneIndex]);
				}
			}
			const CU::GrowingArray<CU::GrowingArray<CU::Matrix4x4f>>& boneDeltas = animList[animIndex].GetBoneDeltaLists();
			for (int tickIndex = 0; tickIndex < boneDeltas.Size(); tickIndex++)
			{
				for (int boneIndex = 0; boneIndex < boneDeltas[tickIndex].Size(); boneIndex++)
				{
					bones.Add(boneDeltas[tickIndex][boneIndex]);
				}
			}
			//since deltas have one less entry
			for (int boneIndex = 0; boneIndex < boneDeltas[0].Size(); boneIndex++)
			{
				bones.Add(CU::Matrix4x4f());
			}
		}

		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE textureData;
		ZeroMemory(&textureData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myAnimationDataTexture, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &textureData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(textureData.pData, bones.GetPointer(), sizeof(CU::Matrix4x4f) * bones.Size());
		myContext->Unmap(myAnimationDataTexture, 0);

		myCreatedAnimationCount = animList.Size();
	}
	else
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE textureData;
		ZeroMemory(&textureData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myAnimationDataTexture, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &textureData);
		if (FAILED(result))
		{
			return;
		}
		memset(textureData.pData, 0, sizeof(CU::Matrix4x4f) * 512);
		myContext->Unmap(myAnimationDataTexture, 0);

		myCreatedAnimationCount = 0;
	}
}