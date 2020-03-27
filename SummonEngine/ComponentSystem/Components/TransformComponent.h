#pragma once
#include "Component.h"
#include <Transform.hpp>

class TransformComponent : public Component
{
public:
	TransformComponent(const int aID, const int aObjectID, ComponentManager* aManager);
	~TransformComponent() = default;

	void Rotate(const CU::Quaternionf& aRotation);
	void Rotate(const CU::Vector3f& aRotationInRadians);
	void Rotate(const CU::Vector3f& aAxis, const float aRadianAngle);
	void MoveLocal(const CU::Vector3f& aMovement);
	void MoveWorld(const CU::Vector3f& aMovement);

	void SetMatrix(const CU::Matrix4x4f& aMatrix);
	void SetRotation(const CU::Quaternionf& aRotation);
	void SetRotation(const CU::Vector3f& aRotationInRadians);
	void SetPosition(const CU::Vector3f& aPosition);
	void SetPositionLocal(const CU::Vector3f& aPosition);

	void SetParent(const int aObjectID);

	const CU::Transform& GetTransform() const;
	CU::Quaternionf GetRotation() const;
	const CU::Quaternionf& GetRotationLocal() const;
	CU::Vector3f GetPosition() const;
	const CU::Vector3f& GetPositionLocal() const;

protected:
	void OnAdd() override;

private:
	CU::Transform myTransform;
	int myParent;
};

