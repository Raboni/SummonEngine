#include "stdafx.h"
#include "VRManager.h"
#include "VRMatrixConversion.h"
#include <DebugTools.h>

VRManager::VRManager()
{
	myVRSystem = nullptr;
	myVRArea = nullptr;
	myHasSetProjection = false;
}
VRManager::~VRManager()
{
	Quit();
}

bool VRManager::Init()
{
	if (myVRSystem == nullptr)
	{
		vr::EVRInitError initError = vr::EVRInitError::VRInitError_None;
		myVRSystem = vr::VR_Init(&initError, vr::VRApplication_Scene);

		if (initError != vr::EVRInitError::VRInitError_None)
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

			myVRSystem = nullptr;
			CU::Log("OpenVR init error: " + std::to_string(static_cast<int>(initError)), CU::LogType::Error);

			SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
			return false;
		}

		myVRArea = vr::VRChaperone();
		if (myVRArea == nullptr)
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

			myVRSystem = nullptr;
			CU::Log("OpenVR failed to get chaperone (area data)", CU::LogType::Error);

			SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
			return false;
		}

		//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		//CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		//GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		//SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

		myVRSystem->GetRecommendedRenderTargetSize(&myRenderTargetSize.x, &myRenderTargetSize.y);
		CU::Log("OpenVR initialized, RenderTarget size: " + std::to_string(myRenderTargetSize.x) + 'x' + std::to_string(myRenderTargetSize.y), CU::LogType::Success);
		//std::cout << "OpenVR initialized, RenderTarget size: " << myRenderTargetSize.x << 'x' << myRenderTargetSize.y << std::endl;

		//SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
		myHasSetProjection = false;
	}
	return (myVRSystem != nullptr);
}

