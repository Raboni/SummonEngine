#pragma once
#include "RenderSettings.h"
#include "PBRBufferStructs.h"
#include <GrowingArray.hpp>

namespace CU
{
	class Camera;
}
class DXFramework;
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
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

class DeferredRenderer
{
public:
	DeferredRenderer(RenderSettings& aSettings);
	~DeferredRenderer() = default;

	bool Init(DXFramework* aFramework);
	void GenerateGBuffer(const CU::Camera* aCamera, const CU::GrowingArray<ModelRenderCommand>& aModelList);
	void GenerateGBufferInstanced(const CU::Camera* aCamera, const CU::GrowingArray<ModelRenderCommand>& aModelList, const CU::GrowingArray<int>& aModelStartIndexList);
	void RenderEnvLight(const CU::Camera* aCamera, const EnviromentLight* aEnviromentLight);
	void RenderPointLight(const CU::Camera* aCamera, const CU::GrowingArray<PointLightRenderCommand>& aLightList);

	void UpdateInstanceMaxCount(const unsigned int aCurrentModelCount);
	void CreateAnimationDataTexture();

private:
	FrameBufferData myFrameBufferData;
	ObjectBufferData myObjectBufferData;

	RenderSettings& mySettings;
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
	ID3D11Buffer* myInstanceBuffer;
	ID3D11Buffer* myBoneBuffer;
	ID3D11Buffer* myEnviromentLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11VertexShader* myFullscreenShader;
	VertexShader* myDefaultVertexShader;
	VertexShader* myDefaultVertexShaderAnim;
	VertexShader* myDefaultVertexShaderCustom;
	VertexShader* myDefaultVertexShaderInstanced;
	VertexShader* myDefaultVertexShaderInstancedAnim;
	VertexShader* myDefaultVertexShaderInstancedCustom;
	GeometryShader* myDefaultGeometryShaderCustom;
	ID3D11PixelShader* myGBufferShader;
	ID3D11PixelShader* myEnviromentLightShader;
	ID3D11PixelShader* myPointLightShader;
	ID3D11Texture2D* myAnimationDataTexture;
	ID3D11ShaderResourceView* myAnimationDataTextureView;
	unsigned int myInstanceMaxCount;
	int myCreatedAnimationCount;
	bool myHasCreatedAnimations;
};