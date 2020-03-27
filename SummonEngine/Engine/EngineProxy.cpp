#include "stdafx.h"
#include "EngineProxy.h"
#include "Engine.h"

Engine* EngineProxy::ourEngine = nullptr;

void EngineProxy::SetActiveScene(const unsigned int aSceneIndex)
{
	ourEngine->SetActiveScene(aSceneIndex);
}
Scene* EngineProxy::GetActiveScene()
{
	return ourEngine->GetActiveScene();
}
Scene* EngineProxy::GetScene(const unsigned int aSceneIndex)
{
	return ourEngine->GetScene(aSceneIndex);
}

void EngineProxy::SetUseVR(const bool aActive)
{
	ourEngine->SetUseVR(aActive);
}
bool EngineProxy::GetUseVR()
{
	return ourEngine->GetUseVR();
}

CU::Vector2i EngineProxy::GetWindowSize()
{
	return ourEngine->GetWindowSize();
}

RenderSettings& EngineProxy::GetRenderSettings()
{
	return ourEngine->GetRenderSettings();
}

void EngineProxy::Draw(const LineRenderCommand& aLine)
{
	ourEngine->Draw(aLine);
}
void EngineProxy::Draw(SpriteInstance* aSprite)
{
	ourEngine->Draw(aSprite);
}
void EngineProxy::Draw(TextInstance* aText)
{
	ourEngine->Draw(aText);
}

void EngineProxy::SetShouldRenderFustrum(const bool aShouldRender)
{
	ourEngine->SetShouldRenderFustrum(aShouldRender);
}

void EngineProxy::WaitForLoading()
{
	ourEngine->WaitForLoading();
}
bool EngineProxy::IsLoading()
{
	return ourEngine->IsLoading();
}