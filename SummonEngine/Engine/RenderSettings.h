#pragma once

struct RenderSettings
{
	float DrawDistance = 400.0f;
	float MapRange = 200.0f;
	bool Fullscreen = false;
	bool VSync = false;
	bool FXAA = true;
	bool VRMirrorEye = true;
	bool UseDeferredContexts = false;
	bool ThreadCulling = true;
};