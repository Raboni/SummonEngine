#include "FullscreenStructs.hlsli"
#include "PBRResources.hlsli"
#include "PBRFunctions.hlsli"

float4 Albedo(const float2 aUV);
float3 Normal(const float2 aUV);
float Metalness(const float2 aUV);
float Roughness(const float2 aUV);
float3 WorldPosition(const float2 aUV);

PixelOutput main(const FullscreenPixelInput aInput)
{
	const float3 worldPosition = WorldPosition(aInput.myUV);
	const float4 albedo = Albedo(aInput.myUV);
	const float3 normal = Normal(aInput.myUV);
	const float metalness = Metalness(aInput.myUV);
	const float roughness = Roughness(aInput.myUV);

	const float3 metalnessAlbedo = MetalnessAlbedo(albedo.rgb, metalness);
	const float3 substance = Substance(albedo.rgb, metalness);

	PixelOutput output;
	output.myColor.rgb = CalculatePointLight(myPointLight, worldPosition, normal, metalnessAlbedo, substance, roughness);
	output.myColor.a = 1.0f;
	return output;
}

float4 Albedo(const float2 aUV)
{
	return AlbedoTexture.Sample(DefaultSampler, aUV).rgba;
}
float3 Normal(const float2 aUV)
{
	return NormalTexture.Sample(DefaultSampler, aUV).rgb;
}
float Metalness(const float2 aUV)
{
	return MetalnessTexture.Sample(DefaultSampler, aUV).r;
}
float Roughness(const float2 aUV)
{
	return RoughnessTexture.Sample(DefaultSampler, aUV).r;
}
float3 WorldPosition(const float2 aUV)
{
	return WorldPositionTexture.Sample(DefaultSampler, aUV).rgb;
}