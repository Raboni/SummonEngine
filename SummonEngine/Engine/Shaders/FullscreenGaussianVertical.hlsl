#include "FullscreenStructs.hlsli"
#include "FullscreenResources.hlsli"

PixelOutput main(const FullscreenPixelInput aInput)
{
	static const unsigned int kernelSize = 11;
	static const float texelSize = 1.0f / resolution.x;
	static const float start = ((float)(kernelSize) - 1.0f) * -0.5f;

	float3 blurColor = float3(0.0f, 0.0f, 0.0f);
	for (unsigned int index = 0; index < kernelSize; index++)
	{
		const float2 uv = aInput.myUV.xy + float2(0.0f, texelSize * (start + index));
		const float3 resource = fullscreenTexture1.Sample(defaultSampler, uv).rgb;
		blurColor += resource * gaussianKernel11[index];
	}

	PixelOutput output;
	output.myColor.rgb = blurColor;
	output.myColor.a = 1.0f;
	return output;
}