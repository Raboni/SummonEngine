#include "FullscreenStructs.hlsli"
#include "FullscreenResources.hlsli"

static const float3 magicFloats = float3(0.2126f, 0.7152f, 0.0722f);
static const float cutoff = 0.8f;

float3 Snippet1(const float3 aResource)
{
	const float luminance = dot(aResource, magicFloats);

	return aResource * pow(luminance, 5);
}
float3 Snippet2(const float3 aResource)
{
	const float luminance = dot(aResource, magicFloats);

	if (luminance >= cutoff)
	{
		return aResource;
	}
	else
	{
		return float3(0.0f, 0.0f, 0.0f);
	}
}
float3 Snippet3(const float3 aResource)
{
	float luminance = dot(aResource, magicFloats);

	luminance = saturate(luminance - cutoff);
	return aResource * luminance * (1.0f / cutoff);
}
float3 Snippet4(const float3 aResource)
{
	const float luminance = dot(aResource, magicFloats);

	if (luminance >= cutoff)
	{
		return aResource;
	}
	else if (luminance >= cutoff * 0.5f)
	{
		float fade = luminance / cutoff;
		fade = pow(fade, 5);
		return aResource * fade;
	}
	else
	{
		return float3(0.0f, 0.0f, 0.0f);
	}
}

PixelOutput main(const FullscreenPixelInput aInput)
{
	const float3 resource = fullscreenTexture1.Sample(defaultSampler, aInput.myUV.xy).rgb;

	PixelOutput output;
	output.myColor.rgb = Snippet1(resource);
	output.myColor.a = 1.0f;
	return output;
}