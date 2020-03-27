#include "stdafx.h"
#include "PointLightInstance.h"
#include "LightLoader.h"

PointLightInstance::PointLightInstance()
{
	myLight = nullptr;
	myPath = "";
}
PointLightInstance::PointLightInstance(const std::string& aPath)
{
	myLight = nullptr;
	myPosition = CU::Vector3f();
	Init(aPath);
}
PointLightInstance::PointLightInstance(const PointLightInstance& aInstance)
{
	myPath = aInstance.myPath;
	myPosition = aInstance.myPosition;
	myLight = aInstance.myLight;
	//if (myLight != nullptr)
	//{
	//	myLight->AddUser();
	//}
}
PointLightInstance& PointLightInstance::operator=(const PointLightInstance& aInstance)
{
	//Release();
	myPosition = aInstance.myPosition;
	myPath = aInstance.myPath;
	myLight = aInstance.myLight;
	//if (myLight != nullptr)
	//{
	//	myLight->AddUser();
	//}
	return *this;
}
PointLightInstance::~PointLightInstance()
{
	//Release();
}

void PointLightInstance::Init(const std::string& aPath)
{
	//Release();
	myPath = aPath;
	myLight = LightLoader::GetInstance()->GetPointLight(aPath);
}
/*void PointLightInstance::Release()
{
	if (myLight != nullptr)
	{
		myLight->RemoveUser();
		if (myLight->GetUserCount() <= 0)
		{
			LightLoader::GetInstance()->UnloadLight(myPath);
		}
		myLight = nullptr;
	}
}*/

void PointLightInstance::SetPosition(const CU::Vector3f& aPosition)
{
	myPosition = aPosition;
}

CU::Vector3f PointLightInstance::GetColor() const
{
	return myLight->GetColor();
}

CU::Vector3f PointLightInstance::GetPosition() const
{
	return myPosition;
}

float PointLightInstance::GetRange() const
{
	return myLight->GetRange();
}

PointLight* PointLightInstance::GetLight() const
{
	return myLight;
}
