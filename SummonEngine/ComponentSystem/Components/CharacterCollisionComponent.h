#pragma once
#include "Component.h"
#include "PhysicsCharacter.h"

class CharacterCollisionComponent : public Component
{
public:
	CharacterCollisionComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~CharacterCollisionComponent() = default;

	void Init(const float aHeight, const float aRadius);

protected:
	void OnAdd() override;
	void OnActivate() override;
	void OnDeactivate() override;
	void OnRelease() override;

private:
	PhysicsCharacter myPhysicsObject;
};