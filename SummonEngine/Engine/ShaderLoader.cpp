#include "stdafx.h"
#include "ShaderLoader.h"
#include <GrowingArray.hpp>
#include <DebugTools.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ShaderLoader* ShaderLoader::ourInstance = nullptr;

void ShaderLoader::CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxShaderAmount)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new ShaderLoader(aDevice, aMaxShaderAmount);
	}
}
ShaderLoader* ShaderLoader::GetInstance()
{
	return ourInstance;
}
void ShaderLoader::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

VertexShader* ShaderLoader::GetVertexShader(const std::string& aPath)
{
	if (aPath.length() < 1)
	{
		return nullptr;
	}
	VertexShader* shader = myVertexShaderMap.Get(aPath);
	if (shader == nullptr)
	{
		const std::experimental::filesystem::path filePath(aPath);
		const std::string ext = filePath.extension().string();
		if (ext != ".hlsl")
		{
			return nullptr;
		}
		if (!myVertexShaderMap.Create(aPath))
		{
			return nullptr;
		}
		shader = myVertexShaderMap.Get(aPath);
		shader->AddUser();

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		#endif
		HRESULT result;
		ID3D10Blob* blob;
		ID3D10Blob* errorBlob;
		const std::wstring wPath(aPath.begin(), aPath.end());
		result = D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, &blob, &errorBlob);
		if (errorBlob != nullptr)
		{
			CU::Log((char*)errorBlob->GetBufferPointer(), CU::LogType::Error);
			errorBlob->Release();
		}
		assert(!FAILED(result) && "Failed to create vertex shader blob");
		if (FAILED(result))
		{
			CU::Log("Failed to create vertex shader blob for: " + aPath, CU::LogType::Error);
			myVertexShaderMap.Remove(aPath);
			return nullptr;
		}
		result = myDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader->myShader);
		assert(!FAILED(result) && "Failed to create vertex shader");
		if (FAILED(result))
		{
			CU::Log("Failed to create vertex shader for: " + aPath, CU::LogType::Error);
			blob->Release();
			myVertexShaderMap.Remove(aPath);
			return nullptr;
		}
		ID3D11ShaderReflection* reflection = nullptr;
		result = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);
		assert(!FAILED(result) && "Failed to reflect vertex shader");
		if (FAILED(result))
		{
			CU::Log("Failed to reflect vertex shader for: " + aPath, CU::LogType::Error);
			blob->Release();
			shader->myShader->Release();
			myVertexShaderMap.Remove(aPath);
			return nullptr;
		}

		D3D11_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);
		CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC, unsigned int> inputLayoutDesc(shaderDesc.InputParameters);
		for (unsigned int paramIndex = 0; paramIndex < shaderDesc.InputParameters; paramIndex++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			reflection->GetInputParameterDesc(paramIndex, &paramDesc);

			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayoutDesc.Add(elementDesc);
		}
		result = myDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.Size(), blob->GetBufferPointer(), blob->GetBufferSize(), &shader->myLayout);
		assert(!FAILED(result) && "Failed to create input layout");
		if (FAILED(result))
		{
			blob->Release();
			reflection->Release();
			shader->myShader->Release();
			myVertexShaderMap.Remove(aPath);
			return nullptr;
		}
		blob->Release();
		reflection->Release();
		/*std::ifstream shaderFile;
		std::string shaderData;
		shaderFile.open(aPath, std::ios::binary);
		shaderData = { std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>() };
		shaderFile.close();
		result = myDevice->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &shader->myShader);
		assert(!FAILED(result) && "Failed to create vertex shader");
		if (FAILED(result))
		{
			myVertexShaderMap.Remove(aPath);
			return nullptr;
		}

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
		result = myDevice->CreateInputLayout(&layout[0], static_cast<unsigned int>(layout.size()), shaderData.data(), shaderData.size(), &shader->myLayout);
		assert(!FAILED(result) && "Failed to create input layout");
		if (FAILED(result))
		{
			shader->myShader->Release();
			myVertexShaderMap.Remove(aPath);
			return nullptr;
		}*/
	}
	else
	{
		shader->AddUser();
	}
	return shader;
}
VertexShader* ShaderLoader::LoadVertexShader(const std::string& aPath, D3D11_INPUT_ELEMENT_DESC* aLayoutElementArray, const unsigned int aElementCount)
{
	if (aPath.length() < 1)
	{
		return nullptr;
	}
	VertexShader* shader = myVertexShaderMap.Get(aPath);
	if (shader == nullptr)
	{
		if (!myVertexShaderMap.Create(aPath))
		{
			return nullptr;
		}
		shader = myVertexShaderMap.Get(aPath);
		shader->AddUser();

		HRESULT result;
		std::ifstream shaderFile;
		std::string shaderData;
		shaderFile.open(aPath, std::ios::binary);
		shaderData = { std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>() };
		shaderFile.close();
		result = myDevice->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &shader->myShader);
		assert(!FAILED(result) && "Failed to create vertex shader");
		if (FAILED(result))
		{
			myVertexShaderMap.Remove(aPath);
			return nullptr;
		}

		if (aLayoutElementArray != nullptr)
		{
			result = myDevice->CreateInputLayout(aLayoutElementArray, aElementCount, shaderData.data(), shaderData.size(), &shader->myLayout);
			assert(!FAILED(result) && "Failed to create input layout");
			if (FAILED(result))
			{
				shader->myShader->Release();
				myVertexShaderMap.Remove(aPath);
				return false;
			}
		}
	}
	else
	{
		return nullptr;
	}
	return shader;
}
GeometryShader* ShaderLoader::GetGeometryShader(const std::string& aPath)
{
	if (aPath.length() < 1)
	{
		return nullptr;
	}
	GeometryShader* shader = myGeometryShaderMap.Get(aPath);
	if (shader == nullptr)
	{
		if (!myGeometryShaderMap.Create(aPath))
		{
			return nullptr;
		}
		shader = myGeometryShaderMap.Get(aPath);
		shader->AddUser();

		const std::experimental::filesystem::path filePath(aPath);
		const std::string ext = filePath.extension().string();
		if (ext == ".cso")
		{
			HRESULT result;
			std::ifstream shaderFile;
			std::string shaderData;
			shaderFile.open(aPath, std::ios::binary);
			shaderData = { std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>() };
			shaderFile.close();
			result = myDevice->CreateGeometryShader(shaderData.data(), shaderData.size(), nullptr, &shader->myShader);
			assert(!FAILED(result) && "Failed to create geometry shader");
			if (FAILED(result))
			{
				myGeometryShaderMap.Remove(aPath);
				return nullptr;
			}
		}
		else if (ext == ".hlsl")
		{
			UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
			#ifdef _DEBUG
			flags |= D3DCOMPILE_DEBUG;
			flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
			#endif
			HRESULT result;
			ID3D10Blob* blob;
			ID3D10Blob* errorBlob;
			const std::wstring wPath(aPath.begin(), aPath.end());
			result = D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, &blob, &errorBlob);
			if (errorBlob != nullptr)
			{
				CU::Log((char*)errorBlob->GetBufferPointer(), CU::LogType::Error);
				errorBlob->Release();
			}
			assert(!FAILED(result) && "Failed to create geometry shader blob");
			if (FAILED(result))
			{
				CU::Log("Failed to create geometry shader blob for: " + aPath, CU::LogType::Error);
				myGeometryShaderMap.Remove(aPath);
				return nullptr;
			}
			result = myDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader->myShader);
			assert(!FAILED(result) && "Failed to create geometry shader");
			if (FAILED(result))
			{
				CU::Log("Failed to create geometry shader for: " + aPath, CU::LogType::Error);
				blob->Release();
				myGeometryShaderMap.Remove(aPath);
				return nullptr;
			}
		}
	}
	else
	{
		shader->AddUser();
	}
	return shader;
}
PixelShader* ShaderLoader::GetPixelShader(const std::string& aPath)
{
	if (aPath.length() < 1)
	{
		return nullptr;
	}
	PixelShader* shader = myPixelShaderMap.Get(aPath);
	if (shader == nullptr)
	{
		if (!myPixelShaderMap.Create(aPath))
		{
			return nullptr;
		}
		shader = myPixelShaderMap.Get(aPath);
		shader->AddUser();

		const std::experimental::filesystem::path filePath(aPath);
		const std::string ext = filePath.extension().string();
		if (ext == ".cso")
		{
			HRESULT result;
			std::ifstream shaderFile;
			std::string shaderData;
			shaderFile.open(aPath, std::ios::binary);
			shaderData = { std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>() };
			shaderFile.close();
			result = myDevice->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &shader->myShader);
			assert(!FAILED(result) && "Failed to create pixel shader");
			if (FAILED(result))
			{
				myPixelShaderMap.Remove(aPath);
				return nullptr;
			}
		}
		else if (ext == ".hlsl")
		{
			UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
			#ifdef _DEBUG
			flags |= D3DCOMPILE_DEBUG;
			flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
			#endif
			HRESULT result;
			ID3D10Blob* blob;
			ID3D10Blob* errorBlob;
			const std::wstring wPath(aPath.begin(), aPath.end());
			result = D3DCompileFromFile(wPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, &blob, &errorBlob);
			if (errorBlob != nullptr)
			{
				CU::Log((char*)errorBlob->GetBufferPointer(), CU::LogType::Error);
				errorBlob->Release();
			}
			assert(!FAILED(result) && "Failed to create pixel shader blob");
			if (FAILED(result))
			{
				CU::Log("Failed to create pixel shader blob for: " + aPath, CU::LogType::Error);
				myPixelShaderMap.Remove(aPath);
				return nullptr;
			}
			result = myDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader->myShader);
			assert(!FAILED(result) && "Failed to create pixel shader");
			if (FAILED(result))
			{
				CU::Log("Failed to create pixel shader for: " + aPath, CU::LogType::Error);
				blob->Release();
				myPixelShaderMap.Remove(aPath);
				return nullptr;
			}
		}
	}
	else
	{
		shader->AddUser();
	}
	return shader;
}

