#pragma once
#include <Vector4.hpp>

struct ID3D11DeviceContext;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
struct D3D11_VIEWPORT;

class FullscreenTexture
{
public:
	friend class FullscreenTextureFactory;
	friend class GBuffer;
	friend class RenderManager;

	FullscreenTexture();
	~FullscreenTexture();

	void ClearTexture(const CU::Vector4f& aColor = CU::Vector4f::Zero, ID3D11DeviceContext* aContext = nullptr);
	void ClearDepth(const float aClearDepth = 1.0f, const unsigned int aClearStencil = 0, ID3D11DeviceContext* aContext = nullptr);

	void ReleaseTexture();
	void ReleaseDepth();

	void SetAsActiveTarget(FullscreenTexture* aDepth = nullptr, ID3D11DeviceContext* aContext = nullptr);
	void SetAsResourceOnSlot(const unsigned int aSlot, ID3D11DeviceContext* aContext = nullptr);

private:
	ID3D11DeviceContext* myContext;
	ID3D11Texture2D* myTexture;
	union
	{
		ID3D11RenderTargetView* myRenderTarget;
		ID3D11DepthStencilView* myDepth;
	};
	ID3D11ShaderResourceView* myShaderResource;
	D3D11_VIEWPORT* myViewport;
};