#pragma once
#include <array>

class DXFramework;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;

enum class FullscreenShaderType : unsigned int
{
	Copy,
	Luminance,
	GaussianHorizontal,
	GaussianVertical,
	Bloom,
	FXAA,
	Count
};

class FullscreenRenderer
{
public:
	FullscreenRenderer();
	~FullscreenRenderer();

	const bool Init(DXFramework* aFramework);

	void Render(const FullscreenShaderType aEffect);

private:
	std::array<ID3D11PixelShader*, static_cast<unsigned int>(FullscreenShaderType::Count)> myPixelShaders;
	ID3D11DeviceContext* myContext;
	ID3D11VertexShader* myVertexShader;
	ID3D11Buffer* myFrameBuffer;
};