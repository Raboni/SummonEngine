#include "stdafx.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterLoader.h"
#include <Random.hpp>
#include <iostream>

void ParticleEmitterInstance::InsertionSortParticle(CU::GrowingArray<ParticleData, int>& aUnsortedArray)
{
	ParticleData temp;
	int pos = 0;
	for (int i = 1; i < aUnsortedArray.Size(); i++)
	{
		temp = aUnsortedArray[i];
		pos = i - 1;

		while (pos >= 0 && temp.myDistanceToCameraSqr > aUnsortedArray[pos].myDistanceToCameraSqr)
		{
			aUnsortedArray[pos + 1] = aUnsortedArray[pos];
			--pos;
		}

		aUnsortedArray[pos + 1] = temp;
	}
}

ParticleEmitterInstance::ParticleEmitterInstance()
{
	myPath = "";
	myParticleList.Init(16);
	myEmitTimer = 0.0f;
	myNextEmit = 0.0f;
	myEmitter = nullptr;
}
ParticleEmitterInstance::ParticleEmitterInstance(const std::string& aPath)
{
	myPath = aPath;
	myEmitter = ParticleEmitterLoader::GetInstance()->GetParticleEmitter(aPath);
	myParticleList.Init(16);
	myEmitTimer = 0.0f;
	myNextEmit = 0.0f;
}
ParticleEmitterInstance::ParticleEmitterInstance(const ParticleEmitterInstance& aInstance)
{
	memcpy(&myTransform, &aInstance.myTransform, sizeof(CU::Transform));
	myPath = aInstance.myPath;
	myParticleList = aInstance.myParticleList;
	myEmitTimer = aInstance.myEmitTimer;
	myNextEmit = aInstance.myNextEmit;
	myEmitter = aInstance.myEmitter;
	if (myEmitter != nullptr)
	{
		myEmitter->AddUser();
	}
}
ParticleEmitterInstance& ParticleEmitterInstance::operator=(const ParticleEmitterInstance& aInstance)
{
	memcpy(&myTransform, &aInstance.myTransform, sizeof(CU::Transform));
	myPath = aInstance.myPath;
	myParticleList = aInstance.myParticleList;
	myEmitTimer = aInstance.myEmitTimer;
	myNextEmit = aInstance.myNextEmit;

	if (myEmitter != nullptr)
	{
		myEmitter->RemoveUser();
	}
	myEmitter = aInstance.myEmitter;
	if (myEmitter != nullptr)
	{
		myEmitter->AddUser();
	}
	return *this;
}
ParticleEmitterInstance::~ParticleEmitterInstance()
{
}

void ParticleEmitterInstance::Init(const std::string& aPath)
{
	myPath = aPath;

	if (myEmitter != nullptr)
	{
		myEmitter->RemoveUser();
	}
	myEmitter = ParticleEmitterLoader::GetInstance()->GetParticleEmitter(myPath);
	if (myEmitter != nullptr)
	{
		myEmitter->AddUser();
	}
}

void ParticleEmitterInstance::Update(const float aDeltaTime)
{
	//ParticleEmitterData& emitData = myEmitter->GetData();
	EmitterSettings& emitSettings = myEmitter->GetSettings();

	myEmitTimer += aDeltaTime;
	if (myEmitTimer >= myNextEmit)
	{
		myNextEmit = CU::RandomDecimal(emitSettings.myMinTimeBetweenParticleSpawns, emitSettings.myMaxTimeBetweenParticleSpawns);
		myEmitTimer = 0.0f;
		Emit(myTransform.GetPosition());
	}
	for (unsigned short index = 0; index < myParticleList.Size(); index++)
	{
		myParticleList[index].myTime += aDeltaTime;
		if (myParticleList[index].myTime > myParticleList[index].myLifeTime)
		{
			myParticleList.RemoveCyclicAtIndex(index);
			index--;
			continue;
		}
		float patricleDeltaTime = myParticleList[index].myTime / myParticleList[index].myLifeTime;
		float scale = emitSettings.myStartScale + (emitSettings.myEndScale - emitSettings.myStartScale) * patricleDeltaTime;
		myParticleList[index].mySize = CU::Vector2f(scale, scale);
		myParticleList[index].myColor = emitSettings.myStartColor + (emitSettings.myEndColor - emitSettings.myStartColor) * patricleDeltaTime;
		myParticleList[index].mySpeed += emitSettings.myAcceleration * aDeltaTime;
		myParticleList[index].myPosition += myParticleList[index].myDirection * myParticleList[index].mySpeed * aDeltaTime;
		myParticleList[index].myPosition.w = 1.0f;
	}
	//InsertionSortParticle(myParticleList);
}

void ParticleEmitterInstance::SetPosition(const CU::Vector3f& aPosition)
{
	myTransform.SetPosition(aPosition);
}

void ParticleEmitterInstance::SetRotation(const CU::Vector3f& aRotation)
{
	myTransform.SetRotation(aRotation);
}

const CU::Vector3f& ParticleEmitterInstance::GetPosition() const
{
	return myTransform.GetPosition();
}

ParticleEmitter* ParticleEmitterInstance::GetEmitter() const
{
	return myEmitter;
}

CU::GrowingArray<ParticleData, int>& ParticleEmitterInstance::GetParticleList()
{
	return myParticleList;
}

const CU::GrowingArray<ParticleData, int>& ParticleEmitterInstance::GetParticleList() const
{
	return myParticleList;
}

void ParticleEmitterInstance::Emit(const CU::Vector3f& aPosition)
{
	ParticleData data;
	EmitterSettings settings = myEmitter->GetSettings();

	//myTransform.SetRotation(settings.myRotation);
	CU::Matrix4x4f orientation;
	orientation *= CU::Matrix4x4f::CreateRotationAroundX(CU::RandomDecimal(settings.myMinEmitAngles.x, settings.myMaxEmitAngles.x));
	orientation *= CU::Matrix4x4f::CreateRotationAroundY(CU::RandomDecimal(settings.myMinEmitAngles.y, settings.myMaxEmitAngles.y));
	orientation *= CU::Matrix4x4f::CreateRotationAroundZ(CU::RandomDecimal(settings.myMinEmitAngles.z, settings.myMaxEmitAngles.z));
	orientation *= myTransform.GetMatrix();
	data.myDirection = CU::Vector4f(0.0f, 0.0f, 1.0f, 0.0f) * orientation;

	/*data.myDirection = CU::Vector4f(CU::RandomDecimal(settings.myMinDirection.x, settings.myMaxDirection.x),
		CU::RandomDecimal(settings.myMinDirection.y, settings.myMaxDirection.y),
		CU::RandomDecimal(settings.myMinDirection.z, settings.myMaxDirection.z), 0.0f);*/
	data.myDirection.Normalize();
	data.mySpeed = CU::RandomDecimal(settings.myMinStartSpeed, settings.myMaxStartSpeed, 5);
	data.myLifeTime = CU::RandomDecimal(settings.myMinLifeTime, settings.myMaxLifeTime);
	data.myPosition = CU::Vector4f(aPosition.x, aPosition.y, aPosition.z, 1.0f);
	data.myTime = 0.0f;
	data.myDistanceToCameraSqr = 0.0f;
	data.myColor = settings.myStartColor;
	data.mySize = CU::Vector2f(settings.myStartScale, settings.myStartScale);

	myParticleList.Add(data);
}