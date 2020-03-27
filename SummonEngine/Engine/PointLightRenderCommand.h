#pragma once
#include <Vector3.hpp>

class PointLight;
class PointLightInstance;

struct PointLightRenderCommand
{
	PointLightRenderCommand();
	PointLightRenderCommand(PointLightInstance* aInstance);
	PointLightRenderCommand(const PointLightRenderCommand& aCommand);
	PointLightRenderCommand& operator=(const PointLightRenderCommand& aCommand);
	~PointLightRenderCommand();

	CU::Vector3f myPosition;
	PointLight* myLight;
};