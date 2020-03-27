#include "stdafx.h"
#include "ModelInstance.h"
#include "Model.h"
#include "SkinnedModel.h"
#include "CustomModel.h"
#include "ModelLoader.h"
#include "ShaderLoader.h"

void ModelInstance::InsertionSortInstanced(CU::GrowingArray<ModelInstance*, int>& aUnsortedArray)
{
	int pos = 0;
	ModelInstance* temp;

	for (int i = 1; i < aUnsortedArray.Size(); i++)
	{
		pos = i - 1;
		temp = aUnsortedArray[i];

		while (pos >= 0 && temp->myModel < aUnsortedArray[pos]->myModel)//lowest pointer value at start of array
		{
			aUnsortedArray[pos + 1] = aUnsortedArray[pos];
			pos--;
		}

		aUnsortedArray[pos + 1] = temp;
	}
}

void ModelInstance::InsertionSortInstanced(CU::GrowingArray<ModelInstance*, int>& aUnsortedArray, CU::GrowingArray<int, int>& outModelStartIndexList)
{
	const int arraySize = aUnsortedArray.Size();
	if (arraySize <= 1)
	{
		outModelStartIndexList.Init(1);
		outModelStartIndexList.Add(0);
		return;
	}

	InsertionSortInstanced(aUnsortedArray);
	outModelStartIndexList = CU::GrowingArray<int, int>(arraySize / 2);
	Model* prevModel = nullptr;
	for (int instanceIndex = 0; instanceIndex < arraySize; instanceIndex++)
	{
		if (aUnsortedArray[instanceIndex]->myModel != prevModel)
		{
			outModelStartIndexList.Add(instanceIndex);
			prevModel = aUnsortedArray[instanceIndex]->myModel;
		}
	}
}

ModelInstance::ModelInstance()
{
	myColor = CU::Vector4f::One;
	myScale = CU::Vector3f::One;
	myPath = "";
	mySkeletonPath = "";
	myModel = nullptr;
	myRenderMode = ModelRenderMode::DeferredInstanced;
	myAnimationTime = 0.0f;
	myAnimationIndex = 0;
	myLoaded = false;
	myRenderOnMap = true;
	myUseAlbedo = true;
	myIsSkinned = false;
	myIsCustom = false;
}
ModelInstance::ModelInstance(const std::string& aPath)
{
	myColor = CU::Vector4f::One;
	myScale = CU::Vector3f::One;
	myModel = nullptr;
	myRenderMode = ModelRenderMode::DeferredInstanced;
	myAnimationTime = 0.0f;
	myAnimationIndex = 0;
	myLoaded = false;
	myRenderOnMap = true;
	myUseAlbedo = true;
	Init(aPath);
}
ModelInstance::ModelInstance(const std::string& aPath, const std::string& aSkeletonPath)
{
	myColor = CU::Vector4f::One;
	myScale = CU::Vector3f::One;
	myModel = nullptr;
	myRenderMode = ModelRenderMode::Deferred;
	myAnimationTime = 0.0f;
	myAnimationIndex = 0;
	myLoaded = false;
	myRenderOnMap = true;
	myUseAlbedo = true;
	Init(aPath, aSkeletonPath);
}
ModelInstance::ModelInstance(const ModelInstance& aInstance)
{
	memcpy(&myTransform, &aInstance.myTransform, sizeof(CU::Transform));
	myColor = aInstance.myColor;
	myScale = aInstance.myScale;
	myPath = aInstance.myPath;
	mySkeletonPath = aInstance.mySkeletonPath;
	myRenderMode = aInstance.myRenderMode;
	myAnimationTime = aInstance.myAnimationTime;
	myAnimationIndex = aInstance.myAnimationIndex;
	myLoaded = aInstance.myLoaded;
	myRenderOnMap = aInstance.myRenderOnMap;
	myUseAlbedo = aInstance.myUseAlbedo;
	myIsSkinned = aInstance.myIsSkinned;
	myIsCustom = aInstance.myIsCustom;
	myModel = aInstance.myModel;
	if (myModel != nullptr)
	{
		myModel->AddUser();
	}
	else
	{
		myLoaded = false;
	}
}
ModelInstance& ModelInstance::operator=(const ModelInstance& aInstance)
{
	Release();
	memcpy(&myTransform, &aInstance.myTransform, sizeof(CU::Transform));
	myColor = aInstance.myColor;
	myScale = aInstance.myScale;
	myPath = aInstance.myPath;
	mySkeletonPath = aInstance.mySkeletonPath;
	myRenderMode = aInstance.myRenderMode;
	myAnimationTime = aInstance.myAnimationTime;
	myAnimationIndex = aInstance.myAnimationIndex;
	myLoaded = aInstance.myLoaded;
	myRenderOnMap = aInstance.myRenderOnMap;
	myUseAlbedo = aInstance.myUseAlbedo;
	myIsSkinned = aInstance.myIsSkinned;
	myIsCustom = aInstance.myIsCustom;
	myModel = aInstance.myModel;
	if (myModel != nullptr)
	{
		myModel->AddUser();
	}
	else
	{
		myLoaded = false;
	}
	return *this;
}
ModelInstance::~ModelInstance()
{
	Release();
}

