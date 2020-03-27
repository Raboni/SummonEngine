#include "stdafx.h"
#include "PointLightRenderCommand.h"
#include "PointLight.h"
#include "PointLightInstance.h"

PointLightRenderCommand::PointLightRenderCommand()
{
	myLight = nullptr;
}

PointLightRenderCommand::PointLightRenderCommand(PointLightInstance* aInstance)
{
	myPosition = aInstance->myPosition;
	myLight = aInstance->myLight;
}

PointLightRenderCommand::PointLightRenderCommand(const PointLightRenderCommand& aCommand)
{
	myPosition = aCommand.myPosition;
	myLight = aCommand.myLight;
}

PointLightRenderCommand& PointLightRenderCommand::operator=(const PointLightRenderCommand& aCommand)
{
	myPosition = aCommand.myPosition;
	myLight = aCommand.myLight;
	return *this;
}

PointLightRenderCommand::~PointLightRenderCommand()
{
	myLight = nullptr;
}
