#include "stdafx.h"
#include "MapRenderCommand.h"

MapRenderCommand::MapRenderCommand()
{
	mySprite = nullptr;
	mySizeScreen = CU::Vector2f::One;
	mySizeWorld = CU::Vector2f(2.0f, 2.0f);
	myRotation = 0.0f;
	myZoom = 1.0f;
}
MapRenderCommand::MapRenderCommand(const MapRenderCommand& aCommand)
{
	mySprite = aCommand.mySprite;
	myPositionScreen = aCommand.myPositionScreen;
	mySizeScreen = aCommand.mySizeScreen;
	myPositionWorld = aCommand.myPositionWorld;
	mySizeWorld = aCommand.mySizeWorld;
	myFocus = aCommand.myFocus;
	myRotation = aCommand.myRotation;
	myZoom = aCommand.myZoom;
}
MapRenderCommand& MapRenderCommand::operator=(const MapRenderCommand& aCommand)
{
	mySprite = aCommand.mySprite;
	myPositionScreen = aCommand.myPositionScreen;
	mySizeScreen = aCommand.mySizeScreen;
	myPositionWorld = aCommand.myPositionWorld;
	mySizeWorld = aCommand.mySizeWorld;
	myFocus = aCommand.myFocus;
	myRotation = aCommand.myRotation;
	myZoom = aCommand.myZoom;
	return *this;
}
MapRenderCommand::~MapRenderCommand()
{
}
