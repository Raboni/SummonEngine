#include "ParticleStructs.hlsli"

[maxvertexcount(4)]
void main(point VertexToGeometry aInput[1], inout TriangleStream<GeometryToPixel> aOutput)
{
	const float2 offset[4] =
	{
		float2(-1.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(-1.0f, -1.0f),
		float2(1.0f, -1.0f)
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
		vertex.myPosition = input.myPosition;
		vertex.myPosition.xy += offset[vertexIndex] * input.mySize;
		vertex.myPosition = mul(toProjection, vertex.myPosition);
		vertex.myColor = input.myColor;
		vertex.myUV = uv[vertexIndex];
		aOutput.Append(vertex);
	}
}