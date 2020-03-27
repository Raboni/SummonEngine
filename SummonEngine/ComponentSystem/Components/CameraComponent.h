#pragma once
#include "Component.h"
#include <Camera.hpp>

class CameraComponent : public Component
{
public:
	CameraComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~CameraComponent() = default;

	void Update(const float aDeltaTime) override;

	CU::Camera* GetCamera();

protected:
	void OnAdd() override;

private:
	CU::Camera myCamera;
};