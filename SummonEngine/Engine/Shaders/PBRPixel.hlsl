#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"
#include "PBRFunctions.hlsli"

cbuffer PointLightStructBuffer : register(b4)
{
	PointLight PointLightList[8];
	unsigned int PointLightCount;
	unsigned int Trash[3];
};

float4 Albedo(const float2 aUV);
//float4 AlbedoWireframe(const float3 aBaryCoordinates, const float2 aUV);
float3 Normal(const PBRPixelInput aInput);
float Metalness(const float2 aUV);
float Roughness(const float2 aUV);
float AmbientOcclusion(const float2 aUV);
float3 Emissive(const float2 aUV);

PixelOutput main(const PBRPixelInput aInput)
{
	const float3 worldPosition = aInput.myWorldPosition.xyz;
	const float4 albedo = Albedo(aInput.myUV);
	const float3 normal = Normal(aInput);
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

	float3 color = ambientDiffuse + ambientSpecularity + directDiffuse + directSpecularity + emissive;
	for (unsigned int pointLightIndex = 0; pointLightIndex < PointLightCount; pointLightIndex++)
	{
		color = color + CalculatePointLight(PointLightList[pointLightIndex], worldPosition, normal, metalnessAlbedo, substance, roughness);
	}
	PixelOutput output;
	output.myColor.rgb = color * Wireframe(aInput.myBaryCoordinates.xyz);
	output.myColor.a = albedo.a;
	output.myColor *= modelColor;
	return output;
}

float4 Albedo(const float2 aUV)
{
	return AlbedoTexture.Sample(DefaultSampler, aUV).rgba;
}
//float4 AlbedoWireframe(const float3 aBaryCoordinates, const float2 aUV)
//{
//	const float4 albedo = Albedo(aUV);
//	//https://catlikecoding.com/unity/tutorials/advanced-rendering/flat-and-wireframe-shading/
//	float3 barys = aBaryCoordinates;
//	//barys.z = 1 - barys.x - barys.y;
//	const float3 deltas = fwidth(barys);
//	const float3 smoothing = deltas * 1.0f;
//	const float3 thickness = deltas * 0.5f;
//	barys = smoothstep(thickness, thickness + smoothing, barys);
//	const float minBary = min(barys.x, min(barys.y, barys.z));
//	return float4(albedo.rgb * minBary, albedo.a);
//}
float3 Normal(const PBRPixelInput aInput)
{
	//return NormalTexture.Sample(DefaultSampler, aInput.myUV.xy).xyz;
	float3 textureNormal = NormalTexture.Sample(DefaultSampler, aInput.myUV.xy).rgb;
	textureNormal = (textureNormal * 2) - 1;

	float3x3 tangentSpace = float3x3(normalize(aInput.myTangent.xyz), normalize(aInput.myBinormal.xyz), normalize(aInput.myNormal.xyz));
	tangentSpace = transpose(tangentSpace);
	float3 pixelNormal = mul(tangentSpace, textureNormal.xyz);
	pixelNormal = normalize(pixelNormal);

	return pixelNormal.xyz;
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