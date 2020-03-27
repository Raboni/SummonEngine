#pragma once
#include "RenderSettings.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterRenderCommand.h"
#include <GrowingArray.hpp>
#include <Matrix4x4.hpp>

namespace CU
{
	class Camera;
}
class DXFramework;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class ParticleRenderer
{
public:
	ParticleRenderer();
	~ParticleRenderer();

	bool Init(DXFramework* aFramework);

	void Render(CU::Camera* aCamera, const CU::GrowingArray<ParticleEmitterInstance*>& aParticleList);
	void Render(CU::Camera* aCamera, const CU::GrowingArray<ParticleEmitterRenderCommand>& aParticleList);

private:
	struct FrameBufferData
	{
		CU::Matrix4x4f myToCameraMatrix;
		CU::Matrix4x4f myToProjectionMatrix;
	};
	struct ObjectBufferData
	{
		CU::Matrix4x4<float> myToWorldMatrix;
	};
private:
	FrameBufferData myFrameBufferData;
	ObjectBufferData myObjectBufferData;

	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
};