#pragma once

namespace physx
{
	class PxMaterial;
}

class PhysicsMaterial
{
public:
	PhysicsMaterial();
	PhysicsMaterial(const float aStaticFriction, const float aDynamicFriction, const float aRestitution);
	~PhysicsMaterial();

	void Init(const float aStaticFriction, const float aDynamicFriction, const float aRestitution);

	physx::PxMaterial* GetPxMaterial();

private:
	physx::PxMaterial* myMaterial;
};