void ModelInstance::Init(const std::string& aPath)
{
	if (aPath != "")
	{
		Release();
	}
	myPath = aPath;
	mySkeletonPath.clear();
	myRenderMode = ModelRenderMode::DeferredInstanced;
	myIsSkinned = false;
	myIsCustom = false;
}
void ModelInstance::Init(const std::string& aPath, const std::string& aSkeletonPath)
{
	if (aPath != "")
	{
		Release();
	}
	myPath = aPath;
	mySkeletonPath = aSkeletonPath;
	myRenderMode = ModelRenderMode::Deferred;
	myIsSkinned = true;
	myIsCustom = false;
}
void ModelInstance::InitCustom(const std::string& aName)
{
	if (aName != "")
	{
		Release();
	}
	myPath = aName;
	mySkeletonPath.clear();
	myRenderMode = ModelRenderMode::Deferred;
	myIsSkinned = false;
	myIsCustom = true;
}

void ModelInstance::Load()
{
	if (ModelLoader::GetInstance() == nullptr)
	{
		return;
	}

	Release();
	if (mySkeletonPath.length() > 0)
	{
		myModel = ModelLoader::GetInstance()->GetSkinnedModel(myPath, mySkeletonPath, reinterpret_cast<SkinnedModel**>(&myModel));
	}
	else if (myIsCustom)
	{
		myModel = ModelLoader::GetInstance()->GetCustomModel(myPath, reinterpret_cast<CustomModel**>(&myModel));
	}
	else
	{
		myModel = ModelLoader::GetInstance()->GetModel(myPath, &myModel);
	}
	myLoaded = true;
}

void ModelInstance::Release()
{
	if (ModelLoader::GetInstance() == nullptr)
	{
		return;
	}

	if (myModel != nullptr)
	{
		myModel->RemoveUser();
		if (myModel->GetUserCount() <= 0)
		{
			if (mySkeletonPath.length() > 0)
			{
				ModelLoader::GetInstance()->UnloadModel(myPath, mySkeletonPath);
			}
			else
			{
				ModelLoader::GetInstance()->UnloadModel(myPath);
			}
		}
		myModel = nullptr;
	}
	myLoaded = false;
}

void ModelInstance::UpdateAnimation(const float aDeltaTime)
{
	if (!myIsSkinned)
	{
		return;
	}

	myAnimationTime += aDeltaTime;
	if (myModel != nullptr)
	{
		SkinnedModel* skinnedModel = static_cast<SkinnedModel*>(myModel);
		const float duration = skinnedModel->GetAnimationDuration(myAnimationIndex);
		if (myAnimationTime >= duration)
		{
			myAnimationTime -= duration;
		}
	}
}

void ModelInstance::SetAnimationTime(const float aTime)
{
	myAnimationTime = aTime;
}

void ModelInstance::AddAnimation(const std::string& aPath)
{
	if (!myIsSkinned)
	{
		return;
	}
	ModelLoader::GetInstance()->AddAnimations(aPath, reinterpret_cast<SkinnedModel**>(&myModel));
}
void ModelInstance::SetAnimation(const std::string& aName)
{
	myAnimationIndex = GetAnimationIndex(aName);
}
void ModelInstance::SetAnimation(const int aAnimationIndex)
{
	myAnimationIndex = aAnimationIndex;
}
int ModelInstance::GetAnimationIndex(const std::string& aName)
{
	if (!myIsSkinned || myModel == nullptr)
	{
		return -1;
	}
	SkinnedModel const* skinnedModel = static_cast<SkinnedModel*>(myModel);
	return skinnedModel->GetAnimationIndex(aName);
}

Skeleton* ModelInstance::GetSkeleton()
{
	if (!myIsSkinned || myModel == nullptr)
	{
		return nullptr;
	}
	SkinnedModel const* skinnedModel = static_cast<SkinnedModel*>(myModel);
	return skinnedModel->mySkeleton;
}

CustomMesh* ModelInstance::GetCustomMesh()
{
	if (!myIsCustom || myModel == nullptr)
	{
		return nullptr;
	}

	CustomModel* customModel = static_cast<CustomModel*>(myModel);
	return &customModel->myCustomMesh;
}

void ModelInstance::MarkCustomMeshForUpdate()
{
	if (!myIsCustom || myModel == nullptr)
	{
		return;
	}

	CustomModel* customModel = static_cast<CustomModel*>(myModel);
	customModel->MarkForUpdate();
}

