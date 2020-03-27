#pragma once
#include "RenderSettings.h"
#include "MapRenderCommand.h"
#include <GrowingArray.hpp>

namespace CU
{
	class Camera;
}
class DXFramework;
class FullscreenTexture;
struct ID3D11DeviceContext;
struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;

class MapRenderer
{
public:
	MapRenderer();
	~MapRenderer();

	bool Init(DXFramework* aFramework);
	void Render(const MapRenderCommand& aCommand);

	void SetMapTexture(FullscreenTexture* aMapTexture);

private:
	ID3D11DeviceContext* myContext;
	ID3D11Device* myDevice;
	ID3D11Buffer* myVertexBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11GeometryShader* myGeometryShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11InputLayout* myInputLayout;
	FullscreenTexture* myMapTexture;
};