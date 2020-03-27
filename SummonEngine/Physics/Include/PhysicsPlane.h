#pragma once
#include <Plane.hpp>
#include <Vector3.hpp>

namespace physx
{
	class PxRigidStatic;
}
class PhysicsMaterial;

class PhysicsPlane
{
public:
	PhysicsPlane();
	~PhysicsPlane();

	void Init(const CU::Vector3f& aNormal, const float aDistance, PhysicsMaterial* aMaterial = nullptr);
	void Init(const CU::Vector3f& aPoint, const CU::Vector3f& aNormal, PhysicsMaterial* aMaterial = nullptr);
	void Init(const CU::Vector3f& aPoint0, const CU::Vector3f& aPoint1, const CU::Vector3f& aPoint2, PhysicsMaterial* aMaterial = nullptr);

	void AddToScene();
	void RemoveFromScene();

private:
	physx::PxRigidStatic* myPhysicsObject;
	PhysicsMaterial* myMaterial;
	
};