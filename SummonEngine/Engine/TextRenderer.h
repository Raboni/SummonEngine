#pragma once
#include "TextInstance.h"
#include <GrowingArray.hpp>
#include <Matrix4x4.hpp>

namespace CU
{
	class Camera;
}
class DXFramework;
struct ID3D11DeviceContext;
struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class TextRenderer
{
public:
	TextRenderer();
	~TextRenderer();

	bool Init(DXFramework* aFramework);

	void Render(const CU::GrowingArray<TextInstance>& aTextList, const CU::Vector2f& aScreenSize);

private:
	struct FrameBufferData
	{
		CU::Vector2f myScreenSize;
		int trash[2];
	};
	struct ObjectBufferData
	{
		CU::Vector4f myColor;
		CU::Vector4f myPosition;
		CU::Vector2f mySize;
		CU::Vector2f myScale;
		CU::Vector2f myPivot;
		int trash[2];
	};

private:
	FrameBufferData myFrameBufferData;
	ObjectBufferData myObjectBufferData;
	ID3D11DeviceContext* myContext;
	ID3D11Device* myDevice;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11InputLayout* myInputLayout;
	const unsigned int myStride;
	const unsigned int myOffset;
};

