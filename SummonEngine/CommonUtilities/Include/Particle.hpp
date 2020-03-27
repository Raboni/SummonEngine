#pragma once
#include "Vector.hpp"

namespace CU
{
	enum BlendState
	{
		Addative,
		Alpha
	};
	struct EmitterSettings3D
	{
		BlendState myBlendState = BlendState::Alpha;
		Vector3f myMinDirection;
		Vector3f myMaxDirection;
		Vector4f myStartColor;
		Vector4f myEndColor;
		float myAcceleration;
		float myStartScale;
		float myEndScale;
		float myMinTimeBetweenParticleSpawns;
		float myMaxTimeBetweenParticleSpawns;
		float myMinStartSpeed;
		float myMaxStartSpeed;
		float myMinLifeTime;
		float myMaxLifeTime;
		float myMaxSpawnOffset;
	};
	struct EmitterSettings2D
	{
		BlendState myBlendState = BlendState::Alpha;
		Vector4f myStartColor;
		Vector4f myEndColor;
		float myAcceleration;
		float myStartScale;
		float myEndScale;
		float myMinTimeBetweenParticleSpawns;
		float myMaxTimeBetweenParticleSpawns;
		float myMinStartSpeed;
		float myMaxStartSpeed;
		float myMinAngle;
		float myMaxAngle;
		float myMinLifeTime;
		float myMaxLifeTime;
		float myMaxSpawnOffset;
	};
	struct ParticleSettings
	{
		BlendState myBlendState;
		Vector4f myStartColor;
		Vector4f myEndColor;
		float myAcceleration;
		float myStartScale;
		float myEndScale;

		ParticleSettings& operator=(const EmitterSettings3D& aSettings);
		ParticleSettings& operator=(const EmitterSettings2D& aSettings);
	};

	class Particle
	{
	public:
		Particle() = default;
		~Particle() = default;

		void Init(ParticleSettings* aEmitterSettings, const Vector3f& aPosition, const Vector3f& aDirection, float aOffset, float aSpeed, float aLifeTime);
		void Init(ParticleSettings* aEmitterSettings, const Vector2f& aPosition, const Vector2f& aDirection, float aOffset, float aSpeed, float aLifeTime);
		void Update(const float& aDeltaTime);

		Vector3f GetPosition3D();
		Vector2f GetPosition2D();
		float GetOffset();
		Vector4f GetColor();
		float GetScale();
		float GetSpeed();
		float GetLifeTime();
		bool GetAlive();

	private:
		ParticleSettings* myParticleSettings;
		Vector3f myPosition;
		Vector3f myDirection;
		Vector3f myVelocity;
		float myOffset;
		float mySpeed;
		float myLifeTime;
		float myTime;
	};
}