#pragma once
#include <Vector2.hpp>

class Sprite;

struct MapRenderCommand
{
public:
	MapRenderCommand();
	MapRenderCommand(const MapRenderCommand& aCommand);
	MapRenderCommand& operator=(const MapRenderCommand& aCommand);
	~MapRenderCommand();

	//Sprite to be used as map (uses map camera if null)
	Sprite* mySprite;
	//Position on screen
	CU::Vector2f myPositionScreen;
	//Size on screen
	CU::Vector2f mySizeScreen;
	//Position in world
	CU::Vector2f myPositionWorld;
	//Size in world
	CU::Vector2f mySizeWorld;
	//Where the focus is on the map (usually player position on map)
	CU::Vector2f myFocus;
	float myRotation;
	float myZoom;
};