#include "stdafx.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent(const int aID, const int aObjectID, ComponentManager* aManager) : Component(aID, aObjectID, aManager)
{
}

void TransformComponent::Rotate(const CU::Quaternionf& aRotation)
{
	myTransform.Rotate(aRotation);
}
void TransformComponent::Rotate(const CU::Vector3f& aRotationInRadians)
{
	myTransform.Rotate(aRotationInRadians);
}
void TransformComponent::Rotate(const CU::Vector3f& aAxis, const float aRadianAngle)
{
	myTransform.Rotate(aAxis, aRadianAngle);
}
void TransformComponent::MoveLocal(const CU::Vector3f& aMovement)
{
	myTransform.Translate(aMovement);
}
void TransformComponent::MoveWorld(const CU::Vector3f& aMovement)
{
	CU::Vector3f movement = aMovement;
	GetRotation().RotateVector(movement);
	MoveLocal(movement);
}

void TransformComponent::SetMatrix(const CU::Matrix4x4f& aMatrix)
{
	myTransform.SetMatrix(aMatrix);
}
void TransformComponent::SetRotation(const CU::Quaternionf& aRotation)
{
	myTransform.SetRotation(aRotation);
}
void TransformComponent::SetRotation(const CU::Vector3f& aRotation)
{
	myTransform.SetRotation(aRotation);
}
void TransformComponent::SetPosition(const CU::Vector3f& aPosition)
{
	myTransform.SetPosition(aPosition);
}
void TransformComponent::SetPositionLocal(const CU::Vector3f& aPosition)
{
	myTransform.SetPosition(aPosition);
}

void TransformComponent::SetParent(const int aObjectID)
{
	myParent = aObjectID;
}

const CU::Transform& TransformComponent::GetTransform() const
{
	return myTransform;
}
CU::Quaternionf TransformComponent::GetRotation() const
{
	if (myParent == -1)
	{
		return GetRotationLocal();
	}

	CU::Quaternionf rotation = myTransform.GetRotation();
	int nextParent = myParent;
	TransformComponent const* parentComponent = nullptr;
	while (nextParent != -1)
	{
		parentComponent = myManager.GetComponent<TransformComponent>(nextParent);
		if (parentComponent == nullptr)
		{
			break;
		}
		rotation = rotation * parentComponent->GetRotationLocal();
		nextParent = parentComponent->myParent;
	}
	return rotation;
}
const CU::Quaternionf& TransformComponent::GetRotationLocal() const
{
	return myTransform.GetRotation();
}
CU::Vector3f TransformComponent::GetPosition() const
{
	if (myParent == -1)
	{
		return GetPositionLocal();
	}

	CU::Vector4f position = CU::Vector4f(myTransform.GetPosition(), 1.0f);
	int nextParent = myParent;
	TransformComponent const* parentComponent = nullptr;
	while (nextParent != -1)
	{
		parentComponent = myManager.GetComponent<TransformComponent>(nextParent);
		if (parentComponent == nullptr)
		{
			break;
		}
		position = position * parentComponent->GetTransform().GetMatrix();
		nextParent = parentComponent->myParent;
	}
	return CU::Vector3(position);
}

const CU::Vector3f& TransformComponent::GetPositionLocal() const
{
	return myTransform.GetPosition();
}

void TransformComponent::OnAdd()
{
	myTransform.SetPosition(CU::Vector3f::Zero);
	myTransform.SetRotation(CU::Quaternionf::Identity);
	//myTransform.SetMatrix(CU::Matrix4x4f());
	myParent = -1;
}