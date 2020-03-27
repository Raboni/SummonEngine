#pragma once
#include <Matrix4x4.hpp>
#include <Vector3.hpp>

class Model;
class ModelInstance;

struct ModelRenderCommand
{
	ModelRenderCommand();
	ModelRenderCommand(const ModelRenderCommand& aCommand);
	ModelRenderCommand(ModelInstance* aInstance);
	ModelRenderCommand& operator=(const ModelRenderCommand& aCommand);
	ModelRenderCommand& operator=(const ModelInstance& aInstance);
	ModelRenderCommand& operator=(ModelInstance* aInstance);
	~ModelRenderCommand();

	const CU::Vector3f GetPosition() const;

	CU::Matrix4x4f myTransform;
	CU::Vector4f myColor;
	CU::Vector3f myScale;
	Model* myModel;
	float myAnimationTime;
	int myAnimationIndex;
	short myRenderMode;
	bool myUseAlbedo;
};