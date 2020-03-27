#pragma once
#include "EngineProxy.h"
#include "..\OpenVR\Include\openvr.h"
#include "FullscreenTexture.h"
#include "VRCamera.h"
#include <Vector2.hpp>

class VRManager
{
public:
	VRManager();
	~VRManager();

	bool Init();
	void Update();
	void Quit();

	const CU::Vector2<unsigned int>& GetEyeRenderTargetSize();

private:
	VRCamera myVRCamera;
	CU::Vector2<unsigned int> myRenderTargetSize;
	vr::IVRSystem* myVRSystem;
	vr::IVRChaperone* myVRArea;
	bool myHasSetProjection;
};