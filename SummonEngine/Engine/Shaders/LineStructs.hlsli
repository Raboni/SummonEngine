struct VertexInput
{
	float4 myPosition : POSITION;
};
struct VertexToPixel
{
	float4 myPosition : SV_POSITION;
};
struct PixelOutput
{
	float4 myColor : SV_TARGET;
};