#include "stdafx.h"
#include "TriggerCollisionComponent.h"
#include "TransformComponent.h"

TriggerCollisionComponent::TriggerCollisionComponent(const int aID, const int aObjectID, ComponentManager* aManager) : Component(aID, aObjectID, aManager)
{
}

void TriggerCollisionComponent::InitBox(const CU::Vector3f& aSize, const unsigned int aIdentityFlags, const unsigned int aCollisionFlags, PhysicsMaterial* aMaterial)
{
	TransformComponent* transformComp = GetComponent<TransformComponent>();
	if (transformComp != nullptr)
	{
		myPhysicsObject.InitBox(transformComp->GetTransform(), aSize, aIdentityFlags, aCollisionFlags, aMaterial);
	}
	else
	{
		myPhysicsObject.InitBox(CU::Transform(), aSize, aIdentityFlags, aCollisionFlags, aMaterial);
	}
}

void TriggerCollisionComponent::OnAdd()
{
}
void TriggerCollisionComponent::OnActivate()
{
	myPhysicsObject.AddToScene();
}
void TriggerCollisionComponent::OnDeactivate()
{
	myPhysicsObject.RemoveFromScene();
}
void TriggerCollisionComponent::OnRelease()
{
	myPhysicsObject.Release();
}