#pragma once
#include "PointLight.h"
#include <Vector3.hpp>
#include <string>

class PointLightInstance
{
public:
	PointLightInstance();
	PointLightInstance(const std::string& aPath);
	PointLightInstance(const PointLightInstance& aInstance);
	PointLightInstance& operator=(const PointLightInstance& aInstance);
	~PointLightInstance();
	void Init(const std::string& aPath);
	//void Release();

	void SetPosition(const CU::Vector3f& aPosition);

	CU::Vector3f GetColor() const;
	CU::Vector3f GetPosition() const;
	float GetRange() const;
	PointLight* GetLight() const;

private:
	friend struct PointLightRenderCommand;

	std::string myPath;
	CU::Vector3f myPosition;
	PointLight* myLight;
};