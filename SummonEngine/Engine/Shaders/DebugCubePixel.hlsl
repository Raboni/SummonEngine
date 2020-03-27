#include "DebugStructs.hlsli"
#include "PBRResources.hlsli"

PixelOutput main(DebugCubePixelInput aInput)
{
	float4 textureColor = AlbedoTexture.Sample(DefaultSampler, aInput.myUV.xy).rgba * modelColor;

	PixelOutput output;
	output.myColor = textureColor;
	return output;
}