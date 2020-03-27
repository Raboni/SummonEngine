#include "FullscreenStructs.hlsli"
#include "FullscreenResources.hlsli"

PixelOutput main(FullscreenPixelInput aInput)
{
	PixelOutput output;
	output.myColor.rgb = fullscreenTexture1.Sample(defaultSampler, aInput.myUV.xy).rgb;
	output.myColor.a = 1.0f;
	return output;
}