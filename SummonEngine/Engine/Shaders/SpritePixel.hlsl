#include "SpriteStructs.hlsli"

PixelOutput main(GeometryToPixel aInput)
{
	PixelOutput returnVal;
	float4 textureColor = SpriteTexture.Sample(DefaultSampler, aInput.myUV.xy).rgba;
	returnVal.myColor = textureColor * aInput.myColor;
	return returnVal;
}