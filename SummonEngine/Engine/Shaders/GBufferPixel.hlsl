#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"
#include "PBRFunctions.hlsli"

GBufferOutput main(const PBRPixelInput aInput)
{
	const float3 albedo = AlbedoTexture.Sample(DefaultSampler, aInput.myUV.xy).rgb;

	//const float3 normal = Normal(aInput).xyz;
	float3 normal = NormalTexture.Sample(DefaultSampler, aInput.myUV.xy).rgb;
	normal = (normal * 2) - 1;
	float3x3 tangentspaceMatrix = float3x3(normalize(aInput.myTangent.xyz), normalize(aInput.myBinormal.xyz), normalize(aInput.myNormal.xyz));
	tangentspaceMatrix = transpose(tangentspaceMatrix);
	normal = mul(tangentspaceMatrix, normal);

	const float metalness = MetalnessTexture.Sample(DefaultSampler, aInput.myUV.xy).r;
	const float roughness = RoughnessTexture.Sample(DefaultSampler, aInput.myUV.xy).r;
	const float ambient = AmbientOcclusionTexture.Sample(DefaultSampler, aInput.myUV.xy).r;
	const float3 emissive = EmissiveTexture.Sample(DefaultSampler, aInput.myUV.xy).rgb;

	GBufferOutput output;
	output.myAlbedo = float4(albedo * aInput.myModelColor.rgb * Wireframe(aInput.myBaryCoordinates.xyz), 1.0f);
	output.myNormal = float4(normal, 0.0f);
	output.myMetalness = metalness;
	output.myRoughness = roughness;
	output.myAmbientOcclusion = ambient;
	output.myEmissive = float4(emissive, 1.0f);
	output.myWorldPosition = aInput.myWorldPosition;
	return output;
}