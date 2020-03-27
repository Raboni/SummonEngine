#include "stdafx.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
{
	mySettings = EmitterSettings();
	myData = ParticleEmitterData();
}
ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::AddUser()
{
	myUserCount++;
	myShouldUpdateBuffer = true;
}
void ParticleEmitter::RemoveUser()
{
	myUserCount--;
	myShouldUpdateBuffer = true;
}
const unsigned int ParticleEmitter::GetUserCount() const
{
	return myUserCount;
}

EmitterSettings& ParticleEmitter::GetSettings()
{
	return mySettings;
}
ParticleEmitterData& ParticleEmitter::GetData()
{
	return myData;
}