#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"

PBRPixelInput main(PBRVertexInputInstanced aInput)
{
	float4 vertexObjectPos = float4(aInput.myPosition.xyz * aInput.myModelScale.xyz, 1.0f);
	float4 vertexWorldPos = mul(aInput.myModelMatrix, vertexObjectPos);
	float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	float3x3 toWorldRotation = (float3x3)aInput.myModelMatrix;
	float3 vertexWorldNormal = mul(toWorldRotation, aInput.myNormal.xyz);
	float3 vertexWorldTangent = mul(toWorldRotation, aInput.myTangent.xyz);
	float3 vertexWorldBinormal = mul(toWorldRotation, aInput.myBinormal.xyz);

	PBRPixelInput output;
	output.myWorldPosition = vertexWorldPos;
	output.myPosition = vertexProjectionPos;
	output.myNormal = float4(vertexWorldNormal, 0);
	output.myTangent = float4(vertexWorldTangent, 0);
	output.myBinormal = float4(vertexWorldBinormal, 0);
	output.myUV = aInput.myUV;
	output.myModelColor = aInput.myModelColor;
	output.myBaryCoordinates = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return output;
}