#include "FullscreenStructs.hlsli"
#include "FullscreenResources.hlsli"

float3 Snippet1(const float3 aResource, const float3 aResource2)
{
	return aResource + aResource2;
}
float3 Snippet2(const float3 aResource, const float3 aResource2)
{
	const float resourceMod = 1.0f - saturate(aResource2);

	return aResource * resourceMod + aResource2;
}
float3 Snippet3(float3 aResource, const float3 aResource2)
{
	static const float3 magicFloats = float3(0.2126f, 0.7152f, 0.0722f);
	const float luminance = dot(aResource, magicFloats);
	const float resourceMod = 1.0f - luminance;

	return aResource * resourceMod + aResource2;
}

PixelOutput main(FullscreenPixelInput aInput)
{
	const float3 resource = fullscreenTexture1.Sample(defaultSampler, aInput.myUV.xy).rgb;
	const float3 resource2 = fullscreenTexture2.Sample(defaultSampler, aInput.myUV.xy).rgb;

	PixelOutput output;
	output.myColor.rgb = Snippet2(resource, resource2);
	output.myColor.a = 1.0f;
	return output;
}