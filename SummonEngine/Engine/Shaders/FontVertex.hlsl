#include "FontStructs.hlsli"

VertexToPixel main(VertexInput aInput)
{
	VertexToPixel returnVal;
	returnVal.myPosition = aInput.myPosition;
	returnVal.myPosition.xy -= ObjectSize * ObjectPivot;
	returnVal.myPosition.xy = ((returnVal.myPosition.xy * ObjectScale) / (ScreenSize.xy * 0.5f));
	returnVal.myPosition.x += ObjectPosition.x * (ScreenSize.y / ScreenSize.x);
	returnVal.myPosition.y -= ObjectPosition.y;
	returnVal.myUV = aInput.myUV;
	return returnVal;
}