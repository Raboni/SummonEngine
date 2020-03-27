#include "ParticleStructs.hlsli"

VertexToGeometry main(VertexInput aInput)
{
	VertexToGeometry returnVal;
	//float4 vertexObjectPos = aInput.myPosition.xyzw;
	//float4 vertexWorldPos = mul(toWorld, vertexObjectPos);
	float4 vertexWorldPos = aInput.myPosition.xyzw;
	float4 vertexViewPos = mul(toCamera, vertexWorldPos);

	returnVal.myPosition = vertexViewPos;
	returnVal.myColor = aInput.myColor;
	returnVal.mySize = aInput.mySize;
	return returnVal;
}