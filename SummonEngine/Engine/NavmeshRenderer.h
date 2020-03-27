#pragma once
#include "Navmesh.h"
#include "PBRBufferStructs.h"
#include "TextureInstance.h"
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

namespace Nav
{
	class NavmeshRenderer
	{
	public:
		NavmeshRenderer();
		~NavmeshRenderer();

		bool Init(DXFramework* aFramework);
		void Render(CU::Camera* aCamera, const CU::GrowingArray<Nav::Navmesh>& aNavmeshList);

	private:
		FrameBufferData myFrameBufferData;
		TextureInstance myTexture;

		ID3D11Device* myDevice;
		ID3D11DeviceContext* myContext;
		ID3D11Buffer* myFrameBuffer;
		ID3D11VertexShader* myVertexShader;
		ID3D11PixelShader* myPixelShader;
		ID3D11InputLayout* myInputLayout;
	};
}