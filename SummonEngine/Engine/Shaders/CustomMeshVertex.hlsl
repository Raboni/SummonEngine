#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"

struct CustomVertexInput
{
	float4 myPosition : POSITION;
};

PBRPixelInput main(const CustomVertexInput aInput)
{
	const float4 vertexObjectPos = float4(aInput.myPosition.xyz * modelScale.xyz, 1.0f);
	const float4 vertexWorldPos = mul(toWorld, vertexObjectPos);
	const float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	const float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	PBRPixelInput output;
	output.myWorldPosition = vertexWorldPos;
	output.myPosition = vertexProjectionPos;
	output.myNormal = float4(0.0f, 1.0f, 0.0f, 0.0f);
	output.myTangent = float4(1.0f, 0.0f, 0.0f, 0.0f);
	output.myBinormal = float4(0.0f, 0.0f, 1.0f, 0.0f);
	output.myUV = float2(0.0f, 0.0f);
	output.myModelColor = modelColor;
	output.myBaryCoordinates = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return output;
}