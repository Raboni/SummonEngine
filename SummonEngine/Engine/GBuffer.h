#pragma once
#include <Vector4.hpp>
#include <array>

class FullscreenTexture;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct D3D11_VIEWPORT;

enum class GBufferTextureType
{
	Albedo,
	Normal,
	Metalness,
	Roughness,
	AmbientOcclusion,
	Emissive,
	Position,
	Count
};

class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	void ClearTextures(const CU::Vector4f& aColor = CU::Vector4f::Zero, ID3D11DeviceContext* aContext = nullptr);
	void SetAsActiveTarget(FullscreenTexture* aDepth = nullptr, ID3D11DeviceContext* aContext = nullptr);
	void SetAsResourceOnSlot(GBufferTextureType aResourceType, unsigned int aSlot, ID3D11DeviceContext* aContext = nullptr);
	void SetAllAsResources(ID3D11DeviceContext* aContext = nullptr);
	void Release();

private:
	friend class FullscreenTextureFactory;

	std::array<ID3D11Texture2D*, static_cast<int>(GBufferTextureType::Count)> myTextures;
	std::array<ID3D11RenderTargetView*, static_cast<int>(GBufferTextureType::Count)> myRenderTargets;
	std::array<ID3D11ShaderResourceView*, static_cast<int>(GBufferTextureType::Count)> myShaderResources;
	ID3D11DeviceContext* myContext;
	D3D11_VIEWPORT* myViewport;
};