#include "MapStructs.hlsli"

VertexToGeometry main(VertexInput aInput)
{
	VertexToGeometry returnVal;
	returnVal.myPositionScreen = aInput.myPositionScreen;
	returnVal.mySizeScreen = aInput.mySizeScreen;
	returnVal.myPositionWorld = aInput.myPositionWorld;
	returnVal.mySizeWorld = aInput.mySizeWorld;
	returnVal.myFocus = aInput.myFocus;
	returnVal.myRotation = aInput.myRotation;
	returnVal.myZoom = aInput.myZoom;
	return returnVal;
}