void ShaderLoader::UnloadVertexShader(const std::string& aPath)
{
	aPath;
}
void ShaderLoader::UnloadGeometryShader(const std::string& aPath)
{
	aPath;
}
void ShaderLoader::UnloadPixelShader(const std::string& aPath)
{
	aPath;
}

ShaderLoader::ShaderLoader(ID3D11Device* aDevice, const unsigned int aMaxShaderAmount)
	: myVertexShaderMap(aMaxShaderAmount), myGeometryShaderMap(aMaxShaderAmount), myPixelShaderMap(aMaxShaderAmount)
{
	myDevice = aDevice;
}
ShaderLoader::~ShaderLoader()
{
	for (CU::HashMap<std::string, VertexShader>::Iterator iterator = myVertexShaderMap.Begin(); iterator != myVertexShaderMap.End(); iterator++)
	{
		iterator.GetValue().myShader->Release();
		if (iterator.GetValue().myLayout != nullptr)
		{
			iterator.GetValue().myLayout->Release();
		}
	}
	for (CU::HashMap<std::string, GeometryShader>::Iterator iterator = myGeometryShaderMap.Begin(); iterator != myGeometryShaderMap.End(); iterator++)
	{
		iterator.GetValue().myShader->Release();
	}
	for (CU::HashMap<std::string, PixelShader>::Iterator iterator = myPixelShaderMap.Begin(); iterator != myPixelShaderMap.End(); iterator++)
	{
		iterator.GetValue().myShader->Release();
	}
}