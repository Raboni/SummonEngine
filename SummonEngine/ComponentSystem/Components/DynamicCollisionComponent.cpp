#include "stdafx.h"
#include "DynamicCollisionComponent.h"
#include "TransformComponent.h"

DynamicCollisionComponent::DynamicCollisionComponent(const int aID, const int aObjectID, ComponentManager* aManager) : Component(aID, aObjectID, aManager)
{
}

void DynamicCollisionComponent::InitBox(const CU::Vector3f& aSize, const unsigned int aIdentityFlags, const unsigned int aCollisionFlags, PhysicsMaterial* aMaterial)
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

void DynamicCollisionComponent::OnAdd()
{
}
void DynamicCollisionComponent::OnActivate()
{
	myPhysicsObject.AddToScene();
}
void DynamicCollisionComponent::OnDeactivate()
{
	myPhysicsObject.RemoveFromScene();
}
void DynamicCollisionComponent::OnRelease()
{
	myPhysicsObject.Release();
}