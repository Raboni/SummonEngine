#include "SpriteStructs.hlsli"

VertexToGeometry main(VertexInput aInput)
{
	VertexToGeometry returnVal;
	returnVal.myColor = aInput.myColor;
	returnVal.myPosition = aInput.myPosition;
	returnVal.mySize = aInput.mySize;
	returnVal.myPivot = aInput.myPivot;
	returnVal.myRotation = aInput.myRotation;
	return returnVal;
}