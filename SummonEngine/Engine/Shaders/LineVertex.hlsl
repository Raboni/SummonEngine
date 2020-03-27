#include "LineStructs.hlsli"
#include "PBRResources.hlsli"

VertexToPixel main(const VertexInput aInput)
{
	const float4 vertexWorldPos = aInput.myPosition.xyzw;
	const float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	const float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	VertexToPixel returnVal;
	returnVal.myPosition = vertexProjectionPos;
	return returnVal;
}