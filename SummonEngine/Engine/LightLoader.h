#pragma once
#include "EnviromentLight.h"
#include "PointLight.h"
#include <HashMap.hpp>
#include <string>

struct ID3D11Device;

class LightLoader
{
public:
	static void CreateInstance(ID3D11Device* aDevice, unsigned int aMaxLightAmount);
	static LightLoader* GetInstance();
	static void DestroyInstance();

	EnviromentLight* CreateEnviromentLight(const std::string& aCubeMapPath);
	EnviromentLight* GetEnviromentLight();

	PointLight* CreatePointLight(const std::string& aAlias, const CU::Vector3f& aColor, float aRange);
	PointLight* GetPointLight(const std::string& aAlias);

private:
	LightLoader(ID3D11Device* aDevice, unsigned int aMaxLightAmount);
	~LightLoader();

	static LightLoader* ourInstance;

	CU::HashMap<std::string, PointLight> myPointLightMap;
	EnviromentLight* myEnviromentLight;
	ID3D11Device* myDevice;
};