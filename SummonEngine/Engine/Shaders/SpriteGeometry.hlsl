#include "SpriteStructs.hlsli"

[maxvertexcount(4)]
void main(point VertexToGeometry aInput[1], inout TriangleStream<GeometryToPixel> aOutput)
{
	const float2 offset[4] =
	{
		float2(-0.5f, -0.5f),
		float2(0.5f, -0.5f),
		float2(-0.5f, 0.5f),
		float2(0.5f, 0.5f)
	};
	const float2 uv[4] =
	{
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f)
	};

	VertexToGeometry input = aInput[0];
	for (unsigned int vertexIndex = 0; vertexIndex < 4; vertexIndex++)
	{
		GeometryToPixel vertex;
		vertex.myColor = input.myColor;
		vertex.myPosition.xy = input.myPosition;
		float2x2 rotMatrix = float2x2(cos(input.myRotation), -sin(input.myRotation), sin(input.myRotation), cos(input.myRotation));
		vertex.myPosition.xy += mul(rotMatrix, (offset[vertexIndex] - input.myPivot * 0.5f) * input.mySize);
		vertex.myPosition.y *= -1.0f;
		vertex.myPosition.x *= ScreenSize.y / ScreenSize.x;
		vertex.myPosition.z = 0.0f;
		vertex.myPosition.w = 1.0f;
		vertex.myUV = uv[vertexIndex];
		aOutput.Append(vertex);
	}
}