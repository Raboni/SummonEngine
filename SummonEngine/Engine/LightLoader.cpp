#include "stdafx.h"
#include "LightLoader.h"

LightLoader* LightLoader::ourInstance = nullptr;
void LightLoader::CreateInstance(ID3D11Device* aDevice, unsigned int aMaxLightAmount)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new LightLoader(aDevice, aMaxLightAmount);
	}
}
LightLoader* LightLoader::GetInstance()
{
	return ourInstance;
}
void LightLoader::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

EnviromentLight* LightLoader::CreateEnviromentLight(const std::string& aCubeMapPath)
{
	myEnviromentLight = new EnviromentLight(aCubeMapPath, myDevice);
	return myEnviromentLight;
}
EnviromentLight* LightLoader::GetEnviromentLight()
{
	return myEnviromentLight;
}

PointLight* LightLoader::CreatePointLight(const std::string& aAlias, const CU::Vector3f& aColor, float aRange)
{
	if (!myPointLightMap.Create(aAlias))
	{
		return nullptr;
	}
	PointLight* light = myPointLightMap.Get(aAlias);
	light->myColor = aColor;
	light->myRange = aRange;
	return light;
}
PointLight* LightLoader::GetPointLight(const std::string& aAlias)
{
	return myPointLightMap.Get(aAlias);
}

LightLoader::LightLoader(ID3D11Device* aDevice, unsigned int aMaxLightAmount) : myPointLightMap(aMaxLightAmount)
{
	myDevice = aDevice;
}
LightLoader::~LightLoader()
{
	delete myEnviromentLight;
}
