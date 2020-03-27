#include "NavmeshStructs.hlsli"
#include "PBRResources.hlsli"

VertexToPixel main(VertexInput aInput)
{
	VertexToPixel returnVal;
	float4 vertexObjectPos = float4(aInput.myPosition.xyz * modelScale.xyz, 1.0f);
	float4 vertexWorldPos = mul(toWorld, vertexObjectPos);
	float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	returnVal.myPosition = vertexProjectionPos;
	return returnVal;
}