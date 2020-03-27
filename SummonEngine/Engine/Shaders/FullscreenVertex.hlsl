#include "FullscreenStructs.hlsli"

FullscreenPixelInput main(const FullscreenVertexInput aInput)
{
	static const float4 positions[3] =
	{
		float4(-1.0f, -1.0f, 0.0f, 1.0f),
		float4(-1.0f, 3.0f, 0.0f, 1.0f),
		float4(3.0f, -1.0f, 0.0f, 1.0f)
	};
	static const float2 uvs[3] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, -1.0f),
		float2(2.0f, 1.0f)
	};

	FullscreenPixelInput output;
	output.myPosition = positions[aInput.myIndex];
	output.myUV = uvs[aInput.myIndex];
	return output;
}