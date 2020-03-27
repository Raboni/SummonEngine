#include "FullscreenStructs.hlsli"
#include "PBRResources.hlsli"
#include "PBRFunctions.hlsli"

float4 Albedo(const float2 aUV);
float3 Normal(const float2 aUV);
float Metalness(const float2 aUV);
float Roughness(const float2 aUV);
float AmbientOcclusion(const float2 aUV);
float3 Emissive(const float2 aUV);
float3 WorldPosition(const float2 aUV);

PixelOutput main(const FullscreenPixelInput aInput)
{
	const float3 worldPosition = WorldPosition(aInput.myUV);
	const float4 albedo = Albedo(aInput.myUV);
	const float3 normal = Normal(aInput.myUV);
	const float metalness = Metalness(aInput.myUV);
	const float roughness = Roughness(aInput.myUV);
	const float ambientOcclusion = AmbientOcclusion(aInput.myUV);
	const float3 emissive = Emissive(aInput.myUV);

	const float3 metalnessAlbedo = MetalnessAlbedo(albedo.rgb, metalness);
	const float3 substance = Substance(albedo.rgb, metalness);

	const float3 fresnel = Fresnel(substance, worldPosition, toDirectionalLight.xyz);
	const float3 distribution = Distribution(normal, roughness, worldPosition, toDirectionalLight.xyz);
	const float visibility = Visibility(normal, roughness, worldPosition, toDirectionalLight.xyz);
	const float lambert = Lambert(normal);

	const float3 ambientDiffuse = AmbientDiffuse(normal, metalnessAlbedo, fresnel, ambientOcclusion);
	const float3 ambientSpecularity = AmbientSpecularity(worldPosition, normal, fresnel, roughness, ambientOcclusion);
	const float3 directDiffuse = DirectDiffuse(metalnessAlbedo, fresnel, lambert);
	const float3 directSpecularity = DirectSpecularity(fresnel, distribution, lambert, visibility);

	PixelOutput output;
	output.myColor.rgb = ambientDiffuse + ambientSpecularity + directDiffuse + directSpecularity + emissive;
	output.myColor.a = 1.0f;
	return output;
}

float4 Albedo(const float2 aUV)
{
	return AlbedoTexture.Sample(DefaultSampler, aUV).rgba;
}
float3 Normal(const float2 aUV)
{
	return NormalTexture.Sample(DefaultSampler, aUV).xyz;
}
float Metalness(const float2 aUV)
{
	return MetalnessTexture.Sample(DefaultSampler, aUV).r;
}
float Roughness(const float2 aUV)
{
	return RoughnessTexture.Sample(DefaultSampler, aUV).r;
}
float AmbientOcclusion(const float2 aUV)
{
	return AmbientOcclusionTexture.Sample(DefaultSampler, aUV).r;
}
float3 Emissive(const float2 aUV)
{
	return EmissiveTexture.Sample(DefaultSampler, aUV).rgb;
}
float3 WorldPosition(const float2 aUV)
{
	return WorldPositionTexture.Sample(DefaultSampler, aUV).xyz;
}