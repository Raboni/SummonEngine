#pragma once
#include "PhysicsObject.h"
#include <Transform.hpp>

namespace physx
{
	class PxRigidDynamic;
}
class PhysicsMaterial;

class PhysicsDynamic : public PhysicsObject
{
public:
	PhysicsDynamic();
	~PhysicsDynamic();

	void InitBox(const CU::Transform& aTransform, const CU::Vector3f aSize, const unsigned int aIdentityFlags, const unsigned int aCollisionFlags, PhysicsMaterial* aMaterial = nullptr);
	void Release();

	void AddToScene();
	void RemoveFromScene();

	void SetTransform(const CU::Transform& aTransform);
	void SetPosition(const CU::Vector3f aPosition);
	void SetRotation(const CU::Quaternionf aRotation);

	CU::Vector3f GetPosition();
	CU::Quaternionf GetRotation();

private:
	physx::PxRigidDynamic* myPhysicsObject;
	PhysicsMaterial* myMaterial;
};