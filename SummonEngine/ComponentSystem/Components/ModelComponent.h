#pragma once
#include "Component.h"
#include "ModelInstance.h"
#include <Transform.hpp>

class ModelComponent : public Component
{
public:
	ModelComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~ModelComponent() = default;

	void Update(const float aDeltaTime) override;

	void Init(const std::string& aPath);
	void Init(const std::string& aPath, const std::string& aSkeletonPath);

	void SetOffsetTransform(const CU::Transform& aTransform);
	void SetOffsetPosition(const CU::Vector3f& aPosition);
	void SetOffsetRotation(const CU::Quaternionf& aRotation);
	void ResetOffset();

	void SetScale(const float aScale);

	void SetVertexShader(const std::string& aShaderPath);
	void SetGeometryShader(const std::string& aShaderPath);
	void SetPixelShader(const std::string& aShaderPath);

protected:
	friend class AnimationControllerComponent;

	void OnAdd() override;
	void OnActivate() override;
	void OnDeactivate() override;
	void OnRelease() override;

private:
	ModelInstance myInstance;
	CU::Transform myOffset;
	bool myHasModifiedOffset;
};

