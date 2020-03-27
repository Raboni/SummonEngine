#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"

PBRPixelInput main(PBRVertexInput aInput)
{
	const float4 vertexObjectPos = float4(aInput.myPosition.xyz * modelScale.xyz, 1.0f);
	const float4 vertexWorldPos = mul(toWorld, vertexObjectPos);
	const float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	const float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	const float3x3 toWorldRotation = (float3x3)toWorld;
	const float3 vertexWorldNormal = mul(toWorldRotation, aInput.myNormal.xyz);
	const float3 vertexWorldTangent = mul(toWorldRotation, aInput.myTangent.xyz);
	const float3 vertexWorldBinormal = mul(toWorldRotation, aInput.myBinormal.xyz);

	PBRPixelInput output;
	output.myWorldPosition = vertexWorldPos;
	output.myPosition = vertexProjectionPos;
	output.myNormal = float4(vertexWorldNormal, 0);
	output.myTangent = float4(vertexWorldTangent, 0);
	output.myBinormal = float4(vertexWorldBinormal, 0);
	output.myUV = aInput.myUV;
	output.myModelColor = modelColor;
	output.myBaryCoordinates = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return output;
}