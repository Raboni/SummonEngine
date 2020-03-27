#include "stdafx.h"
#include "RenderManager.h"
#include "Engine.h"
#include "DXFramework.h"
#include "WindowHandler.h"
#include "VRManager.h"
#include "..\OpenVR\Include\openvr.h"
#include "RenderBuffer.h"
#include "Threadpool.h"
#include <DebugTools.h>
#include <future>
#include <d3d11.h>

RenderManager::RenderManager(RenderSettings& aSettings) : myForwardRenderer(aSettings), myDeferredRenderer(aSettings), mySettings(aSettings)
{
}
RenderManager::~RenderManager()
{
	myIntermediateDepthVR.ReleaseDepth();
	myLeftEye.ReleaseTexture();
	myRightEye.ReleaseTexture();
	myIntermediateTextureVR.ReleaseTexture();
	myGBufferVR.Release();

	myIntermediateDepth.ReleaseDepth();
	myBackbuffer.ReleaseTexture();
	myIntermediateTexture.ReleaseTexture();
	myIntermediateTextureCopy.ReleaseTexture();
	myLuminanceTexture.ReleaseTexture();
	myHalfTexture.ReleaseTexture();
	myQuarterTexture.ReleaseTexture();
	myBlurTexture1.ReleaseTexture();
	myBlurTexture2.ReleaseTexture();
	myMapTexture.ReleaseTexture();
	myGBuffer.Release();
}

