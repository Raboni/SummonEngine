#include "DebugStructs.hlsli"
#include "PBRResources.hlsli"

DebugCubePixelInput main(DebugCubeVertexInput aInput)
{
	float4 vertexObjectPos = float4(aInput.myPosition.xyz * modelScale.xyz, 1.0f);
	float4 vertexWorldPos = mul(toWorld, vertexObjectPos);
	float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	DebugCubePixelInput output;
	output.myPosition = vertexProjectionPos;
	output.myColor = aInput.myColor;
	output.myUV = aInput.myUV;
	return output;
}