#include "stdafx.h"
#include "AnimationControllerComponent.h"
#include "Engine.h"
#include "ModelComponent.h"
#include <DebugTools.h>

AnimationControllerComponent::AnimationControllerComponent(const int aID, const int aObjectID, ComponentManager* aManager) : Component(aID, aObjectID, aManager)
{
	myModelComponent = nullptr;
}

void AnimationControllerComponent::Init()
{
	UpdateModelComponent();
}

void AnimationControllerComponent::Update(const float aDeltaTime)
{
	if (myModelComponent == nullptr || myModelComponent->GetObjectID() != GetObjectID())
	{
		UpdateModelComponent();
	}

	if (myModelComponent != nullptr)
	{
		myModelComponent->myInstance.UpdateAnimation(aDeltaTime);
	}
}

void AnimationControllerComponent::SetAnimationTime(const float aTime)
{
	if (myModelComponent != nullptr)
	{
		myModelComponent->myInstance.SetAnimationTime(aTime);
	}
}
void AnimationControllerComponent::AddAnimation(const std::string& aPath)
{
	if (myModelComponent != nullptr)
	{
		if (!myModelComponent->myInstance.HasCalledLoad())
		{
			myModelComponent->myInstance.Load();
		}
		myModelComponent->myInstance.AddAnimation(aPath);
	}
}
void AnimationControllerComponent::SetAnimation(const std::string& aName)
{
	if (myModelComponent != nullptr)
	{
		myModelComponent->myInstance.SetAnimation(aName);
	}
}
void AnimationControllerComponent::SetAnimation(const int aAnimationIndex)
{
	if (myModelComponent != nullptr)
	{
		myModelComponent->myInstance.SetAnimation(aAnimationIndex);
	}
}

void AnimationControllerComponent::OnAdd()
{
	myModelComponent = nullptr;
	OnActivate();
}
void AnimationControllerComponent::OnActivate()
{
	UpdateModelComponent();
}
void AnimationControllerComponent::OnDeactivate()
{
}
void AnimationControllerComponent::OnRelease()
{
	OnDeactivate();
}

void AnimationControllerComponent::UpdateModelComponent()
{
	myModelComponent = GetComponent<ModelComponent>();
}