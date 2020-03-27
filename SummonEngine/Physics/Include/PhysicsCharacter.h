#pragma once
#include "PhysicsObject.h"
#include <Transform.hpp>

namespace physx
{
	class PxControllerManager;
	class PxController;
	class PxCapsuleController;
	class PxBoxController;
}
class PhysicsMaterial;

class PhysicsCharacter : public PhysicsObject
{
public:
	PhysicsCharacter();
	~PhysicsCharacter();

	void InitBox(const CU::Transform& aTransform, const CU::Vector3f aSize, PhysicsMaterial* aMaterial = nullptr);
	void InitCapsule(const CU::Transform& aTransform, const float aHeight, const float aRadius, PhysicsMaterial* aMaterial = nullptr);
	void Release();

	void AddToScene();
	void RemoveFromScene();

	CU::Vector3f GetPosition();
	CU::Vector3f GetSize();

private:
	CU::Vector3f mySize;
	physx::PxControllerManager* myManager;
	physx::PxController* myPhysicsObject;
	PhysicsMaterial* myMaterial;
};

