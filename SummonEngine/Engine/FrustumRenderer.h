#pragma once
#include "PBRBufferStructs.h"
#include "TextureInstance.h"
#include "RenderStateManager.h"
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

class FrustumRenderer
{
public:
	FrustumRenderer();
	~FrustumRenderer();

	bool Init(DXFramework* aFramework);
	void Render(CU::Camera* aViewCamera, CU::Camera* aFrustumCamera, RenderStateManager& aStateManager);

private:
	FrameBufferData myFrameBufferData;
	TextureInstance myTexture;

	CU::GrowingArray<CU::Vector4f> myPointList;
	CU::GrowingArray<unsigned int> myIndexList;

	ID3D11Device* myDevice;
	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11InputLayout* myInputLayout;
};