#pragma once
#include "HitResults.h"
#include <Vector3.hpp>

class PhysicsManager;

class PhysicsInterface
{
public:
	static bool Init();
	static void Destroy();
	static void Update(const float aDeltaTime);
	static int CreateScene(const CU::Vector3f aGravity, const bool aSetCurrentScene = true);
	static void SetCurrentScene(const int aSceneIndex);

	static bool RaycastInstant(const CU::Vector3f aOrigin, const CU::Vector3f aDirection, const float aDistance, RaycastHitResult& outHitResult);

	static float GetLastSimulationDuration();
};