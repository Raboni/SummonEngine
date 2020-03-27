#pragma once
#include "RenderSettings.h"
#include "SpriteInstance.h"
#include "SpriteRenderCommand.h"
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

class SpriteRenderer
{
public:
	SpriteRenderer();
	~SpriteRenderer();

	bool Init(DXFramework* aFramework);

	void Render(const CU::GrowingArray<SpriteRenderCommand>& aSpriteList, const CU::Vector2f& aScreenSize);

private:
	struct FrameBufferData
	{
		CU::Vector2f myScreenSize;
		int trash[2];
	};
private:
	FrameBufferData myFrameBufferData;
	ID3D11DeviceContext* myContext;
	ID3D11Device* myDevice;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myVertexBuffer;
	unsigned int mySpriteMaxCount;
};