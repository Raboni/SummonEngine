#pragma once
#include "SpriteInstance.h"
#include <Matrix4x4.hpp>

struct SpriteRenderCommand
{
	SpriteRenderCommand();
	SpriteRenderCommand(SpriteInstance* aInstance);
	SpriteRenderCommand(const SpriteRenderCommand& aCommand);
	SpriteRenderCommand& operator=(const SpriteRenderCommand& aCommand);
	~SpriteRenderCommand();

	CU::GrowingArray<SpriteRenderData, int> myData;
	Sprite* mySprite;
};