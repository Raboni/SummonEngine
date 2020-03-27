#include "stdafx.h"
#include "ModelRenderCommand.h"
#include "Model.h"
#include "ModelInstance.h"
#include "ModelLoader.h"

ModelRenderCommand::ModelRenderCommand()
{
	myColor = CU::Vector4f::One;
	myScale = CU::Vector3f::One;
	myModel = nullptr;
	myAnimationTime = 0.0f;
	myAnimationIndex = 0;
	myRenderMode = 0;
	myUseAlbedo = true;
}

ModelRenderCommand::ModelRenderCommand(const ModelRenderCommand& aCommand)
{
	memcpy(&myTransform, &aCommand.myTransform, sizeof(CU::Matrix4x4f));
	myColor = aCommand.myColor;
	myScale = aCommand.myScale;
	myAnimationTime = aCommand.myAnimationTime;
	myAnimationIndex = aCommand.myAnimationIndex;
	myRenderMode = aCommand.myRenderMode;
	myUseAlbedo = aCommand.myUseAlbedo;
	myModel = aCommand.myModel;
	/*if (myModel != nullptr)
	{
		myModel->AddUser();
	}*/
}

ModelRenderCommand::ModelRenderCommand(ModelInstance* aInstance)
{
	memcpy(&myTransform, &aInstance->myTransform.GetMatrix(), sizeof(CU::Matrix4x4f));
	myColor = aInstance->myColor;
	myScale = aInstance->myScale;
	myAnimationTime = aInstance->myAnimationTime;
	myAnimationIndex = aInstance->myAnimationIndex;
	myRenderMode = static_cast<short>(aInstance->myRenderMode);
	myUseAlbedo = aInstance->myUseAlbedo;
	myModel = aInstance->myModel;
	/*if (myModel != nullptr)
	{
		myModel->AddUser();
	}*/
}

ModelRenderCommand& ModelRenderCommand::operator=(const ModelRenderCommand& aCommand)
{
	memcpy(&myTransform, &aCommand.myTransform, sizeof(CU::Matrix4x4f));
	myColor = aCommand.myColor;
	myScale = aCommand.myScale;
	myAnimationTime = aCommand.myAnimationTime;
	myAnimationIndex = aCommand.myAnimationIndex;
	myRenderMode = aCommand.myRenderMode;
	myUseAlbedo = aCommand.myUseAlbedo;
	myModel = aCommand.myModel;
	/*if (myModel != nullptr)
	{
		myModel->AddUser();
	}*/
	return *this;
}

ModelRenderCommand& ModelRenderCommand::operator=(ModelInstance* aInstance)
{
	return *this = *aInstance;
}
ModelRenderCommand& ModelRenderCommand::operator=(const ModelInstance& aInstance)
{
	memcpy(&myTransform, &aInstance.myTransform.GetMatrix(), sizeof(CU::Matrix4x4f));
	myColor = aInstance.myColor;
	myScale = aInstance.myScale;
	myAnimationTime = aInstance.myAnimationTime;
	myAnimationIndex = aInstance.myAnimationIndex;
	myRenderMode = static_cast<short>(aInstance.myRenderMode);
	myUseAlbedo = aInstance.myUseAlbedo;
	myModel = aInstance.myModel;
	return *this;
}

ModelRenderCommand::~ModelRenderCommand()
{
	/*if (myModel != nullptr)
	{
		myModel->RemoveUser();
		if (myModel->GetUserCount() <= 0)
		{
			ModelLoader::GetInstance()->UnloadModel(myModel->GetPath());
		}
		myModel = nullptr;
	}*/
}

const CU::Vector3f ModelRenderCommand::GetPosition() const
{
	return CU::Vector3f(myTransform[12], myTransform[13], myTransform[14]);
}
