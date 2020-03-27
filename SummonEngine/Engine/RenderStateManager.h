#pragma once
#include <array>

class DXFramework;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;

enum class BlendState
{
	Disable,
	Alphablend,
	Addativeblend,
	Count
};
enum class DepthStencilState
{
	Default,
	ReadOnly,
	ReadWriteLessEqual,
	IgnoreDepth,
	Count
};
enum class RasterizerState
{
	Default,
	Wireframe,
	Count
};
enum class SamplerState
{
	Trilinear,
	Wrapped,
	Point,
	Count
};

class RenderStateManager
{
public:
	RenderStateManager();
	~RenderStateManager() = default;

	bool Init(DXFramework* aFramework);

	void SetBlendState(const BlendState aState, ID3D11DeviceContext* aContext = nullptr);
	void SetDepthStencilState(const DepthStencilState aState, ID3D11DeviceContext* aContext = nullptr);
	void SetRasterizerState(const RasterizerState aState, ID3D11DeviceContext* aContext = nullptr);
	void SetSamplerState(const SamplerState aState, const unsigned int aSlot = 0, ID3D11DeviceContext* aContext = nullptr);
	void SetStatesDefault(ID3D11DeviceContext* aContext = nullptr);

private:
	bool CreateBlendStates(ID3D11Device* aDevice);
	bool CreateDepthStencilStates(ID3D11Device* aDevice);
	bool CreateRasterizerStates(ID3D11Device* aDevice);
	bool CreateSamplerStates(ID3D11Device* aDevice);

private:
	std::array<ID3D11BlendState*, static_cast<int>(BlendState::Count)> myBlendStates;
	std::array<ID3D11DepthStencilState*, static_cast<int>(DepthStencilState::Count)> myDepthStencilStates;
	std::array<ID3D11RasterizerState*, static_cast<int>(RasterizerState::Count)> myRasterizerStates;
	std::array<ID3D11SamplerState*, static_cast<int>(SamplerState::Count)> mySamplerStates;
	ID3D11DeviceContext* myContext;
	int myCurrentBlendState;
	int myCurrentDepthStencilState;
	int myCurrentRasterizerState;
	int myCurrentSamplerState;
};