#pragma once
#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"
#include <HashMap.hpp>
#include <string>

struct ID3D11Device;
struct D3D11_INPUT_ELEMENT_DESC;

class ShaderLoader
{
public:
	static void CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxShaderAmount);
	static ShaderLoader* GetInstance();
	static void DestroyInstance();

	//Can only load uncompiled hlsl vertex shaders
	VertexShader* GetVertexShader(const std::string& aPath);
	//Loads compiled vertex shader, returns nullptr if already loaded
	VertexShader* LoadVertexShader(const std::string& aPath, D3D11_INPUT_ELEMENT_DESC* aLayoutElementArray = nullptr, const unsigned int aElementCount = 0);
	GeometryShader* GetGeometryShader(const std::string& aPath);
	PixelShader* GetPixelShader(const std::string& aPath);
	void UnloadVertexShader(const std::string& aPath);
	void UnloadGeometryShader(const std::string& aPath);
	void UnloadPixelShader(const std::string& aPath);

private:
	ShaderLoader(ID3D11Device* aDevice, const unsigned int aMaxShaderAmount);
	~ShaderLoader();

private:
	static ShaderLoader* ourInstance;

	CU::HashMap<std::string, VertexShader> myVertexShaderMap;
	CU::HashMap<std::string, GeometryShader> myGeometryShaderMap;
	CU::HashMap<std::string, PixelShader> myPixelShaderMap;
	ID3D11Device* myDevice;
};