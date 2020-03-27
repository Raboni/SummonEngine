#pragma once
#include "WindowHandler.h"
#include "DXFramework.h"
#include "RenderManager.h"
#include "VRManager.h"
#include "Scene.h"
#include "EngineMessages.h"
#include "RenderSettings.h"
#include "RenderBuffer.h"
#include <functional>
#include <Windows.h>

class Engine : public CU::Subscriber<EngineMessages>
{
public:
	Engine();
	~Engine();

	bool Init(WindowHandler::WindowData& aWinData, const bool aUseVR);
	void BeginFrame();
	void EndFrame();
	void Render();
	void Update();
	void UpdateAnimation(const float aDeltaTime);

	void SetActiveScene(const unsigned int aSceneIndex);
	Scene* GetActiveScene();
	Scene* GetScene(const unsigned int aSceneIndex);
	HWND GetWindow();
	CU::Vector2i GetWindowSize();
	void OpenConsole(const CU::Vector2i& aPosition = CU::Vector2i::Zero, const CU::Vector2i& aSize = CU::Vector2i::Zero, const std::string& aTitle = "Debug Console");
	void CloseConsole();
	void SetUseVR(const bool aActive);
	bool GetUseVR();
	RenderSettings& GetRenderSettings();
	float GetLastRenderDuration() const;
	float GetLastCullingDuration() const;

	RenderBuffer& GetRenderBuffer();
	void Draw(const LineRenderCommand& aLine);
	void Draw(SpriteInstance* aSprite);
	void Draw(TextInstance* aText);
	void CreateRenderCommands(RenderSettings& aSettings);
	void SwitchAndCleanBuffers();
	void SetShouldRenderFustrum(const bool aShouldRender);

	void WaitForLoading();
	bool IsLoading() const;

	void RecieveMessage(const CU::Message<EngineMessages>& aMessage) override;

private:
	WindowHandler myWinHandler;
	DXFramework myFramework;
	RenderManager myRenderer;
	VRManager myVRManager;
	std::array<Scene, 8> myScene;
	RenderSettings myRenderSettings;
	RenderSettings myRenderSettingsBuffer;
	RenderBuffer myBufferA;
	RenderBuffer myBufferB;
	unsigned int myActiveScene;
	float myLastRenderDuration;
	float myLastCullingDuration;
	std::atomic<bool> myRenderBufferA;
	bool myDirtyBuffer;
	bool myRenderFustrum;
	bool myUseVR;
};