void ModelInstance::Move(const CU::Vector3f& aMovement)
{
	myTransform.Translate(aMovement);
}
void ModelInstance::Rotate(const CU::Vector3f& aRotationInRadians)
{
	myTransform.Rotate(aRotationInRadians);
}
void ModelInstance::SetTransform(const CU::Transform& aTransform)
{
	myTransform = aTransform;
}
void ModelInstance::SetRotation(const CU::Quaternionf& aRotation)
{
	myTransform.SetRotation(aRotation);
}
void ModelInstance::SetRotation(const CU::Vector3f& aRotationInRadians)
{
	myTransform.SetRotation(aRotationInRadians);
}
void ModelInstance::SetPosition(const CU::Vector3f& aPosition)
{
	myTransform.SetPosition(aPosition);
}

void ModelInstance::SetScale(const CU::Vector3f& aScale)
{
	myScale = aScale;
}

void ModelInstance::SetColor(const CU::Vector4f& aColor)
{
	myColor = aColor;
}
void ModelInstance::SetColor(const CU::Vector3f& aColor)
{
	myColor = CU::Vector4f(aColor, myColor.w);
}
void ModelInstance::SetAlpha(const float aAlpha)
{
	myColor.w = aAlpha;
}

void ModelInstance::SetRenderMode(const ModelRenderMode aRenderMode)
{
	myRenderMode = aRenderMode;
}
void ModelInstance::SetRenderDeferred(const bool aRenderDeferred)
{
	if (!aRenderDeferred)
	{
		myRenderMode = ModelRenderMode::Forward;
	}
	else if (!IsDeferred())
	{
		myRenderMode = ModelRenderMode::Deferred;
	}
}
void ModelInstance::SetRenderInstanced(const bool aRenderInstanced)
{
	if (aRenderInstanced)
	{
		myRenderMode = ModelRenderMode::DeferredInstanced;
	}
	else if (IsInstanced())
	{
		myRenderMode = ModelRenderMode::Deferred;
	}
}
void ModelInstance::SetRenderOnMap(const bool aRenderOnMap)
{
	myRenderOnMap = aRenderOnMap;
}
void ModelInstance::SetUseAlbedo(const bool aUseAlbedo)
{
	myUseAlbedo = aUseAlbedo;
}

void ModelInstance::SetVertexShader(const std::string& aShaderPath)
{
	if (myModel == nullptr)
	{
		return;
	}

	myModel->myVertexShader = ShaderLoader::GetInstance()->GetVertexShader(aShaderPath);
}
void ModelInstance::SetGeometryShader(const std::string& aShaderPath)
{
	if (myModel == nullptr)
	{
		return;
	}

	myModel->myGeometryShader = ShaderLoader::GetInstance()->GetGeometryShader(aShaderPath);
}
void ModelInstance::SetPixelShader(const std::string& aShaderPath)
{
	if (myModel == nullptr)
	{
		return;
	}

	myModel->myPixelShader = ShaderLoader::GetInstance()->GetPixelShader(aShaderPath);
}

CU::Transform& ModelInstance::GetTransform()
{
	return myTransform;
}
const CU::Transform& ModelInstance::GetTransform() const
{
	return myTransform;
}
const CU::Quaternionf& ModelInstance::GetRotation() const
{
	return myTransform.GetRotation();
}
const CU::Vector3f& ModelInstance::GetPosition() const
{
	return myTransform.GetPosition();
}

const CU::Vector3f& ModelInstance::GetScale() const
{
	return myScale;
}

const CU::Vector4f& ModelInstance::GetColor() const
{
	return myColor;
}
float ModelInstance::GetAlpha() const
{
	return myColor.w;
}

float ModelInstance::GetEncapsulationRange() const
{
	if (myModel == nullptr)
	{
		return 0.0f;
	}
	return myModel->GetEncapsulationRange();
}

ModelRenderMode ModelInstance::GetRenderMode() const
{
	return myRenderMode;
}
bool ModelInstance::ShouldRenderOnMap() const
{
	return myRenderOnMap;
}

bool ModelInstance::HasCalledLoad() const
{
	return myLoaded;
}
bool ModelInstance::HasModel() const
{
	return myModel != nullptr;
}
bool ModelInstance::IsDeferred() const
{
	return myRenderMode == ModelRenderMode::Deferred || myRenderMode == ModelRenderMode::DeferredInstanced;
}
bool ModelInstance::IsInstanced() const
{
	return myRenderMode == ModelRenderMode::DeferredInstanced;
}
bool ModelInstance::IsSkinned() const
{
	return myIsSkinned;
}
bool ModelInstance::IsCustom() const
{
	return myIsCustom;
}

Model* ModelInstance::GetModel()
{
	return myModel;
}