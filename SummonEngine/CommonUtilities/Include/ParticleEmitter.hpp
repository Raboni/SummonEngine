#pragma once
#include "Particle.hpp"
#include "GrowingArray.hpp"
//#include "JsonReader.hpp"
#include <string>

namespace CU
{
	class ParticleEmitter
	{
	public:
		ParticleEmitter();
		~ParticleEmitter() = default;

		//void Init(const std::string& aJsonPath);
		void Init(const EmitterSettings3D& aSettings);
		void Update(const float& aDeltaTime);
		void ClearParticles();

		void AddPosition(const Vector3f& aPosition);
		void RemovePosition(const Vector3f& aPosition);
		void SetPositions(const GrowingArray<Vector3f>& aArray);
		void SetPositions(const Vector3f& aPosition);
		void ResetPositions();

		void AddPositionPointer(Vector3f* aPosition);
		void RemovePositionPointer(Vector3f* aPosition);
		void SetPositionPointers(GrowingArray<Vector3f*> aArray);
		void ClearPositionPointers();
		void DeletePositionPointers();

		GrowingArray<Particle> GetParticles();
		unsigned short GetPathPointAmount();

	private:
		void Emit(const Vector3f& aPosition);

		//JsonReader myJsonReader;
		GrowingArray<Particle> myParticles;
		GrowingArray<Vector3f> myPath;
		GrowingArray<Vector3f*> myPositionPointers;

		EmitterSettings3D mySettings;
		ParticleSettings myParticleSettings;
		float myEmitTimer;
		float myNextEmit;
		bool myIsEmitting;
	};

	class ParticleEmitter2D
	{
	public:
		ParticleEmitter2D();
		~ParticleEmitter2D() = default;

		//void Init(const std::string& aJsonPath);
		void Init(const EmitterSettings2D& aSettings);
		void Update(const float& aDeltaTime);
		void ClearParticles();

		void AddPosition(const Vector2f& aPosition);
		bool RemovePosition(const Vector2f& aPosition);
		void SetPositions(const GrowingArray<Vector2f>& aArray);
		void SetPositions(const Vector2f& aPosition);
		void ResetPositions();

		void AddPositionPointer(Vector2f* aPosition);
		bool RemovePositionPointer(Vector2f* aPosition);
		void SetPositionPointers(const GrowingArray<Vector2f*>& aArray);
		void SetPositionPointers(Vector2f* aPosition);
		void ClearPositionPointers();
		void DeletePositionPointers();

		GrowingArray<Particle> GetParticles();
		unsigned short GetPathPointAmount();

	private:
		void Emit(const Vector2f& aPosition);

		//JsonReader myJsonReader;
		GrowingArray<Particle> myParticles;
		GrowingArray<Vector2f> myPath;
		GrowingArray<Vector2f*> myPositionPointers;

		EmitterSettings2D mySettings;
		ParticleSettings myParticleSettings;
		float myEmitTimer;
		float myNextEmit;
		bool myIsEmitting;
	};
}