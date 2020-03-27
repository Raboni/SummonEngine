#include "stdafx.h"
#include "SpriteUnits.h"

float SpriteUnits::myBorderTop = 1.0f;
float SpriteUnits::myBorderBottom = 1.0f;
float SpriteUnits::myBorderLeft = 1.0f;
float SpriteUnits::myBorderRight = 1.0f;
float SpriteUnits::myBorderWidth = 2.0f;
float SpriteUnits::myBorderHeight = 2.0f;
float SpriteUnits::myPixelSize = 0.001f;


const float SpriteUnits::GetBorderTop()
{
	return myBorderTop;
}
const float SpriteUnits::GetBorderBottom()
{
	return myBorderBottom;
}
const float SpriteUnits::GetBorderLeft()
{
	return myBorderLeft;
}
const float SpriteUnits::GetBorderRight()
{
	return myBorderRight;
}
const float SpriteUnits::GetBorderWidth()
{
	return myBorderWidth;
}
const float SpriteUnits::GetBorderHeight()
{
	return myBorderHeight;
}

const float SpriteUnits::GetPixelSize()
{
	return myPixelSize;
}
