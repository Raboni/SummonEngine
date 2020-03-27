#pragma once
#include "Component.h"

class StaticCollisionComponent : public Component
{
public:
	StaticCollisionComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~StaticCollisionComponent() = default;

protected:
	void OnAdd() override;
	void OnActivate() override;
	void OnDeactivate() override;
	void OnRelease() override;
};