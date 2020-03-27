#include "MapStructs.hlsli"

PixelOutput main(GeometryToPixel aInput)
{
	PixelOutput returnVal;
	float4 textureColor = MapTexture.Sample(DefaultSampler, aInput.myUV.xy).rgba;
	returnVal.myColor = textureColor;
	return returnVal;
}