#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"

SkyboxPixelInput main(const SkyboxVertexInput aInput)
{
	const float4 vertexObjectPos = float4(aInput.myPosition.xyz * modelScale.xyz, 1.0f);
	const float4 vertexWorldPos = mul(toWorld, vertexObjectPos);
	const float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	const float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	SkyboxPixelInput output;
	output.myPosition = vertexProjectionPos.xyww;
	output.myTexCoord = aInput.myPosition;
	return output;
}