#ifndef PBRFunctions
#define PBRFunctions
#include "PBRResources.hlsli"

static const float k0 = 0.00098f;
static const float k1 = 0.9921f;
static const float fakeLysMaxSpecularPower = (2.0f / (0.0014f * 0.0014f)) - 2.0f;
static const float fMaxT = (exp2(-10.0f / sqrt(fakeLysMaxSpecularPower)) - k0) / k1;
float GetSpecPowToMip(float aFSpecPow, int aMipCount)
{
	float fSmulMaxT = (exp2(-10.0f / sqrt(aFSpecPow)) - k0) / k1;
	return float(aMipCount - 1 - 0) * (1.0f - clamp(fSmulMaxT / fMaxT, 0.0f, 1.0f));
}
float RoughToSPow(float aFRoughness)
{
	return (2.0f / (aFRoughness * aFRoughness)) - 2.0f;
}

float3 MetalnessAlbedo(const float3 aAlbedo, const float aMetalness);
float3 Substance(const float3 aAlbedo, const float aMetalness);

float3 ReflectionFresnel(const float3 aSubstance, const float3 aNormal, const float3 aWorldPosition, const float aRoughness);
float3 Fresnel(const float3 aSubstance, const float3 aWorldPosition, const float3 aToLight);
float3 Distribution(const float3 aNormal, const float aRoughness, const float3 aWorldPosition, const float3 aToLight);
float Visibility(const float3 aNormal, const float aRoughness, const float3 aWorldPosition, const float3 aToLight);
float Lambert(const float3 aNormal);

float3 AmbientDiffuse(const float3 aNormal, const float3 aMetalnessAlbedo, const float3 aFresnel, const float aAmbientOcclusion);
float3 AmbientSpecularity(const float3 aWorldPosition, const float3 aNormal, const float3 aFresnel, const float aRoughness, const float aAmbientOcclusion);
float3 DirectDiffuse(const float3 aMetalnessAlbedo, const float3 aFresnel, const float aLambert);
float3 DirectSpecularity(const float3 aFresnel, const float3 aDistribution, const float aLambert, const float aVisibility);

float3 CalculatePointLight(PointLight aPointLight, const float3 aWorldPosition, const float3 aNormal, const float3 aMetalnessAlbedo, const float3 aSubstance, const float aRoughness);
float Wireframe(const float3 aBaryCoordinates);


float3 MetalnessAlbedo(const float3 aAlbedo, const float aMetalness)
{
	const float3 albedo = aAlbedo.rgb;
	const float3 metalnessAlbedo = albedo - (albedo * aMetalness);

	return metalnessAlbedo;
}
float3 Substance(const float3 aAlbedo, const float aMetalness)
{
	const float3 magicFloat = float3(0.04f, 0.04f, 0.04f);
	const float3 substance = (magicFloat - (magicFloat * aMetalness.rrr)) + aAlbedo.rgb * aMetalness;

	return substance;
}

float3 ReflectionFresnel(const float3 aSubstance, const float3 aNormal, const float3 aWorldPosition, const float aRoughness)
{
	const float3 toEye = normalize(cameraPosition.xyz - aWorldPosition);

	float3 VdotN = dot(toEye.xyz, aNormal);
	VdotN = 1.0f - saturate(VdotN);
	VdotN = pow(VdotN, 5);

	float3 fresnel = VdotN * (1.0f - aSubstance);
	fresnel = fresnel / (6 - 5 * aRoughness);
	fresnel = fresnel + aSubstance;
	return fresnel;
}
float3 Fresnel(const float3 aSubstance, const float3 aWorldPosition, const float3 aToLight)
{
	const float3 toLight = aToLight.xyz;
	const float3 toEye = normalize(cameraPosition.xyz - aWorldPosition);
	const float3 halfvector = normalize(toLight.xyz + toEye.xyz);

	float LdotH = dot(toLight.xyz, halfvector.xyz);
	LdotH = 1.0f - saturate(LdotH);
	LdotH = pow(LdotH, 5);
	float3 fresnel = LdotH * (1.0f - aSubstance);
	fresnel = fresnel + aSubstance;

	return fresnel;
}
float3 Distribution(const float3 aNormal, const float aRoughness, const float3 aWorldPosition, const float3 aToLight)
{
	const float3 toLight = aToLight.xyz;

	const float3 toEye = normalize(cameraPosition.xyz - aWorldPosition);
	const float3 halfvector = normalize(toLight.xyz + toEye.xyz);

	const float HdotN = saturate(dot(halfvector, aNormal));
	const float m = aRoughness * aRoughness;
	const float m2 = m * m;
	const float denominator = HdotN * HdotN * (m2 - 1.0f) + 1.0f;
	const float distribution = m2 / (3.1415f * denominator * denominator);

	return distribution;
}
float Visibility(const float3 aNormal, const float aRoughness, const float3 aWorldPosition, const float3 aToLight)
{
	const float3 toLight = aToLight.xyz;

	const float3 toEye = normalize(cameraPosition.xyz - aWorldPosition);

	const float roughnessRemapped = (aRoughness + 1.0f) / 2.0f;

	const float NdotL = saturate(dot(aNormal, toLight));
	const float NdotV = saturate(dot(aNormal, toEye));

	const float k = roughnessRemapped * roughnessRemapped * sqrt(3.1415f);
	const float G1V = NdotV * (1.0f - k) + k;
	const float G1L = NdotL * (1.0f - k) + k;
	const float visibility = (NdotV * NdotL) / (G1V * G1L);

	return visibility;
}
float Lambert(const float3 aNormal)
{
	return saturate(dot(toDirectionalLight.xyz, aNormal));
}

