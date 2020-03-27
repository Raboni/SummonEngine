#pragma once
#include "ParticleEmitter.h"
#include <Transform.hpp>
#include <GrowingArray.hpp>

class ParticleEmitterInstance
{
public:
	static void InsertionSortParticle(CU::GrowingArray<ParticleData, int>& aUnsortedArray);

	ParticleEmitterInstance();
	ParticleEmitterInstance(const std::string& aPath);
	ParticleEmitterInstance(const ParticleEmitterInstance& aInstance);
	ParticleEmitterInstance& operator=(const ParticleEmitterInstance& aInstance);
	~ParticleEmitterInstance();

	void Init(const std::string& aPath);
	void Update(const float aDeltaTime);
	void Emit(const CU::Vector3f& aPosition);

	void SetPosition(const CU::Vector3f& aPosition);
	void SetRotation(const CU::Vector3f& aRotation);

	const CU::Vector3f& GetPosition() const;
	ParticleEmitter* GetEmitter() const;
	CU::GrowingArray<ParticleData, int>& GetParticleList();
	const CU::GrowingArray<ParticleData, int>& GetParticleList() const;

private:
	friend struct ParticleEmitterRenderCommand;

	CU::Transform myTransform;
	CU::GrowingArray<ParticleData, int> myParticleList;
	std::string myPath;
	ParticleEmitter* myEmitter;
	float myEmitTimer;
	float myNextEmit;
};