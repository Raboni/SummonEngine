#pragma once
//#include "ParticleEmitter.hpp"
#include "TextureInstance.h"
#include <Vector.hpp>
#include <HashMap.hpp>

struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

struct EmitterSettings
{
	CU::Vector4f myStartColor;
	CU::Vector4f myEndColor;
	CU::Vector3f myRotation;
	CU::Vector3f myMinEmitAngles;
	CU::Vector3f myMaxEmitAngles;
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

struct ParticleEmitterData
{
	TextureInstance myTexture = TextureInstance();
	ID3D11Buffer* myVertexBuffer = nullptr;
	ID3D11VertexShader* myVertexShader = nullptr;
	ID3D11GeometryShader* myGeometryShader = nullptr;
	ID3D11PixelShader* myPixelShader = nullptr;
	ID3D11InputLayout* myInputLayout = nullptr;
	unsigned int myMaxParticlesPerInstance = 0;
	unsigned int myStride = 0;
	unsigned int myOffset = 0;
	unsigned short myTopology = 0;
};

struct ParticleData
{
	CU::Vector4f myPosition;
	CU::Vector4f myDirection;
	CU::Vector4f myColor;
	CU::Vector2f mySize;
	float myLifeTime;
	float myTime;
	float myDistanceToCameraSqr;
	float mySpeed;
};

class ParticleEmitter
{
public:
	EmitterSettings& GetSettings();
	ParticleEmitterData& GetData();

private:
	friend class ParticleEmitterLoader;
	friend class ParticleEmitterInstance;
	friend class CU::HashMap<std::string, ParticleEmitter>;
	ParticleEmitter();
	~ParticleEmitter();

	void AddUser();
	void RemoveUser();
	const unsigned int GetUserCount() const;

private:
	EmitterSettings mySettings;
	ParticleEmitterData myData;
	unsigned int myUserCount;
	bool myShouldUpdateBuffer;
};