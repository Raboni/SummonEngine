#ifndef FullscreenStructs
#define FullscreenStructs

struct FullscreenVertexInput
{
	unsigned int myIndex : SV_VertexID;
};

struct FullscreenPixelInput
{
	float4 myPosition : SV_POSITION;
	float2 myUV : UV;
};

struct PixelOutput
{
	float4 myColor : SV_TARGET;
};
#endif