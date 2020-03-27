#ifndef GBufferStructs
#define GBufferStructs

struct PBRVertexInput
{
	float4 myPosition : POSITION;
	float4 myNormal : NORMAL;
	float4 myTangent : TANGENT;
	float4 myBinormal : BINORMAL;
	float2 myUV : UV;
};
struct PBRVertexInputInstanced
{
	float4 myPosition : POSITION;
	float4 myNormal : NORMAL;
	float4 myTangent : TANGENT;
	float4 myBinormal : BINORMAL;
	float2 myUV : UV;
	float4x4 myModelMatrix : MODELMATRIX;
	float4 myModelScale : MODELSCALE;
	float4 myModelColor : MODELCOLOR;
};
struct PBRVertexInputAnim
{
	float4 myPosition : POSITION;
	float4 myNormal : NORMAL;
	float4 myTangent : TANGENT;
	float4 myBinormal : BINORMAL;
	float2 myUV : UV;
	int4 myBoneIDs : BONEID;
	float4 myWeights : BONEWEIGHT;
};
struct PBRVertexInputInstancedAnim
{
	float4 myPosition : POSITION;
	float4 myNormal : NORMAL;
	float4 myTangent : TANGENT;
	float4 myBinormal : BINORMAL;
	float2 myUV : UV;
	int4 myBoneIDs : BONEID;
	float4 myWeights : BONEWEIGHT;
	float4x4 myModelMatrix : MODELMATRIX;
	float4 myModelScale : MODELSCALE;
	float4 myModelColor : MODELCOLOR;
	uint3 myAnimationData : ANIMDATA;
	float myBlendPercent : BLEND;
};
struct PBRPixelInput
{
	float4 myWorldPosition : WORLDPOSITION;
	float4 myPosition : SV_POSITION;
	float4 myNormal : NORMAL;
	float4 myTangent : TANGENT;
	float4 myBinormal : BINORMAL;
	float2 myUV : UV;
	float4 myModelColor : MODELCOLOR;
	float4 myBaryCoordinates : BARYS;
};
struct PixelOutput
{
	float4 myColor : SV_TARGET;
};
struct GBufferOutput
{
	float4 myAlbedo : SV_TARGET0;
	float4 myNormal : SV_TARGET1;
	float myMetalness : SV_TARGET2;
	float myRoughness : SV_TARGET3;
	float myAmbientOcclusion : SV_TARGET4;
	float4 myEmissive : SV_TARGET5;
	float4 myWorldPosition : SV_TARGET6;
};

struct SkyboxVertexInput
{
	float4 myPosition : POSITION;
};
struct SkyboxPixelInput
{
	float4 myPosition : SV_POSITION;
	float4 myTexCoord : TEXCOORD;
};
#endif