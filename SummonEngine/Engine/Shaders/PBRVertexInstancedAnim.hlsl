#include "GBufferStructs.hlsli"
#include "PBRResources.hlsli"

float4x4 CreateMatrixFromCols(const float4 c0, const float4 c1, const float4 c2, const float4 c3)
{
	return float4x4(c0.x, c1.x, c2.x, c3.x,
		c0.y, c1.y, c2.y, c3.y,
		c0.z, c1.z, c2.z, c3.z,
		c0.w, c1.w, c2.w, c3.w);
}
float4x4 LoadBoneMatrix(const uint3 aAnimationData, const float aBlendPercent, const int aBone)
{
	uint frameStartIndex = aAnimationData.x + aAnimationData.z;
	uint frameDeltaStartIndex = aAnimationData.y + aAnimationData.z;
	// We use 4 * bone because each bone is 4 texels to form a float4x4.
	frameStartIndex += (4 * aBone);
	frameDeltaStartIndex += (4 * aBone);

	const uint frameStartU = frameStartIndex % 512;
	const uint frameStartV = frameStartIndex / 512;
	const uint frameDeltaStartU = frameDeltaStartIndex % 512;
	const uint frameDeltaStartV = frameDeltaStartIndex / 512;

	// Note that we assume the width of the texture
	 // is an even multiple of the number of texels per bone;
	 // otherwise we'd have to recalculate the V component per lookup.
	const float4 mat1 = AnimationDataTexture.Load(uint3(frameStartU, frameStartV, 0));
	const float4 mat2 = AnimationDataTexture.Load(uint3(frameStartU + 1, frameStartV, 0));
	const float4 mat3 = AnimationDataTexture.Load(uint3(frameStartU + 2, frameStartV, 0));
	const float4 mat4 = AnimationDataTexture.Load(uint3(frameStartU + 3, frameStartV, 0));

	const float4 mat1Delta = AnimationDataTexture.Load(uint3(frameDeltaStartU, frameDeltaStartV, 0));
	const float4 mat2Delta = AnimationDataTexture.Load(uint3(frameDeltaStartU + 1, frameDeltaStartV, 0));
	const float4 mat3Delta = AnimationDataTexture.Load(uint3(frameDeltaStartU + 2, frameDeltaStartV, 0));
	const float4 mat4Delta = AnimationDataTexture.Load(uint3(frameDeltaStartU + 3, frameDeltaStartV, 0));

	// Only load 3 of the 4 values, and decode the matrix from them.
	//float4x4 rval = decodeMatrix(float3x4(mat1, mat2, mat3));
	return CreateMatrixFromCols(mat1 + mat1Delta * aBlendPercent, mat2 + mat2Delta * aBlendPercent, mat3 + mat3Delta * aBlendPercent, mat4 + mat4Delta * aBlendPercent);
}
/*VS_to_PS CharacterAnimatedInstancedVS(A_to_VS input)
{
	VS_to_PS output;
	uint4 animationData = g_Instances[input.InstanceId].animationData;
	// Our per instance data is stored in constants
	float4 worldMatrix1 = g_Instances[input.InstanceId].world1;
	float4 worldMatrix2 = g_Instances[input.InstanceId].world2;
	float4 worldMatrix3 = g_Instances[input.InstanceId].world3;
	float4 instanceColor = g_Instances[input.InstanceId].color;

	float4x4 finalMatrix;
	// Load the first and most influential bone weight.
	finalMatrix = input.vWeights.x * loadBoneMatrix(animationData, input.vBones.x);
	// Conditionally apply subsequent bone matrices if the weight is > 0.
	if (input.vWeights.y > 0)
	{
		finalMatrix += input.vWeights.y * loadBoneMatrix(animationData, input.vBones.y);
		if (input.vWeights.z > 0)
		{
			finalMatrix += input.vWeights.z * loadBoneMatrix(animationData, input.vBones.z);
			if (input.vWeights.w > 0)
			{
				finalMatrix += input.vWeights.w * loadBoneMatrix(animationData, input.vBones.w);
			}
		}
	}
}*/

PBRPixelInput main(PBRVertexInputInstancedAnim aInput)
{
	//float4 vertexObjectPos = float4(aInput.myPosition.xyz, 1.0f);
	/*float4x4 matX = LoadBoneMatrix(aInput.myAnimationData.xy, aInput.myBoneIDs.x);
	float4x4 matY = LoadBoneMatrix(aInput.myAnimationData.xy, aInput.myBoneIDs.y);
	float4x4 matZ = LoadBoneMatrix(aInput.myAnimationData.xy, aInput.myBoneIDs.z);
	float4x4 matW = LoadBoneMatrix(aInput.myAnimationData.xy, aInput.myBoneIDs.w);
	vertexObjectPos = mul(matX, vertexObjectPos) * aInput.myWeights.x +
		mul(matY, vertexObjectPos) * aInput.myWeights.y +
		mul(matZ, vertexObjectPos) * aInput.myWeights.z +
		mul(matW, vertexObjectPos) * aInput.myWeights.w;*/
	float4x4 finalMatrix = aInput.myWeights.x * LoadBoneMatrix(aInput.myAnimationData, aInput.myBlendPercent, aInput.myBoneIDs.x);
	if (aInput.myWeights.y > 0)
	{
		finalMatrix += aInput.myWeights.y * LoadBoneMatrix(aInput.myAnimationData, aInput.myBlendPercent, aInput.myBoneIDs.y);
		if (aInput.myWeights.z > 0)
		{
			finalMatrix += aInput.myWeights.z * LoadBoneMatrix(aInput.myAnimationData, aInput.myBlendPercent, aInput.myBoneIDs.z);
			if (aInput.myWeights.w > 0)
			{
				finalMatrix += aInput.myWeights.w * LoadBoneMatrix(aInput.myAnimationData, aInput.myBlendPercent, aInput.myBoneIDs.w);
			}
		}
	}
	float4 vertexObjectPos = mul(finalMatrix, float4(aInput.myPosition.xyz, 1.0f));
	vertexObjectPos = float4(vertexObjectPos.xyz * aInput.myModelScale.xyz, 1.0f);

	const float4 vertexWorldPos = mul(aInput.myModelMatrix, vertexObjectPos);
	const float4 vertexViewPos = mul(toCamera, vertexWorldPos);
	const float4 vertexProjectionPos = mul(toProjection, vertexViewPos);

	const float3x3 toWorldRotation = (float3x3)aInput.myModelMatrix;
	const float3 vertexWorldNormal = mul(toWorldRotation, aInput.myNormal.xyz);
	const float3 vertexWorldTangent = mul(toWorldRotation, aInput.myTangent.xyz);
	const float3 vertexWorldBinormal = mul(toWorldRotation, aInput.myBinormal.xyz);

	PBRPixelInput output;
	output.myWorldPosition = vertexWorldPos;
	output.myPosition = vertexProjectionPos;
	output.myNormal = float4(vertexWorldNormal, 0);
	output.myTangent = float4(vertexWorldTangent, 0);
	output.myBinormal = float4(vertexWorldBinormal, 0);
	output.myUV = aInput.myUV;
	output.myModelColor = aInput.myModelColor;
	output.myBaryCoordinates = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return output;
}