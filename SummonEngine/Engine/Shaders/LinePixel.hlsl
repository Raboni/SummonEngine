#include "LineStructs.hlsli"

PixelOutput main(const VertexToPixel aInput)
{
	PixelOutput returnVal;
	returnVal.myColor = float4(0.0f, 1.0f, 0.0f, 0.7f);
	return returnVal;
}