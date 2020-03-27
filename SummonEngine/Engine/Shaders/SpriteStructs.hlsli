cbuffer FrameBuffer : register(b0)
{
	float2 ScreenSize;
	int trash[2];
};
/*cbuffer ObjectBuffer : register(b1)
{
	float4x4 toWorld;
};*/

struct VertexInput
{
	float4 myColor : COLOR;
	float2 myPosition : POSITION;
	float2 mySize : SIZE;
	float2 myPivot : PIVOT;
	float myRotation : ROTATION;
};

struct VertexToGeometry
{
	float4 myColor : COLOR;
	float2 myPosition : POSITION;
	float2 mySize : SIZE;
	float2 myPivot : PIVOT;
	float myRotation : ROTATION;
};

struct GeometryToPixel
{
	float4 myColor : COLOR;
	float4 myPosition : SV_POSITION;
	float2 myUV : UV;
};

struct PixelOutput
{
	float4 myColor : SV_TARGET;
};

Texture2D SpriteTexture : register(t0);

SamplerState DefaultSampler : register(s0);