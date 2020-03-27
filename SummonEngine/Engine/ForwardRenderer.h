#pragma once
#include "RenderSettings.h"
#include "PBRBufferStructs.h"
#include <GrowingArray.hpp>

namespace CU
{
	class Camera;
}
class DXFramework;
class ModelInstance;
struct ModelRenderCommand;
struct PointLightRenderCommand;
class EnviromentLight;
class VertexShader;
class GeometryShader;
class PixelShader;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11CommandList;
struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class ForwardRenderer
{
public:
	ForwardRenderer(RenderSettings& aSettings);
	~ForwardRenderer();

	bool Init(DXFramework* aFramework);
	void Render(const CU::Camera* aCamera, const EnviromentLight* aLight,
		const CU::GrowingArray<ModelRenderCommand>& aModelList, const CU::GrowingArray<CU::GrowingArray<PointLightRenderCommand>>& aLightList);
	void RenderSkybox(const CU::Camera* aCamera, const EnviromentLight* aLight, const ModelRenderCommand& aSkybox);

private:
	FrameBufferData myFrameBufferData;
	ObjectBufferData myObjectBufferData;
	PointLightListBufferData myPointLightBufferData;

	RenderSettings& mySettings;
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
	ID3D11Buffer* myEnviromentLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	VertexShader* myDefaultVertexShader;
	VertexShader* myDefaultVertexShaderAnim;
	VertexShader* myDefaultVertexShaderCustom;
	GeometryShader* myDefaultGeometryShaderCustom;
	PixelShader* myDefaultPixelShaderCustom;
	PixelShader* myDefaultPixelShader;
};