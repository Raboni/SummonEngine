#include "ParticleStructs.hlsli"

PixelOutput main(GeometryToPixel aInput)
{
	PixelOutput returnVal;
	float4 textureColor = ParticleTexture.Sample(DefaultSampler, aInput.myUV.xy).rgba;
	returnVal.myColor.rgba = textureColor.rgba * aInput.myColor.rgba;
	return returnVal;
}