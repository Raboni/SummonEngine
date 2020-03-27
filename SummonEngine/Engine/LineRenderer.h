#pragma once
#include "LineRenderCommand.h"
#include "PBRBufferStructs.h"
#include <GrowingArray.hpp>

namespace CU
{
	class Camera;
}
class DXFramework;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class LineRenderer
{
public:
	LineRenderer();
	~LineRenderer();

	const bool Init(DXFramework* aFramework);
	void Render(CU::Camera* aCamera, const CU::GrowingArray<LineRenderCommand>& aLineList);

private:
	FrameBufferData myFrameBufferData;
	LineRenderCommand myLine;

	ID3D11Device* myDevice;
	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myVertexBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11InputLayout* myInputLayout;
};