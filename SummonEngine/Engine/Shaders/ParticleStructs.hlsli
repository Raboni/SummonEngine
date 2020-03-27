cbuffer FrameBuffer : register(b0)
{
	float4x4 toCamera;
	float4x4 toProjection;
};
/*cbuffer ObjectBuffer : register(b1)
{
	float4x4 toWorld;
};*/

struct VertexInput
{
	float4 myPosition : POSITION;
	float4 myDirection : DIRECTION;
	float4 myColor : COLOR;
	float2 mySize : SIZE;
	float myLifeTime : LIFETIME;
	float myTime : TIME;
	float myDistanceToCamera : DISTANCETOCAMERA;
	float mySpeed : SPEED;
};

struct VertexToGeometry
{
	float4 myPosition : POSITION;
	float4 myColor : COLOR;
	float2 mySize : SIZE;
};

struct GeometryToPixel
{
	float4 myPosition : SV_POSITION;
	float4 myColor : COLOR;
	float2 myUV : UV;
};

struct PixelOutput
{
	float4 myColor : SV_TARGET;
};

Texture2D ParticleTexture : register(t0);

SamplerState DefaultSampler : register(s0);