float3 AmbientDiffuse(const float3 aNormal, const float3 aMetalnessAlbedo, const float3 aFresnel, const float aAmbientOcclusion)
{
	const float3 enviromentLight = CubemapTexture.SampleLevel(DefaultSampler, aNormal, enviromentLightMipCount - 2).rgb;

	return aMetalnessAlbedo * enviromentLight * (float3(1.0f, 1.0f, 1.0f) - aFresnel) * aAmbientOcclusion;
}
float3 AmbientSpecularity(const float3 aWorldPosition, const float3 aNormal, const float3 aFresnel, const float aRoughness, const float aAmbientOcclusion)
{
	const float3 toEye = normalize(cameraPosition.xyz - aWorldPosition);
	const float3 reflectionVector = -reflect(toEye, aNormal);

	const float fakeLysSpecularPower = RoughToSPow(aRoughness);
	const float lysMipMap = GetSpecPowToMip(fakeLysSpecularPower, enviromentLightMipCount);

	const float3 enviromentLight = CubemapTexture.SampleLevel(DefaultSampler, reflectionVector, lysMipMap).rgb;

	return enviromentLight * aFresnel * aAmbientOcclusion;
}

float3 DirectDiffuse(const float3 aMetalnessAlbedo, const float3 aFresnel, const float aLambert)
{
	const float3 lightColor = directionalLightColor.rgb;

	return aMetalnessAlbedo * lightColor * (float3(1.0f, 1.0f, 1.0f) - aFresnel) * aLambert;
}
float3 DirectSpecularity(const float3 aFresnel, const float3 aDistribution, const float aLambert, const float aVisibility)
{
	const float3 lightColor = directionalLightColor.rgb;

	return lightColor * aFresnel * aDistribution * aLambert * aVisibility;
}

float3 CalculatePointLight(PointLight aPointLight, const float3 aWorldPosition, const float3 aNormal, const float3 aMetalnessAlbedo, const float3 aSubstance, const float aRoughness)
{
	const float3 lightColor = aPointLight.myColor.rgb;
	float3 toLight = aPointLight.myPosition.xyz - aWorldPosition;
	const float toLightDistance = length(toLight);
	toLight = normalize(toLight);

	const float3 fresnel = Fresnel(aSubstance, aWorldPosition, toLight);
	const float3 distribution = Distribution(aNormal, aRoughness, aWorldPosition, toLight);
	const float visibility = Visibility(aNormal, aRoughness, aWorldPosition, toLight);

	const float lambertAttenuation = saturate(dot(aNormal, toLight));
	const float linearAttenuation = saturate(1.0f - (toLightDistance / aPointLight.myRange));
	const float physicalAttenuation = 1.0f / (toLightDistance * toLightDistance);
	const float attenuation = lambertAttenuation * linearAttenuation * physicalAttenuation;

	const float3 diffuse = aMetalnessAlbedo * lightColor * (float3(1.0f, 1.0f, 1.0f) - fresnel) * attenuation;
	const float3 specularity = lightColor * fresnel * distribution * attenuation * visibility;

	return diffuse + specularity;
}

float Wireframe(const float3 aBaryCoordinates)
{
	//https://catlikecoding.com/unity/tutorials/advanced-rendering/flat-and-wireframe-shading/
	float3 barys = aBaryCoordinates;
	//barys.z = 1 - barys.x - barys.y;
	const float3 deltas = fwidth(barys);
	const float3 smoothing = deltas * 1.0f;
	const float3 thickness = deltas * 0.5f;
	barys = smoothstep(thickness, thickness + smoothing, barys);
	const float minBary = min(barys.x, min(barys.y, barys.z));
	return minBary;
}
#endif