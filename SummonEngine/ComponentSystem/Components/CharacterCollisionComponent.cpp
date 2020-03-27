#include "stdafx.h"
#include "CharacterCollisionComponent.h"
#include "TransformComponent.h"

CharacterCollisionComponent::CharacterCollisionComponent(const int aID, const int aObjectID, ComponentManager* aManager) : Component(aID, aObjectID, aManager)
{
}

void CharacterCollisionComponent::Init(const float aHeight, const float aRadius)
{
	const TransformComponent* transform = GetComponent<TransformComponent>();
	if (transform != nullptr)
	{
		myPhysicsObject.InitCapsule(transform->GetTransform(), aHeight, aRadius);
	}
	else
	{
		myPhysicsObject.InitCapsule(CU::Transform(), aHeight, aRadius);
	}
}

void CharacterCollisionComponent::OnAdd()
{
}
void CharacterCollisionComponent::OnActivate()
{
	myPhysicsObject.AddToScene();
}
void CharacterCollisionComponent::OnDeactivate()
{
	myPhysicsObject.RemoveFromScene();
}
void CharacterCollisionComponent::OnRelease()
{
	myPhysicsObject.Release();
}