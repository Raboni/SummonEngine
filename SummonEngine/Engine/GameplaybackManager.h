#pragma once
#include <GrowingArray.hpp>
#include <Transform.hpp>

class ModelInstance;

struct RecordedOrientation
{
	RecordedOrientation() {};
	RecordedOrientation(const CU::Transform& aTransform)
	{
		myRotation = aTransform.GetRotation();
		myPosition = aTransform.GetPosition();
	}

	CU::Quaternionf myRotation;
	CU::Vector3f myPosition;
};

struct PlaybackKeyFrame
{
	RecordedOrientation myOrientation;
	ModelInstance* myModel;
	float myAnimationTime;
};

struct PlaybackFrame
{
	CU::GrowingArray<PlaybackKeyFrame> myKeyFrames;
};

class GameplaybackManager
{
public:
	GameplaybackManager();
	~GameplaybackManager();

	void StartRecording(const float aTimer = 0.0f);
	void StopRecording();
	void ClearRecording();

	//Will stop recording
	void StartPlayback(const bool aResetPlayback = true, const bool aPlayReversed = false);
	void StopPlayback();

	void Update(const float aDeltaTime);

	//Will clear recording
	void SetRecordingFrameRate(const int aFrameRate);
	void AddModelToRecord(ModelInstance* aModel);

	int GetFrameRate() const;
	bool IsRecording() const;
	bool IsPlaying() const;

private:
	void RecordFrame();
	void PlayFrame();

private:
	CU::GrowingArray<PlaybackFrame> myFrames;
	CU::GrowingArray<ModelInstance*> myRecordedModels;
	CU::GrowingArray<RecordedOrientation> myModelOrientationList;

	float myRecordTimer;
	float myFrameTimer;
	float myFrameDuration;
	int myPlayFrame;
	int myFrameRate;
	bool myIsRecording;
	bool myIsPlaying;
	bool myPlayReversed;
};