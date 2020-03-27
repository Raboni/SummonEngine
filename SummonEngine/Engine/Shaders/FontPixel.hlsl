#include "FontStructs.hlsli"

PixelOutput main(VertexToPixel aInput)
{
	PixelOutput output;
	output.myColor = FontTexture.Sample(DefaultSampler, aInput.myUV.xy).rgba;
	output.myColor.rgb = output.myColor.rgb * ObjectColor.rgb;
	return output;
}