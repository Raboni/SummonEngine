#include "stdafx.h"
#include "ModelComponent.h"
#include "Engine.h"
#include "TransformComponent.h"

ModelComponent::ModelComponent(const int aID, const int aObjectID, ComponentManager* aManager) : Component(aID, aObjectID, aManager)
{
}

void ModelComponent::Update(const float aDeltaTime)
{
	aDeltaTime;
	TransformComponent const* transform = myManager.GetComponent<TransformComponent>(GetObjectID());
	if (transform != nullptr)
	{
		if (myHasModifiedOffset)
		{
			myInstance.SetPosition(CU::Vector4f(myOffset.GetPosition(), 1.0f) * transform->GetTransform().GetMatrix());
			myInstance.SetRotation(myOffset.GetRotation() * transform->GetRotation());
		}
		else
		{
			myInstance.SetPosition(transform->GetPosition());
			myInstance.SetRotation(transform->GetRotation());
		}
	}
}

void ModelComponent::Init(const std::string& aPath)
{
	myInstance.Init(aPath);
}
void ModelComponent::Init(const std::string& aPath, const std::string& aSkeletonPath)
{
	myInstance.Init(aPath, aSkeletonPath);
}

void ModelComponent::SetOffsetTransform(const CU::Transform& aTransform)
{
	myOffset = aTransform;
	myHasModifiedOffset = true;
}
void ModelComponent::SetOffsetPosition(const CU::Vector3f& aPosition)
{
	myOffset.SetPosition(aPosition);
	myHasModifiedOffset = true;
}
void ModelComponent::SetOffsetRotation(const CU::Quaternionf& aRotation)
{
	myOffset.SetRotation(aRotation);
	myHasModifiedOffset = true;
}
void ModelComponent::ResetOffset()
{
	myOffset.SetPosition(CU::Vector3f::Zero);
	myOffset.SetRotation(CU::Quaternionf::Identity);
	myHasModifiedOffset = false;
}

void ModelComponent::SetScale(const float aScale)
{
	myInstance.SetScale(CU::Vector3f::One * aScale);
}

void ModelComponent::SetVertexShader(const std::string& aShaderPath)
{
	myInstance.SetVertexShader(aShaderPath);
}
void ModelComponent::SetGeometryShader(const std::string& aShaderPath)
{
	myInstance.SetGeometryShader(aShaderPath);
}
void ModelComponent::SetPixelShader(const std::string& aShaderPath)
{
	myInstance.SetPixelShader(aShaderPath);
}

void ModelComponent::OnAdd()
{
	myInstance.SetPosition(CU::Vector3f::Zero);
	myInstance.SetRotation(CU::Vector3f::Zero);
	myInstance.SetScale(CU::Vector3f::One);
	myInstance.SetColor(CU::Vector4f::One);

	ResetOffset();

	OnActivate();
}
void ModelComponent::OnActivate()
{
	//myInstance.Load();
	EngineProxy::GetActiveScene()->AddInstance(&myInstance);
}
void ModelComponent::OnDeactivate()
{
	EngineProxy::GetActiveScene()->RemoveInstance(&myInstance);
}
void ModelComponent::OnRelease()
{
	OnDeactivate();
}