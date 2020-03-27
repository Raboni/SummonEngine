#pragma once
#include "PBRBufferStructs.h"
#include "TextureInstance.h"
#include <GrowingArray.hpp>

namespace CU
{
	class Camera;
}
class DXFramework;
struct ModelRenderCommand;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class WireframeRenderer
{
public:
	WireframeRenderer();
	~WireframeRenderer();

	bool Init(DXFramework* aFramework);
	void Render(CU::Camera* aCamera, const CU::GrowingArray<ModelRenderCommand>& aModelList);

private:
	FrameBufferData myFrameBufferData;
	ObjectBufferData myObjectBufferData;
	TextureInstance myTexture;

	ID3D11Device* myDevice;
	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11InputLayout* myInputLayout;
};