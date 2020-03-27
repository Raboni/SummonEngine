cbuffer FrameBuffer : register(b0)
{
	float2 ScreenSize;
	float2 trash;
};
cbuffer ObjectBuffer : register(b1)
{
	float4 ObjectColor;
	float4 ObjectPosition;
	float2 ObjectSize;
	float2 ObjectScale;
	float2 ObjectPivot;
	float2 ObjectTrash;
};

struct VertexInput
{
	float4 myPosition : POSITION;
	float2 myUV : UV;
};
struct VertexToPixel
{
	float4 myPosition : SV_POSITION;
	float2 myUV : UV;
};
struct PixelOutput
{
	float4 myColor : SV_TARGET;
};

Texture2D FontTexture : register(t0);
SamplerState DefaultSampler : register(s0);