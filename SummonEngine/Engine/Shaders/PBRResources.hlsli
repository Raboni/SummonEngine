#ifndef PBRResources
#define PBRResources

struct PointLight
{
	float4 myPosition;
	float3 myColor;
	float myRange;
};

cbuffer FrameBuffer : register(b0)
{
	float4x4 toCamera;
	float4x4 toProjection;
	float4 cameraPosition;
};
cbuffer ObjectBuffer : register(b1)
{
	float4x4 toWorld;
	float4 modelScale;
	float4 modelColor;
};
cbuffer EnviromentLightBuffer : register(b2)
{
	float4 toDirectionalLight;
	float4 directionalLightColor;
	unsigned int enviromentLightMipCount;
	unsigned int frameReserved[3];
};
cbuffer PointLightBuffer : register(b3)
{
	PointLight myPointLight;
};
cbuffer AnimBoneBuffer : register(b4)
{
	float4x4 myBoneTransform[100];
}

TextureCube CubemapTexture : register(t0);
Texture2D AlbedoTexture : register(t1);
Texture2D NormalTexture : register(t2);
Texture2D MetalnessTexture : register(t3);
Texture2D RoughnessTexture : register(t4);
Texture2D AmbientOcclusionTexture : register(t5);
Texture2D EmissiveTexture : register(t6);
Texture2D WorldPositionTexture : register(t7);
Texture2D AnimationDataTexture : register(t8);

SamplerState DefaultSampler : register(s0);
#endif