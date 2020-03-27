#pragma once
#include "Scene.h"
#include "LineRenderCommand.h"

class Engine;
class TextInstance;

class EngineProxy
{
public:
	static void SetActiveScene(const unsigned int aSceneIndex);
	static Scene* GetActiveScene();
	static Scene* GetScene(const unsigned int aSceneIndex);
	static void SetUseVR(const bool aActive);
	static bool GetUseVR();
	static CU::Vector2i GetWindowSize();
	static RenderSettings& GetRenderSettings();

	static void Draw(const LineRenderCommand& aLine);
	static void Draw(SpriteInstance* aSprite);
	static void Draw(TextInstance* aText);
	static void SetShouldRenderFustrum(const bool aShouldRender);

	static void WaitForLoading();
	static bool IsLoading();

private:
	friend class Engine;
	static Engine* ourEngine;
};