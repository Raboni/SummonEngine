static const float2 resolution = float2(1600.0f, 900.0f) / 4.0f;
static const float gaussianKernel5[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f };
static const float gaussianKernel7[7] = { 0.00578f, 0.060626f, 0.241843f, 0.383103f, 0.241843f, 0.060626f, 0.00578f };
static const float gaussianKernel11[11] = { 0.000003f, 0.000229f, 0.005977f, 0.060598f, 0.24173f, 0.382925f, 0.24173f, 0.060598f, 0.005977f, 0.000229f, 0.000003f };

Texture2D fullscreenTexture1 : register(t0);
Texture2D fullscreenTexture2 : register(t1);

SamplerState defaultSampler : register(s0);

cbuffer FrameBuffer : register(b0)
{
	uint2 myResolution;
	uint2 myScreenSize;
}