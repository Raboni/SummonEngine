cbuffer FrameBuffer : register(b0)
{
	float2 ScreenSize;
	int trash[2];
};

struct VertexInput
{
	float2 myPositionScreen : POSITION_SCREEN;
	float2 mySizeScreen : SIZE_SCREEN;
	float2 myPositionWorld : POSITION_WORLD;
	float2 mySizeWorld : SIZE_WORLD;
	float2 myFocus : FOCUS;
	float myRotation : ROTATION;
	float myZoom : ZOOM;
};

struct VertexToGeometry
{
	float2 myPositionScreen : POSITION_SCREEN;
	float2 mySizeScreen : SIZE_SCREEN;
	float2 myPositionWorld : POSITION_WORLD;
	float2 mySizeWorld : SIZE_WORLD;
	float2 myFocus : FOCUS;
	float myRotation : ROTATION;
	float myZoom : ZOOM;
};

struct GeometryToPixel
{
	float4 myPosition : SV_POSITION;
	float2 myUV : UV;
};

struct PixelOutput
{
	float4 myColor : SV_TARGET;
};

Texture2D MapTexture : register(t0);

SamplerState DefaultSampler : register(s0);