#pragma once
#include "Component.h"
#include "PhysicsTrigger.h"

class TriggerCollisionComponent : public Component
{
public:
	TriggerCollisionComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~TriggerCollisionComponent() = default;

	void InitBox(const CU::Vector3f& aSize, const unsigned int aIdentityFlags, const unsigned int aCollisionFlags, PhysicsMaterial* aMaterial = nullptr);

protected:
	void OnAdd() override;
	void OnActivate() override;
	void OnDeactivate() override;
	void OnRelease() override;

private:
	PhysicsTrigger myPhysicsObject;
};