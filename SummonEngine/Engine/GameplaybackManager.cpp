#include "stdafx.h"
#include "GameplaybackManager.h"
#include "ModelInstance.h"

GameplaybackManager::GameplaybackManager() : myFrames(640)
{
	myRecordedModels.Init(8);
	myModelOrientationList.Init(8);
	SetRecordingFrameRate(30);
	myRecordTimer = 0.0f;
	myFrameTimer = 0.0f;
	myPlayFrame = 0;
	myIsRecording = false;
	myIsPlaying = false;
}
GameplaybackManager::~GameplaybackManager()
{
}

void GameplaybackManager::StartRecording(const float aTimer)
{
	myRecordTimer = aTimer;
	myFrameTimer = 0.0f;
	StopPlayback();
	if (myRecordedModels.Size() > 0)
	{
		myIsRecording = true;
		RecordFrame();
	}
}
void GameplaybackManager::StopRecording()
{
	myIsRecording = false;
	myRecordTimer = 0.0f;
}
void GameplaybackManager::ClearRecording()
{
	myFrames.RemoveAll();
}

void GameplaybackManager::StartPlayback(const bool aResetPlayback, const bool aPlayReversed)
{
	StopRecording();
	myPlayReversed = aPlayReversed;
	myFrameTimer = 0.0f;
	if (aResetPlayback)
	{
		if (aPlayReversed)
		{
			myPlayFrame = myFrames.Size() - 1;
		}
		else
		{
			myPlayFrame = 0;
		}
	}
	else
	{
		myPlayFrame = CU::Math::Clamp(myPlayFrame, 0, myFrames.Size() - 1);
	}

	if (myFrames.Size() > 0)
	{
		myIsPlaying = true;
		PlayFrame();
	}
}
void GameplaybackManager::StopPlayback()
{
	myIsPlaying = false;
}

void GameplaybackManager::Update(const float aDeltaTime)
{
	if (myIsPlaying)
	{
		myFrameTimer += aDeltaTime;
		bool newFrame = false;
		while (myFrameTimer >= myFrameDuration)
		{
			myFrameTimer -= myFrameDuration;
			newFrame = true;
			if (myPlayReversed)
			{
				myPlayFrame--;
			}
			else
			{
				myPlayFrame++;
			}
		}

		if (myPlayFrame >= myFrames.Size() || myPlayFrame < 0)
		{
			StopPlayback();
			return;
		}

		if (newFrame)
		{
			PlayFrame();
		}
	}
	else if (myIsRecording)
	{
		myFrameTimer += aDeltaTime;
		while (myFrameTimer >= myFrameDuration)
		{
			myFrameTimer -= myFrameDuration;
			RecordFrame();
		}
		if (myRecordTimer > 0.0f)
		{
			myRecordTimer -= aDeltaTime;
			if (myRecordTimer <= 0.0f)
			{
				StopRecording();
			}
		}
	}
}

void GameplaybackManager::SetRecordingFrameRate(const int aFrameRate)
{
	if (aFrameRate > 0)
	{
		myFrameRate = aFrameRate;
		myFrameDuration = 1.0f / myFrameRate;
	}
}

void GameplaybackManager::AddModelToRecord(ModelInstance* aModel)
{
	if (aModel != nullptr)
	{
		myRecordedModels.Add(aModel);
		myModelOrientationList.Add();
	}
}

int GameplaybackManager::GetFrameRate() const
{
	return myFrameRate;
}
bool GameplaybackManager::IsRecording() const
{
	return myIsRecording;
}
bool GameplaybackManager::IsPlaying() const
{
	return myIsPlaying;
}

void GameplaybackManager::RecordFrame()
{
	int prevKeyFrameSize = 0;
	if (myFrames.Size() > 0)
	{
		prevKeyFrameSize = myFrames.GetLast().myKeyFrames.Size();
	}
	else
	{
		prevKeyFrameSize = myRecordedModels.Size();
	}

	PlaybackFrame& frame = myFrames.Add();
	frame.myKeyFrames.Init(prevKeyFrameSize);
	for (int modelIndex = 0; modelIndex < myRecordedModels.Size(); modelIndex++)
	{
		ModelInstance* model = myRecordedModels[modelIndex];
		RecordedOrientation orientation(model->GetTransform());
		if (orientation.myRotation == myModelOrientationList[modelIndex].myRotation && orientation.myPosition == myModelOrientationList[modelIndex].myPosition)
		{
			continue;
		}
		myModelOrientationList[modelIndex] = orientation;

		PlaybackKeyFrame& key = frame.myKeyFrames.Add();
		key.myModel = model;
		key.myOrientation = model->GetTransform();
	}
}
void GameplaybackManager::PlayFrame()
{
	if (myPlayFrame < 0 || myPlayFrame >= myFrames.Size())
	{
		return;
	}
	PlaybackFrame& frame = myFrames[myPlayFrame];
	for (int keyIndex = 0; keyIndex < frame.myKeyFrames.Size(); keyIndex++)
	{
		ModelInstance* model = frame.myKeyFrames[keyIndex].myModel;
		RecordedOrientation& orientation = frame.myKeyFrames[keyIndex].myOrientation;
		model->SetRotation(orientation.myRotation);
		model->SetPosition(orientation.myPosition);
		if (model->IsSkinned())
		{
			
		}
	}
}