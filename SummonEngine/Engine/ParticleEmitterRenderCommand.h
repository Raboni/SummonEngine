#pragma once
#include "ParticleEmitter.h"
#include <Matrix4x4.hpp>

class ParticleEmitterInstance;

struct ParticleEmitterRenderCommand
{
	ParticleEmitterRenderCommand();
	ParticleEmitterRenderCommand(ParticleEmitterInstance* aInstance);
	ParticleEmitterRenderCommand(const ParticleEmitterRenderCommand& aCommand);
	ParticleEmitterRenderCommand& operator=(const ParticleEmitterRenderCommand& aCommand);
	~ParticleEmitterRenderCommand();

	CU::GrowingArray<ParticleData, int> myParticleList;
	ParticleEmitter* myEmitter;
};