void VRManager::Update()
{
	if (!myHasSetProjection)
	{
		myVRCamera.SetCameraLeft(EngineProxy::GetActiveScene()->GetVRCameraLeft());
		myVRCamera.SetCameraRight(EngineProxy::GetActiveScene()->GetVRCameraRight());
		myVRCamera.GetCameraLeft()->SetAspectRatio(static_cast<float>(myRenderTargetSize.x) / static_cast<float>(myRenderTargetSize.y));
		myVRCamera.GetCameraRight()->SetAspectRatio(static_cast<float>(myRenderTargetSize.x) / static_cast<float>(myRenderTargetSize.y));

		CU::Matrix4x4f cuProjectionMatrix;
		VRMatrixToCUMatrixProjection(myVRSystem->GetProjectionMatrix(vr::EVREye::Eye_Left, 0.1f, 40.0f), cuProjectionMatrix);
		myVRCamera.SetProjectionMatrixLeft(cuProjectionMatrix);
		VRMatrixToCUMatrixProjection(myVRSystem->GetProjectionMatrix(vr::EVREye::Eye_Right, 0.1f, 40.0f), cuProjectionMatrix);
		myVRCamera.SetProjectionMatrixRight(cuProjectionMatrix);

		CU::Matrix4x4f cuEyeMatrix;
		VRMatrixToCUMatrix(myVRSystem->GetEyeToHeadTransform(vr::EVREye::Eye_Left), cuEyeMatrix);
		myVRCamera.SetOffsetLeft(CU::Vector3f(cuEyeMatrix[12], cuEyeMatrix[13], cuEyeMatrix[14]));
		VRMatrixToCUMatrix(myVRSystem->GetEyeToHeadTransform(vr::EVREye::Eye_Right), cuEyeMatrix);
		myVRCamera.SetOffsetRight(CU::Vector3f(cuEyeMatrix[12], cuEyeMatrix[13], cuEyeMatrix[14]));

		myHasSetProjection = true;
	}

	float secondsSinceLastVsync;
	myVRSystem->GetTimeSinceLastVsync(&secondsSinceLastVsync, NULL);
	float displayFrequency = myVRSystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
	float frameDuration = 1.f / displayFrequency;
	float vsyncToPhotons = myVRSystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);
	float predictedSecondsFromNow = frameDuration * 2 - secondsSinceLastVsync + vsyncToPhotons;

	vr::TrackedDevicePose_t trackedDevicePoses[vr::k_unMaxTrackedDeviceCount];
	//vr::VRCompositor()->WaitGetPoses(trackedDevicePoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
	myVRSystem->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, predictedSecondsFromNow, trackedDevicePoses, 1);
	CU::Matrix4x4f cuHeadMatrix;
	VRMatrixToCUMatrix(trackedDevicePoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking, cuHeadMatrix);
	myVRCamera.SetTransformMatrix(cuHeadMatrix);

	//std::cout << "camPosX:" << cuHeadMatrix[12] << " camPosY:" << cuHeadMatrix[13] << " camPosZ:" << cuHeadMatrix[14] << std::endl;

	vr::VREvent_t vrEvent;
	bool gotPoll = myVRSystem->PollNextEvent(&vrEvent, sizeof(vr::VREvent_t));
	if (gotPoll)
	{
		switch (vrEvent.eventType)
		{
		case vr::EVREventType::VREvent_ProcessQuit:
		case vr::EVREventType::VREvent_QuitAcknowledged:
			CU::Log("OpenVR quit");
			break;
		case vr::EVREventType::VREvent_ButtonPress:
			CU::Log("Pressed VR button: " + std::to_string(vrEvent.data.controller.button));
			break;
		case vr::EVREventType::VREvent_None:
		default:
			break;
		}
	}
	vr::ChaperoneCalibrationState calibrationState = myVRArea->GetCalibrationState();
	if (calibrationState != vr::ChaperoneCalibrationState_OK)
	{
		switch (calibrationState)
		{
		case vr::ChaperoneCalibrationState_Warning:
			CU::Log("OpenVR CalibrationState warning", CU::LogType::Warning);
			break;
		case vr::ChaperoneCalibrationState_Warning_BaseStationMayHaveMoved:
			CU::Log("OpenVR CalibrationState: base station may have moved", CU::LogType::Warning);
			break;
		case vr::ChaperoneCalibrationState_Warning_BaseStationRemoved:
			CU::Log("OpenVR CalibrationState: base station removed", CU::LogType::Warning);
			break;
		case vr::ChaperoneCalibrationState_Warning_SeatedBoundsInvalid:
			CU::Log("OpenVR CalibrationState: seated bounds invalid", CU::LogType::Warning);
			break;
		case vr::ChaperoneCalibrationState_Error:
			CU::Log("OpenVR CalibrationState error", CU::LogType::Error);
			break;
		case vr::ChaperoneCalibrationState_Error_BaseStationUninitialized:
			CU::Log("OpenVR CalibrationState: base station uninitialized", CU::LogType::Error);
			break;
		case vr::ChaperoneCalibrationState_Error_BaseStationConflict:
			CU::Log("OpenVR CalibrationState: base station conflict", CU::LogType::Error);
			break;
		case vr::ChaperoneCalibrationState_Error_PlayAreaInvalid:
			CU::Log("OpenVR CalibrationState: play area invalid", CU::LogType::Error);
			break;
		case vr::ChaperoneCalibrationState_Error_CollisionBoundsInvalid:
			CU::Log("OpenVR CalibrationState: collision bounds invalid", CU::LogType::Error);
			break;
		default:
			CU::Log("OpenVR CalibrationState not ok", CU::LogType::Warning);
			break;
		}
	}
}

void VRManager::Quit()
{
	if (myVRSystem != nullptr)
	{
		myVRSystem->AcknowledgeQuit_Exiting();
		vr::VR_Shutdown();
		myVRSystem = nullptr;
		myVRArea = nullptr;
		myHasSetProjection = false;
	}
}

const CU::Vector2<unsigned int>& VRManager::GetEyeRenderTargetSize()
{
	return myRenderTargetSize;
}
