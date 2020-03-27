#pragma once
#include <Vector4.hpp>

struct LineRenderCommand
{
	LineRenderCommand();
	LineRenderCommand(const CU::Vector4f& aFirstPosition, const CU::Vector4f& aSecondPosition);
	LineRenderCommand(const CU::Vector3f& aFirstPosition, const CU::Vector3f& aSecondPosition);
	~LineRenderCommand();

	CU::Vector4f myFirstPosition;
	CU::Vector4f mySecondPosition;
};