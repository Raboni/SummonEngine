#include "FullscreenStructs.hlsli"
#include "FullscreenResources.hlsli"

SamplerState ansitropic8XSampler : register(s1);

float rgb2luma(const float3 rgb)
{
	return sqrt(dot(rgb, float3(0.299, 0.587, 0.114)));
}

float3 FXaaTextureOffset(const float2 aPos, const float2 aDir, const float2 aD)
{
	const float2 temp = float2(aDir.x * aD.x, aDir.y * aD.y);
	return fullscreenTexture1.Sample(defaultSampler, aPos + temp).rgb;
}

float QUALITY(const int aIndex) //EWH
{
	if (aIndex > 5)
	{
		if (aIndex == 6)
		{
			return 1.5f;
		}
		if (6 < aIndex && aIndex < 11)
		{
			return 2.0f;
		}
		if (aIndex == 11)
		{
			return 4.0f;
		}
		if (aIndex == 12)
		{
			return 8.0f;
		}
	}
	return 1.0f;
}

PixelOutput main(const FullscreenPixelInput aInput)
{
	static const float EDGE_THRESHOLD_MIN = 1.0f / 16.0f; //0.0625f; //0.0312f; //1/32 visible limit, 1/16 high Q, 1/12 upper limit
	static const float EDGE_THRESHOLD_MAX = 1.0f / 8.0f; //"1/3 too little, 1/4 low quality, 1/8 high quality, 1/16 overkill"
	static const int ITERATIONS = 16;
	static const float SUBPIXEL_QUALITY = 0.75f;

	PixelOutput output;

	const float4 colorCenter = fullscreenTexture1.Sample(defaultSampler, aInput.myUV.xy).rgba;

	const float2 dXdY = float2(1.0f / myResolution.x, 1.0f / myResolution.y);

	// Luma at the four direct neighbours of the current fragment.
	const float lumaCenter = rgb2luma(colorCenter.rgb);
	const float lumaDown = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(0.0f, -1.0f), dXdY).rgb);
	const float lumaUp = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(0.0f, 1.0f), dXdY).rgb);
	const float lumaLeft = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(-1.0f, 0.0f), dXdY).rgb);
	const float lumaRight = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(1.0f, 0.0f), dXdY).rgb);

	// Find the maximum and minimum luma around the current fragment.
	const float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
    const float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

	// Compute the delta.
	const float lumaRange = lumaMax - lumaMin;

	// If the luma variation is lower that a threshold (or if we are in a really dark area), we are not on an edge, don't perform any AA.
	if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX))
	{
		output.myColor.rgb = colorCenter.rgb;
		output.myColor.a = 1.0f;
		return output;
	}


	const float lumaDownLeft = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(-1.0f, -1.0f), dXdY).rgb);
	const float lumaUpRight = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(1.0f, 1.0f), dXdY).rgb);
	const float lumaUpLeft = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(-1.0f, 1.0f), dXdY).rgb);
	const float lumaDownRight = rgb2luma(FXaaTextureOffset(aInput.myUV, float2(1.0f, -1.0f), dXdY).rgb);

	const float lumaDownUp = lumaDown + lumaUp;
	const float lumaLeftRight = lumaLeft + lumaRight;

	const float lumaLeftCornerns = lumaDownLeft + lumaUpLeft;
	const float lumaDownCorners = lumaDownLeft + lumaDownRight;
	const float lumaRightCorners = lumaDownRight + lumaUpRight;
	const float lumaUpCorners = lumaUpRight + lumaUpLeft;

	//|(upleft - left) - (left - downleft)| = abs(-2.0f * lumaLeft + lumaLeftCornerns)
	//link
	const float edgeHorizontal = abs(-2.0f * lumaLeft + lumaLeftCornerns) + abs(-2.0f * lumaCenter + lumaDownUp) * 2.0f + abs(-2.0f * lumaRight + lumaRightCorners);
	const float edgeVertical = abs(-2.0f * lumaUp + lumaUpCorners) + abs(-2.0f * lumaCenter + lumaLeftRight) * 2.0f + abs(-2.0f * lumaDown + lumaDownCorners);


	//Nivida whitepaper
	//float edgeVertical = abs((0.25f * lumaUpLeft) + (-0.5 * lumaUp) + (0.25f * lumaUpRight)) +
	//abs((0.50f * lumaLeft) + (-1.0 * lumaCenter) + (0.5f * lumaRight))
	//+ abs((0.25f * lumaDownLeft) + (-0.5 * lumaDown) + (0.25f * lumaDownRight));

	//float edgeHorizontal = abs((0.25f * lumaUpLeft) + (-0.5 * lumaLeft) + (0.25f * lumaDownLeft)) +
	//abs((0.50f * lumaUp) + (-1.0 * lumaCenter) + (0.5f * lumaDown))
	//+ abs((0.25f * lumaUpRight) + (-0.5 * lumaRight) + (0.25f * lumaDownRight));

	const bool isHorizontal = (edgeHorizontal >= edgeVertical);

	/*--------------------------------------------------------------------------*/
	const float luma1 = isHorizontal ? lumaDown : lumaLeft;
	const float luma2 = isHorizontal ? lumaUp : lumaRight;

	const float gradient1 = luma1 - lumaCenter;
	const float grandient2 = luma2 - lumaCenter;

	const bool is1Steepest = abs(gradient1) >= abs(grandient2);

	const float gradientScaled = 0.25f * max(abs(gradient1), abs(grandient2));
	/*--------------------------------------------------------------------------*/

	float stepLength = isHorizontal ? dXdY.y : dXdY.x;

	float lumaLocalAverage = 0.0f;

	if (is1Steepest)
	{
		stepLength = -stepLength;
		lumaLocalAverage = 0.5f * (luma1 + lumaCenter);
	}
	else
	{
		lumaLocalAverage = 0.5f * (luma2 + lumaCenter);
	}

	float2 currentUV = aInput.myUV;
	if (isHorizontal)
	{
		currentUV.y += (stepLength * 0.5f);
	}
	else
	{
		currentUV.x += (stepLength * 0.5f);
	}
	/*--------------------------------------------------------------------------*/

	const float2 offset = isHorizontal ? float2(dXdY.x, 0.0f) : float2(0.0f, dXdY.y);

	float2 uv1 = currentUV - offset;
	float2 uv2 = currentUV + offset;

	float lumaEnd1 = rgb2luma(fullscreenTexture1.Sample(ansitropic8XSampler, uv1).rgb);
	float lumaEnd2 = rgb2luma(fullscreenTexture1.Sample(ansitropic8XSampler, uv2).rgb);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;

	bool reached1 = abs(lumaEnd1) >= gradientScaled;
	bool reached2 = abs(lumaEnd2) >= gradientScaled;
	bool reachedBoth = reached1 && reached2;

	if (!reached1)
	{
		uv1 -= offset;
	}
	if (!reached2)
	{
		uv2 += offset;
	}
	/*--------------------------------------------------------------------------*/
	if (!reachedBoth) //iterate to find edges
	{
		[loop]for (int i = 2; i < ITERATIONS; i++)  // ITERATIONS
		{
			if (!reached1)
			{
				lumaEnd1 = rgb2luma(fullscreenTexture1.Sample(defaultSampler, uv1).rgb);
				lumaEnd1 = lumaEnd1 - lumaLocalAverage;
			}
			if (!reached2)
			{
				lumaEnd2 = rgb2luma(fullscreenTexture1.Sample(defaultSampler, uv2).rgb);
				lumaEnd2 = lumaEnd2 - lumaLocalAverage;
			}

			reached1 = abs(lumaEnd1) >= gradientScaled;
			reached2 = abs(lumaEnd2) >= gradientScaled;
			reachedBoth = reached1 && reached2;

			if (!reached1)
			{
				uv1 -= offset * QUALITY(i);
			}
			if (!reached2)
			{
				uv2 += offset * QUALITY(i);
			}
			if (reachedBoth)
			{
				break;
			}
		}
	}
	/*--------------------------------------------------------------------------*/

	const float distance1 = isHorizontal ? (aInput.myUV.x - uv1.x) : (aInput.myUV.y - uv1.y);
	const float distance2 = isHorizontal ? (uv2.x - aInput.myUV.x) : (uv2.y - aInput.myUV.y);

	const bool isDirection1 = distance1 < distance2;
	const float distanceFinal = min(distance1, distance2);

	const float edgeThickness = (distance1 + distance2);

	const float pixelOffset = -(distanceFinal / edgeThickness) + 0.5f;

	/*--------------------------------------------------------------------------*/

	const bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage; //is the luma at center smaller than the avaaerage value?
	const bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0f) != isLumaCenterSmaller; //if lumacenter is smaller than edgePixel

	float finalOffset = correctVariation ? pixelOffset : 0.0f; // if above is incorrect no pixelOFfset
	/*--------------------------------------------------------------------------*/


	//SubPixel antialiasing
	//compute over 3x3 box-filter

	const float lumaAverage = (1.0f / 12.0f) * (2.0f * (lumaDownUp + lumaLeftRight) + lumaLeftCornerns + lumaRightCorners);

	const float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0f, 1.0f);
	const float subPixelOffset2 = (-2.0f * subPixelOffset1 + 3.0f) * subPixelOffset1 * subPixelOffset1;

	const float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

	finalOffset = max(finalOffset, subPixelOffsetFinal);

	//final
	float2 finalUV = aInput.myUV;
	if (isHorizontal)
	{
		finalUV.y += finalOffset * stepLength;
	}
	else
	{
		finalUV.x += finalOffset * stepLength;
	}

	output.myColor.rgb = fullscreenTexture1.Sample(ansitropic8XSampler, finalUV).rgb;
	output.myColor.a = 1.0f;
	return output;
}