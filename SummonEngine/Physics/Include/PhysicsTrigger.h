#pragma once
#include "PhysicsObject.h"
#include <Transform.hpp>
#include <functional>

namespace physx
{
	class PxRigidDynamic;
}
class PhysicsMaterial;

class PhysicsTrigger : public PhysicsObject
{
public:
	PhysicsTrigger();
	~PhysicsTrigger();

	void InitBox(const CU::Transform& aTransform, const CU::Vector3f aSize, const unsigned int aIdentityFlags, const unsigned int aCollisionFlags, PhysicsMaterial* aMaterial = nullptr);
	void Release();

	void AddToScene();
	void RemoveFromScene();

	void SetOnEnterCallback(const std::function<void(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags)>& aFunction);
	void SetOnLeaveCallback(const std::function<void(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags)>& aFunction);

	void SetTransform(const CU::Transform& aTransform);
	void SetPosition(const CU::Vector3f aPosition);
	void SetRotation(const CU::Quaternionf aRotation);

	CU::Vector3f GetPosition();
	CU::Quaternionf GetRotation();

protected:
	virtual void OnEnter(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags) override;
	//virtual void OnStay(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags) override;
	virtual void OnLeave(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags) override;

private:
	std::function<void(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags)> myOnEnterCallback;
	std::function<void(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags)> myOnLeaveCallback;
	physx::PxRigidDynamic* myPhysicsObject;
	PhysicsMaterial* myMaterial;
};