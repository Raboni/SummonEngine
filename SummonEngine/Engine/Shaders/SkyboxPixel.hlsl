#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"

PixelOutput main(const SkyboxPixelInput aInput)
{
	const float3 textureColor = CubemapTexture.Sample(DefaultSampler, aInput.myTexCoord.xyz).rgb * modelColor.rgb;

	PixelOutput output;
	output.myColor = float4(textureColor, 1.0f);
	return output;
}