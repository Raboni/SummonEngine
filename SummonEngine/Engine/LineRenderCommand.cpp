#include "stdafx.h"
#include "LineRenderCommand.h"

LineRenderCommand::LineRenderCommand()
{
}
LineRenderCommand::LineRenderCommand(const CU::Vector4f& aFirstPosition, const CU::Vector4f& aSecondPosition)
{
	myFirstPosition = aFirstPosition;
	mySecondPosition = aSecondPosition;
}
LineRenderCommand::LineRenderCommand(const CU::Vector3f& aFirstPosition, const CU::Vector3f& aSecondPosition)
{
	myFirstPosition = CU::Vector4f(aFirstPosition, 1.0f);
	mySecondPosition = CU::Vector4f(aSecondPosition, 1.0f);
}
LineRenderCommand::~LineRenderCommand()
{
}