bool RenderManager::Init(Engine* aEngine, DXFramework* aFramework, WindowHandler* aWindowHandler, VRManager* aVRManager)
{
	myUseVR = aVRManager != nullptr;
	myEngine = aEngine;
	if (aEngine == nullptr || aFramework == nullptr || aWindowHandler == nullptr)
	{
		return false;
	}
	if (!myDeferredRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myForwardRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myWireframeRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myNavmeshRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myLineRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myFrustumRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myParticleRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myMapRenderer.Init(aFramework))
	{
		return false;
	}
	if (!mySpriteRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myTextRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myFullscreenRenderer.Init(aFramework))
	{
		return false;
	}
	if (!myTextureFactory.Init(aFramework))
	{
		return false;
	}
	if (!myStateManager.Init(aFramework))
	{
		return false;
	}
	myStateManager.SetStatesDefault();

	myContext = aFramework->GetDeviceContext();

	myIntermediateDepth = myTextureFactory.CreateDepth(aWindowHandler->GetWindowSize<float>(), DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT);
	ID3D11Texture2D* backbufferTexture = aFramework->GetBackbufferTexture();
	if (backbufferTexture == nullptr)
	{
		return false;
	}
	myBackbuffer = myTextureFactory.CreateTexture(backbufferTexture);
	if (myUseVR)
	{
		myIntermediateDepthVR = myTextureFactory.CreateDepth(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT);
		myLeftEye = myTextureFactory.CreateTexture(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myRightEye = myTextureFactory.CreateTexture(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myIntermediateTextureVR = myTextureFactory.CreateTexture(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myGBufferVR = myTextureFactory.CreateGBuffer(aVRManager->GetEyeRenderTargetSize());
	}
	myIntermediateTexture = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myIntermediateTextureCopy = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myLuminanceTexture = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>() / 2.0f, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myHalfTexture = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>() / 2.0f, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myQuarterTexture = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>() / 4.0f, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myBlurTexture1 = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>() / 4.0f, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myBlurTexture2 = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>() / 4.0f, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myMapTexture = myTextureFactory.CreateTexture(aWindowHandler->GetWindowSize<float>(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	myGBuffer = myTextureFactory.CreateGBuffer(aWindowHandler->GetWindowSize<float>());

	myScreenSize = aWindowHandler->GetWindowSize<float>();

	return true;
}

void RenderManager::InitVR(VRManager* aVRManager)
{
	myUseVR = true;
	if (myIntermediateDepthVR.myContext == nullptr)
	{
		myIntermediateDepthVR = myTextureFactory.CreateDepth(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT);
		myLeftEye = myTextureFactory.CreateTexture(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myRightEye = myTextureFactory.CreateTexture(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myIntermediateTextureVR = myTextureFactory.CreateTexture(aVRManager->GetEyeRenderTargetSize(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myGBufferVR = myTextureFactory.CreateGBuffer(aVRManager->GetEyeRenderTargetSize());
	}
}

void RenderManager::QuitVR()
{
	myUseVR = false;
}

void RenderManager::Render()
{
	if (myUseVR)
	{
		myIntermediateDepthVR.ClearDepth();
		myLeftEye.ClearTexture();
		myRightEye.ClearTexture();
		myIntermediateTextureVR.ClearTexture();
		myGBufferVR.ClearTextures(CU::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
	}
	myIntermediateDepth.ClearDepth();
	myBackbuffer.ClearTexture();
	myIntermediateTexture.ClearTexture();
	myIntermediateTextureCopy.ClearTexture();
	myMapTexture.ClearTexture();
	myGBuffer.ClearTextures(CU::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
	myStateManager.SetStatesDefault();
	/*if (mySettings.UseDeferredContexts)
	{
		myIntermediateDepth.ClearDepth(1.0f, 0, myDeferredRenderer.GetContext());
		myBackbuffer.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContext());
		myIntermediateTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContext());
		myIntermediateTextureCopy.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContext());
		myMapTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContext());
		myGBuffer.ClearTextures(CU::Vector4f::Zero, myDeferredRenderer.GetContext());
		myStateManager.SetStatesDefault(myDeferredRenderer.GetContext());

		myIntermediateDepth.ClearDepth(1.0f, 0, myDeferredRenderer.GetContextInstanced());
		myBackbuffer.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextInstanced());
		myIntermediateTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextInstanced());
		myIntermediateTextureCopy.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextInstanced());
		myMapTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextInstanced());
		myGBuffer.ClearTextures(CU::Vector4f::Zero, myDeferredRenderer.GetContextInstanced());
		myStateManager.SetStatesDefault(myDeferredRenderer.GetContextInstanced());

		myIntermediateDepth.ClearDepth(1.0f, 0, myDeferredRenderer.GetContextEnviromentLight());
		myBackbuffer.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextEnviromentLight());
		myIntermediateTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextEnviromentLight());
		myIntermediateTextureCopy.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextEnviromentLight());
		myMapTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextEnviromentLight());
		myGBuffer.ClearTextures(CU::Vector4f::Zero, myDeferredRenderer.GetContextEnviromentLight());
		myStateManager.SetStatesDefault(myDeferredRenderer.GetContextEnviromentLight());

		myIntermediateDepth.ClearDepth(1.0f, 0, myDeferredRenderer.GetContextPointLight());
		myBackbuffer.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextPointLight());
		myIntermediateTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextPointLight());
		myIntermediateTextureCopy.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextPointLight());
		myMapTexture.ClearTexture(CU::Vector4f::Zero, myDeferredRenderer.GetContextPointLight());
		myGBuffer.ClearTextures(CU::Vector4f::Zero, myDeferredRenderer.GetContextPointLight());
		myStateManager.SetStatesDefault(myDeferredRenderer.GetContextPointLight());

		myIntermediateDepth.ClearDepth(1.0f, 0, myForwardRenderer.GetContext());
		myBackbuffer.ClearTexture(CU::Vector4f::Zero, myForwardRenderer.GetContext());
		myIntermediateTexture.ClearTexture(CU::Vector4f::Zero, myForwardRenderer.GetContext());
		myIntermediateTextureCopy.ClearTexture(CU::Vector4f::Zero, myForwardRenderer.GetContext());
		myMapTexture.ClearTexture(CU::Vector4f::Zero, myForwardRenderer.GetContext());
		myGBuffer.ClearTextures(CU::Vector4f::Zero, myForwardRenderer.GetContext());
		myStateManager.SetStatesDefault(myForwardRenderer.GetContext());
	}*/

	RenderBuffer& renderBuffer = myEngine->GetRenderBuffer();
	EnviromentLight* envLight = &renderBuffer.myEnviromentLight;
	CU::Camera* cullCamera = &renderBuffer.myMainCamera;
	CU::Camera* viewCamera = &renderBuffer.myMainCamera;
	if (renderBuffer.myUseViewCamera)
	{
		viewCamera = &renderBuffer.myViewCamera;
	}
	CU::Camera* vrCameraL = &renderBuffer.myVRCameraL;
	CU::Camera* vrCameraR = &renderBuffer.myVRCameraR;
	if (myUseVR)
	{
		vrCameraL = &renderBuffer.myVRCameraL;
		vrCameraR = &renderBuffer.myVRCameraR;
		cullCamera = vrCameraR;
	}
	CU::Camera* mapCamera = nullptr;
	if (renderBuffer.myUseMapCamera)
	{
		mapCamera = &renderBuffer.myMapCamera;
	}
	myDeferredRenderer.UpdateInstanceMaxCount(static_cast<unsigned int>(renderBuffer.myModelsInstanced.Size()));

	if (!myUseVR || !mySettings.VRMirrorEye)
	{
		if (renderBuffer.myUseMapCamera && !mySettings.UseDeferredContexts)
		{
			myGBuffer.SetAsActiveTarget(&myIntermediateDepth);
			myDeferredRenderer.GenerateGBufferInstanced(mapCamera, renderBuffer.myModelsMap, renderBuffer.myModelMapStartIndexList);
			//myDeferredRenderer.GenerateGBuffer(mapCamera, renderBuffer.myModelsMap);
			myIntermediateTexture.SetAsActiveTarget();
			myGBuffer.SetAllAsResources();
			myStateManager.SetBlendState(BlendState::Disable);
			myDeferredRenderer.RenderEnvLight(mapCamera, envLight);

			myMapTexture.SetAsActiveTarget();
			myIntermediateTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(FullscreenShaderType::Copy);

			myMapRenderer.SetMapTexture(&myMapTexture);

			myIntermediateDepth.ClearDepth();
			myBackbuffer.ClearTexture();
			myIntermediateTexture.ClearTexture();
			myGBuffer.ClearTextures(CU::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
		}

		/*if (mySettings.UseDeferredContexts)
		{
			myGBuffer.SetAsActiveTarget(&myIntermediateDepth, myDeferredRenderer.GetContext());
			myGBuffer.SetAsActiveTarget(&myIntermediateDepth, myDeferredRenderer.GetContextInstanced());
			myStateManager.SetBlendState(BlendState::Disable, myDeferredRenderer.GetContext());
			myStateManager.SetBlendState(BlendState::Disable, myDeferredRenderer.GetContextInstanced());

			myIntermediateTexture.SetAsActiveTarget(nullptr, myDeferredRenderer.GetContextEnviromentLight());
			myIntermediateTexture.SetAsActiveTarget(nullptr, myDeferredRenderer.GetContextPointLight());
			myGBuffer.SetAllAsResources(myDeferredRenderer.GetContextEnviromentLight());
			myGBuffer.SetAllAsResources(myDeferredRenderer.GetContextPointLight());
			myStateManager.SetBlendState(BlendState::Disable, myDeferredRenderer.GetContextEnviromentLight());
			myStateManager.SetBlendState(BlendState::Addativeblend, myDeferredRenderer.GetContextPointLight());


			const long long taskIDInstanced = Threadpool::GetInstance()->AddTask(std::bind(&DeferredRenderer::GenerateGBufferInstanced, &myDeferredRenderer, viewCamera, renderBuffer.myModelsInstanced, renderBuffer.myModelInstancedStartIndexList), true);
			const long long taskIDDeferred = Threadpool::GetInstance()->AddTask(std::bind(&DeferredRenderer::GenerateGBuffer, &myDeferredRenderer, viewCamera, renderBuffer.myModelsDeferred), true);
			const long long taskIDEnvLight = Threadpool::GetInstance()->AddTask(std::bind(&DeferredRenderer::RenderEnvLight, &myDeferredRenderer, viewCamera, envLight), true);
			const long long taskIDPointLight = Threadpool::GetInstance()->AddTask(std::bind(&DeferredRenderer::RenderPointLight, &myDeferredRenderer, viewCamera, renderBuffer.myPointLights), true);
			Threadpool::GetInstance()->WaitForTask(taskIDInstanced);
			Threadpool::GetInstance()->WaitForTask(taskIDDeferred);
			Threadpool::GetInstance()->WaitForTask(taskIDEnvLight);
			Threadpool::GetInstance()->WaitForTask(taskIDPointLight);
			//myDeferredRenderer.GenerateGBufferInstanced(viewCamera, renderBuffer.myModelsInstanced, renderBuffer.myModelInstancedStartIndexList);
			myContext->ExecuteCommandList(myDeferredRenderer.GetCommandListInstanced(), false);
			//myDeferredRenderer.GenerateGBuffer(viewCamera, renderBuffer.myModelsDeferred);
			myContext->ExecuteCommandList(myDeferredRenderer.GetCommandList(), false);

			//myIntermediateTexture.SetAsActiveTarget(nullptr, myDeferredRenderer.GetContextEnviromentLight());
			//myIntermediateTexture.SetAsActiveTarget(nullptr, myDeferredRenderer.GetContextPointLight());
			//myGBuffer.SetAllAsResources(myDeferredRenderer.GetContextEnviromentLight());
			//myGBuffer.SetAllAsResources(myDeferredRenderer.GetContextPointLight());
			//myStateManager.SetBlendState(BlendState::Disable, myDeferredRenderer.GetContextEnviromentLight());
			//myStateManager.SetBlendState(BlendState::Addativeblend, myDeferredRenderer.GetContextPointLight());

			//myDeferredRenderer.RenderEnvLight(viewCamera, envLight);
			myContext->ExecuteCommandList(myDeferredRenderer.GetCommandListEnviromentLight(), false);
			//myDeferredRenderer.RenderPointLight(viewCamera, renderBuffer.myPointLights);
			myContext->ExecuteCommandList(myDeferredRenderer.GetCommandListPointLight(), false);

			//myIntermediateTexture.SetAsActiveTarget(&myIntermediateDepth, myForwardRenderer.GetContext());
			//myStateManager.SetBlendState(BlendState::Alphablend, myForwardRenderer.GetContext());

			if (renderBuffer.myModelsForward.Size() > 0)
			{
				myForwardRenderer.Render(envLight, viewCamera, renderBuffer.myModelsForward, renderBuffer.myModelPointLights);
				myContext->ExecuteCommandList(myForwardRenderer.GetCommandList(), false);
			}

			//myGBuffer.SetAllAsResources();
			myStateManager.SetBlendState(BlendState::Alphablend);
			myIntermediateTexture.SetAsActiveTarget(&myIntermediateDepth);
		}
		else*/
		{
			//RenderOnTexture(false, renderBuffer, myBackbuffer);
			myGBuffer.SetAsActiveTarget(&myIntermediateDepth);
			myStateManager.SetBlendState(BlendState::Disable);
			myDeferredRenderer.GenerateGBufferInstanced(viewCamera, renderBuffer.myModelsInstanced, renderBuffer.myModelInstancedStartIndexList);
			myDeferredRenderer.GenerateGBuffer(viewCamera, renderBuffer.myModelsDeferred);
			myIntermediateTexture.SetAsActiveTarget();
			myGBuffer.SetAllAsResources();
			myStateManager.SetBlendState(BlendState::Disable);
			myDeferredRenderer.RenderEnvLight(viewCamera, envLight);
			myStateManager.SetBlendState(BlendState::Addativeblend);
			myDeferredRenderer.RenderPointLight(viewCamera, renderBuffer.myPointLights);

			myIntermediateTexture.SetAsActiveTarget(&myIntermediateDepth);
			myStateManager.SetDepthStencilState(DepthStencilState::ReadWriteLessEqual);
			myStateManager.SetBlendState(BlendState::Disable);
			myForwardRenderer.RenderSkybox(viewCamera, envLight, renderBuffer.mySkybox);
			myStateManager.SetDepthStencilState(DepthStencilState::Default);
			myStateManager.SetBlendState(BlendState::Alphablend);
			myForwardRenderer.Render(viewCamera, envLight, renderBuffer.myModelsForward, renderBuffer.myModelPointLights);
		}

		if (mySettings.FXAA)
		{
			myIntermediateTextureCopy.SetAsActiveTarget();
			myIntermediateTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(FullscreenShaderType::FXAA);

			myIntermediateTexture.SetAsActiveTarget();
			myIntermediateTextureCopy.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(FullscreenShaderType::Copy);

			myIntermediateTexture.SetAsActiveTarget(&myIntermediateDepth);
		}

		if (renderBuffer.myNavmeshes.Size() > 0 || renderBuffer.myModelsWireframe.Size() > 0)
		{
			myStateManager.SetRasterizerState(RasterizerState::Wireframe);
			myWireframeRenderer.Render(viewCamera, renderBuffer.myModelsWireframe);
			myNavmeshRenderer.Render(viewCamera, renderBuffer.myNavmeshes);
			myStateManager.SetRasterizerState(RasterizerState::Default);
		}

		if (renderBuffer.myRenderFustrum)
		{
			myFrustumRenderer.Render(viewCamera, cullCamera, myStateManager);
		}

		myLineRenderer.Render(viewCamera, renderBuffer.myLines);

		myStateManager.SetSamplerState(SamplerState::Point);
		myStateManager.SetBlendState(BlendState::Alphablend);
		myStateManager.SetDepthStencilState(DepthStencilState::ReadOnly);
		myParticleRenderer.Render(viewCamera, renderBuffer.myParticleEmitters);
		if (renderBuffer.myMap.mySprite != nullptr || renderBuffer.myUseMapCamera)
		{
			myMapRenderer.Render(renderBuffer.myMap);
		}
		mySpriteRenderer.Render(renderBuffer.mySprites, myScreenSize);
		myTextRenderer.Render(renderBuffer.myTexts, myScreenSize);
		myStateManager.SetBlendState(BlendState::Disable);
		myStateManager.SetDepthStencilState(DepthStencilState::Default);
		myStateManager.SetSamplerState(SamplerState::Trilinear);

		myBackbuffer.SetAsActiveTarget();
		myIntermediateTexture.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(FullscreenShaderType::Copy);
	}
	if (myUseVR)
	{
		myGBufferVR.SetAsActiveTarget(&myIntermediateDepthVR);
		myDeferredRenderer.GenerateGBufferInstanced(vrCameraL, renderBuffer.myModelsDeferred, renderBuffer.myModelInstancedStartIndexList);
		myIntermediateTextureVR.SetAsActiveTarget();
		myGBufferVR.SetAllAsResources();
		myStateManager.SetBlendState(BlendState::Disable);
		myDeferredRenderer.RenderEnvLight(vrCameraL, envLight);
		myStateManager.SetBlendState(BlendState::Addativeblend);
		myDeferredRenderer.RenderPointLight(vrCameraL, renderBuffer.myPointLights);

		myStateManager.SetBlendState(BlendState::Alphablend);
		myIntermediateTextureVR.SetAsActiveTarget(&myIntermediateDepthVR);
		myForwardRenderer.Render(vrCameraL, envLight, renderBuffer.myModelsForward, renderBuffer.myModelPointLights);

		if (renderBuffer.myNavmeshes.Size() > 0 || renderBuffer.myModelsWireframe.Size() > 0)
		{
			myStateManager.SetRasterizerState(RasterizerState::Wireframe);
			myWireframeRenderer.Render(viewCamera, renderBuffer.myModelsWireframe);
			myNavmeshRenderer.Render(vrCameraL, renderBuffer.myNavmeshes);
			myStateManager.SetRasterizerState(RasterizerState::Default);
		}

		if (renderBuffer.myRenderFustrum)
		{
			myFrustumRenderer.Render(vrCameraL, cullCamera, myStateManager);
		}

		myLineRenderer.Render(vrCameraL, renderBuffer.myLines);

		if (renderBuffer.myParticleEmitters.Size() > 0)
		{
			myStateManager.SetBlendState(BlendState::Alphablend);
			myStateManager.SetDepthStencilState(DepthStencilState::ReadOnly);
			myParticleRenderer.Render(vrCameraL, renderBuffer.myParticleEmitters);
			myStateManager.SetDepthStencilState(DepthStencilState::Default);
		}
		myStateManager.SetBlendState(BlendState::Disable);

		myLeftEye.SetAsActiveTarget();
		myIntermediateTextureVR.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(FullscreenShaderType::Copy);


		//next eye
		myIntermediateDepthVR.ClearDepth();
		myIntermediateTextureVR.ClearTexture();
		myGBufferVR.ClearTextures(CU::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

		myGBufferVR.SetAsActiveTarget(&myIntermediateDepthVR);
		myDeferredRenderer.GenerateGBufferInstanced(vrCameraR, renderBuffer.myModelsDeferred, renderBuffer.myModelInstancedStartIndexList);
		myIntermediateTextureVR.SetAsActiveTarget();
		myGBufferVR.SetAllAsResources();
		myStateManager.SetBlendState(BlendState::Disable);
		myDeferredRenderer.RenderEnvLight(vrCameraR, envLight);
		myStateManager.SetBlendState(BlendState::Addativeblend);
		myDeferredRenderer.RenderPointLight(vrCameraR, renderBuffer.myPointLights);

		myStateManager.SetBlendState(BlendState::Alphablend);
		myIntermediateTextureVR.SetAsActiveTarget(&myIntermediateDepthVR);
		myForwardRenderer.Render(vrCameraR, envLight, renderBuffer.myModelsForward, renderBuffer.myModelPointLights);

		if (renderBuffer.myNavmeshes.Size() > 0 || renderBuffer.myModelsWireframe.Size() > 0)
		{
			myStateManager.SetRasterizerState(RasterizerState::Wireframe);
			myWireframeRenderer.Render(viewCamera, renderBuffer.myModelsWireframe);
			myNavmeshRenderer.Render(vrCameraR, renderBuffer.myNavmeshes);
			myStateManager.SetRasterizerState(RasterizerState::Default);
		}

		if (renderBuffer.myRenderFustrum)
		{
			myFrustumRenderer.Render(vrCameraR, cullCamera, myStateManager);
		}

		myLineRenderer.Render(vrCameraR, renderBuffer.myLines);

		if (renderBuffer.myParticleEmitters.Size() > 0)
		{
			myStateManager.SetBlendState(BlendState::Alphablend);
			myStateManager.SetDepthStencilState(DepthStencilState::ReadOnly);
			myParticleRenderer.Render(vrCameraR, renderBuffer.myParticleEmitters);
			myStateManager.SetDepthStencilState(DepthStencilState::Default);
		}
		myStateManager.SetBlendState(BlendState::Disable);

		myRightEye.SetAsActiveTarget();
		myIntermediateTextureVR.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(FullscreenShaderType::Copy);

		if (mySettings.VRMirrorEye)
		{
			myBackbuffer.SetAsActiveTarget();
			myIntermediateTextureVR.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(FullscreenShaderType::Copy);
		}

		vr::Texture_t left;
		left.eColorSpace = vr::EColorSpace::ColorSpace_Auto;
		left.eType = vr::ETextureType::TextureType_DirectX;
		left.handle = myLeftEye.myTexture;
		vr::Texture_t right = left;
		right.handle = myRightEye.myTexture;

		vr::TrackedDevicePose_t trackedDevicePoses[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->ClearLastSubmittedFrame();
		vr::VRCompositor()->CompositorBringToFront();

		vr::EVRCompositorError error = vr::EVRCompositorError::VRCompositorError_None;
		error = vr::VRCompositor()->WaitGetPoses(trackedDevicePoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
		if (error != vr::EVRCompositorError::VRCompositorError_None)
		{
			CU::Log("OpenVR submit error: " + std::to_string(static_cast<int>(error)), CU::LogType::Error);
		}
		error = vr::VRCompositor()->Submit(vr::EVREye::Eye_Left, &left);
		if (error != vr::EVRCompositorError::VRCompositorError_None)
		{
			CU::Log("OpenVR submit error: " + std::to_string(static_cast<int>(error)), CU::LogType::Error);
		}
		error = vr::VRCompositor()->Submit(vr::EVREye::Eye_Right, &right);
		if (error != vr::EVRCompositorError::VRCompositorError_None)
		{
			CU::Log("OpenVR submit error: " + std::to_string(static_cast<int>(error)), CU::LogType::Error);
		}
	}
}

void RenderManager::RenderBloom()
{
	myLuminanceTexture.SetAsActiveTarget();
	myIntermediateTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::Luminance);

	myHalfTexture.SetAsActiveTarget();
	myLuminanceTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::Copy);

	myQuarterTexture.SetAsActiveTarget();
	myHalfTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::Copy);

	myBlurTexture1.SetAsActiveTarget();
	myQuarterTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::Copy);

	myBlurTexture2.SetAsActiveTarget();
	myBlurTexture1.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::GaussianHorizontal);

	myBlurTexture1.SetAsActiveTarget();
	myBlurTexture2.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::GaussianVertical);

	myBlurTexture2.SetAsActiveTarget();
	myBlurTexture1.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::GaussianHorizontal);

	myBlurTexture1.SetAsActiveTarget();
	myBlurTexture2.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::GaussianVertical);

	myQuarterTexture.SetAsActiveTarget();
	myBlurTexture1.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::Copy);

	myHalfTexture.SetAsActiveTarget();
	myQuarterTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(FullscreenShaderType::Copy);

	myBackbuffer.SetAsActiveTarget();
	myIntermediateTexture.SetAsResourceOnSlot(0);
	myHalfTexture.SetAsResourceOnSlot(1);
	myFullscreenRenderer.Render(FullscreenShaderType::Bloom);
}
