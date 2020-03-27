#pragma once
#include "ParticleEmitter.hpp"

namespace CU
{
	class ParticlePathEmitter2D
	{
	public:
		ParticlePathEmitter2D();
		~ParticlePathEmitter2D() = default;

		//void Init(const std::string& aJsonPath);
		void Init(const EmitterSettings2D& aSettings);
		void Update(float aDeltaTime);
		void ClearParticles();
		void SetPausedEmit(bool aPaused);
		bool GetPausedEmit();

		void AddPathPoint(const Vector2f& aPosition);
		void RemovePathPoint(unsigned short aIndex);
		void SetPath(const GrowingArray<Vector2f>& aArray);
		void ResetPath();
		void SetMinPointDistance(float aDistance);

		GrowingArray<Particle> GetParticles();
		unsigned short GetPathPointAmount();

	private:
		void Emit();

		//JsonReader myJsonReader;
		GrowingArray<Particle> myParticles;
		GrowingArray<unsigned short> myParticlePathPoints;
		GrowingArray<Vector2f> myPath;

		EmitterSettings2D mySettings;
		ParticleSettings myParticleSettings;
		CU::Vector2f myEmitDirection;
		float myEmitTimer;
		float myNextEmit;
		float myMinPointDistance;
		float mySpawnOffset;
		bool myIsEmitting;
	};
}