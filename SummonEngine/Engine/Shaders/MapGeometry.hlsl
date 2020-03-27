#include "MapStructs.hlsli"

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
		float2(-1.0f, -1.0f),
		float2(1.0f, -1.0f),
		float2(-1.0f, 1.0f),
		float2(1.0f, 1.0f)
	};

	VertexToGeometry input = aInput[0];
	for (unsigned int vertexIndex = 0; vertexIndex < 4; vertexIndex++)
	{
		GeometryToPixel vertex;
		vertex.myPosition.xy = input.myPositionScreen;
		float2x2 rotMatrix = float2x2(cos(input.myRotation), -sin(input.myRotation), sin(input.myRotation), cos(input.myRotation));
		vertex.myPosition.xy += /*mul(rotMatrix,*/ offset[vertexIndex] * input.mySizeScreen;//);
		vertex.myPosition.y *= -1.0f;
		vertex.myPosition.z = 0.0f;
		vertex.myPosition.w = 1.0f;
		float2 focus = (input.myFocus - input.myPositionWorld) / (input.mySizeWorld * 0.5f);
		focus.y *= -1.0f;
		vertex.myUV = (mul(rotMatrix, uv[vertexIndex] / input.myZoom) + focus + float2(1.0f, 1.0f)) / 2;
		aOutput.Append(vertex);
	}
	/*for (unsigned int vertexIndex = 0; vertexIndex < 4; vertexIndex++)
	{
		GeometryToPixel vertex;
		float2 focus = (input.myFocus - input.myPositionWorld) / (input.mySizeWorld * 0.5f);
		focus.y *= -1.0f;
		vertex.myPosition.xy = input.myPositionScreen + focus;
		float2x2 rotMatrix = float2x2(cos(input.myRotation), -sin(input.myRotation), sin(input.myRotation), cos(input.myRotation));
		vertex.myPosition.xy += offset[vertexIndex] * input.mySizeScreen * 0.1f;
		vertex.myPosition.y *= -1.0f;
		vertex.myPosition.z = 0.0f;
		vertex.myPosition.w = 1.0f;
		vertex.myUV = uv[vertexIndex];
		aOutput.Append(vertex);
	}*/
}