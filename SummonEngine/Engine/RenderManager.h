#pragma once
#include "RenderStateManager.h"
#include "DeferredRenderer.h"
#include "ForwardRenderer.h"
#include "WireframeRenderer.h"
#include "NavmeshRenderer.h"
#include "LineRenderer.h"
#include "FrustumRenderer.h"
#include "ParticleRenderer.h"
#include "MapRenderer.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "FullscreenRenderer.h"
#include "FullscreenTextureFactory.h"
#include "FullscreenTexture.h"
#include <atomic>

class Engine;
class DXFramework;
class WindowHandler;
class VRManager;
struct ID3D11DeviceContext;

class RenderManager
{
public:
	RenderManager(RenderSettings& aSettings);
	~RenderManager();

	bool Init(Engine* aEngine, DXFramework* aFramework, WindowHandler* aWindowHandler, VRManager* aVRManager);
	void InitVR(VRManager* aVRManager);
	void QuitVR();
	void Render();

private:
	void RenderBloom();

	RenderSettings& mySettings;
	Engine* myEngine;
	ID3D11DeviceContext* myContext;

	RenderStateManager myStateManager;
	DeferredRenderer myDeferredRenderer;
	ForwardRenderer myForwardRenderer;
	WireframeRenderer myWireframeRenderer;
	Nav::NavmeshRenderer myNavmeshRenderer;
	LineRenderer myLineRenderer;
	FrustumRenderer myFrustumRenderer;
	ParticleRenderer myParticleRenderer;
	MapRenderer myMapRenderer;
	SpriteRenderer mySpriteRenderer;
	TextRenderer myTextRenderer;
	FullscreenRenderer myFullscreenRenderer;

	FullscreenTextureFactory myTextureFactory;
	FullscreenTexture myIntermediateDepth;
	FullscreenTexture myIntermediateDepthVR;
	FullscreenTexture myBackbuffer;
	FullscreenTexture myLeftEye;
	FullscreenTexture myRightEye;
	FullscreenTexture myIntermediateTexture;
	FullscreenTexture myIntermediateTextureCopy;
	FullscreenTexture myIntermediateTextureVR;
	FullscreenTexture myLuminanceTexture;
	FullscreenTexture myHalfTexture;
	FullscreenTexture myQuarterTexture;
	FullscreenTexture myBlurTexture1;
	FullscreenTexture myBlurTexture2;
	FullscreenTexture myMapTexture;
	GBuffer myGBuffer;
	GBuffer myGBufferVR;

	CU::Vector2f myScreenSize;
	std::atomic<bool> myUseVR;
};