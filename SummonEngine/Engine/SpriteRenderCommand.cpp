#include "stdafx.h"
#include "SpriteRenderCommand.h"

SpriteRenderCommand::SpriteRenderCommand()
{
	mySprite = nullptr;
}

SpriteRenderCommand::SpriteRenderCommand(SpriteInstance* aInstance)
{
	myData.Init(1);
	myData.Add(aInstance->GetData());
	mySprite = aInstance->GetSprite();
}

SpriteRenderCommand::SpriteRenderCommand(const SpriteRenderCommand& aCommand)
{
	myData = aCommand.myData;
	mySprite = aCommand.mySprite;
}

SpriteRenderCommand& SpriteRenderCommand::operator=(const SpriteRenderCommand& aCommand)
{
	myData = aCommand.myData;
	mySprite = aCommand.mySprite;
	return *this;
}

SpriteRenderCommand::~SpriteRenderCommand()
{
}
