#pragma once
#include "ParticleEmitter.h"
#include "HashMap.hpp"
#include <string>

struct ID3D11Device;

class ParticleEmitterLoader
{
public:
	static void CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxEmitterAmount);
	static ParticleEmitterLoader* GetInstance();
	static void DestroyInstance();

	ParticleEmitter* CreateParticleEmitter(const std::string& aAlias, const std::string& aTexture, const EmitterSettings& aSettings);
	ParticleEmitter* CreateParticleEmitter(const std::string& aAlias, const std::string& aTexture, const std::string& aJsonAlias);
	ParticleEmitter* GetParticleEmitter(const std::string& aAlias);
	void UpdateParticleEmitterBuffer(ParticleEmitter* aEmitter);

private:
	ParticleEmitterLoader(ID3D11Device* aDevice, const unsigned int aMaxEmitterAmount);
	~ParticleEmitterLoader();

	static ParticleEmitterLoader* ourInstance;

	CU::HashMap<std::string, ParticleEmitter> myEmitterMap;
	ID3D11Device* myDevice;
};