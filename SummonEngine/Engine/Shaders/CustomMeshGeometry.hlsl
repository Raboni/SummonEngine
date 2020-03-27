#include "GBufferStructs.hlsli"

[maxvertexcount(3)]
void main(triangle PBRPixelInput aInput[3], inout TriangleStream<PBRPixelInput> aOutput)
{
	float3 position[3];
	position[0] = aInput[0].myWorldPosition.xyz;
	position[1] = aInput[1].myWorldPosition.xyz;
	position[2] = aInput[2].myWorldPosition.xyz;

	const float4 normal = float4(normalize(cross(position[1] - position[0], position[2] - position[0])), 0.0f);
	aInput[0].myNormal = normal;
	aInput[1].myNormal = normal;
	aInput[2].myNormal = normal;

	aInput[0].myBaryCoordinates.xyz = float3(1.0f, 0.0f, 0.0f);
	aInput[1].myBaryCoordinates.xyz = float3(0.0f, 1.0f, 0.0f);
	aInput[2].myBaryCoordinates.xyz = float3(0.0f, 0.0f, 1.0f);

	aOutput.Append(aInput[0]);
	aOutput.Append(aInput[1]);
	aOutput.Append(aInput[2]);
}