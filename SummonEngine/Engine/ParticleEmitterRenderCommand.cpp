#include "stdafx.h"
#include "ParticleEmitterRenderCommand.h"
#include "ParticleEmitterInstance.h"

ParticleEmitterRenderCommand::ParticleEmitterRenderCommand()
{
	myEmitter = nullptr;
}

ParticleEmitterRenderCommand::ParticleEmitterRenderCommand(ParticleEmitterInstance* aInstance)
{
	myParticleList = aInstance->myParticleList;
	myEmitter = aInstance->myEmitter;
}

ParticleEmitterRenderCommand::ParticleEmitterRenderCommand(const ParticleEmitterRenderCommand& aCommand)
{
	myParticleList = aCommand.myParticleList;
	myEmitter = aCommand.myEmitter;
}

ParticleEmitterRenderCommand& ParticleEmitterRenderCommand::operator=(const ParticleEmitterRenderCommand& aCommand)
{
	myParticleList = aCommand.myParticleList;
	myEmitter = aCommand.myEmitter;
	return *this;
}

ParticleEmitterRenderCommand::~ParticleEmitterRenderCommand()
{
	myEmitter = nullptr;
}
