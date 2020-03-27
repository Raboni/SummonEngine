#pragma once
#include "Component.h"
#include "ModelInstance.h"
#include <Transform.hpp>

class ModelComponent;

class AnimationControllerComponent : public Component
{
public:
	AnimationControllerComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~AnimationControllerComponent() = default;

	void Init();

	void Update(const float aDeltaTime) override;

	void SetAnimationTime(const float aTime);
	void AddAnimation(const std::string& aPath);
	void SetAnimation(const std::string& aName);
	void SetAnimation(const int aAnimationIndex);

protected:
	void OnAdd() override;
	void OnActivate() override;
	void OnDeactivate() override;
	void OnRelease() override;

	void UpdateModelComponent();

private:
	ModelComponent* myModelComponent;
};

