#include "stdafx.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "EngineProxy.h"

CameraComponent::CameraComponent(const int aID, const int aObjectID, ComponentManager* aManager) : Component(aID, aObjectID, aManager)
{
}

void CameraComponent::Update(const float aDeltaTime)
{
	aDeltaTime;
	TransformComponent* transform = GetComponent<TransformComponent>();
	if (transform != nullptr)
	{
		myCamera.SetPosition(transform->GetPosition());
		myCamera.SetRotation(transform->GetRotation().GetEulerAngles());
	}
}

CU::Camera* CameraComponent::GetCamera()
{
	return &myCamera;
}

void CameraComponent::OnAdd()
{
	myCamera.SetFoV(3.14f / 3);
	myCamera.SetFarPlane(400.0f);
	myCamera.SetAspectRatio(static_cast<float>(EngineProxy::GetWindowSize().x) / static_cast<float>(EngineProxy::GetWindowSize().y));
}