#include "NavmeshStructs.hlsli"
#include "PBRResources.hlsli"

PixelOutput main(VertexToPixel aInput)
{
	PixelOutput returnVal;
	returnVal.myColor = float4(1.0f, 0.0f, 0.0f, 0.7f);
	return returnVal;
}