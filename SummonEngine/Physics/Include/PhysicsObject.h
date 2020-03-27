#pragma once

class PhysicsObject
{
public:
	PhysicsObject();
	~PhysicsObject() = default;

protected:
	friend class PhysicsEventCallback;

	virtual void OnEnter(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags) { aIdentityFlags; aCollisionFlags; };
	virtual void OnStay(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags) { aIdentityFlags; aCollisionFlags; };
	virtual void OnLeave(const unsigned int aIdentityFlags, const unsigned int aCollisionFlags) { aIdentityFlags; aCollisionFlags; };
};