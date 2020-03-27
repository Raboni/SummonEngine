#pragma once
#include "Component.h"
#include "PhysicsDynamic.h"

class DynamicCollisionComponent : public Component
{
public:
	DynamicCollisionComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~DynamicCollisionComponent() = default;

	void InitBox(const CU::Vector3f& aSize, const unsigned int aIdentityFlags, const unsigned int aCollisionFlags, PhysicsMaterial* aMaterial = nullptr);

protected:
	void OnAdd() override;
	void OnActivate() override;
	void OnDeactivate() override;
	void OnRelease() override;

private:
	PhysicsDynamic myPhysicsObject;
};