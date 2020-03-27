#pragma once
#include <GrowingArray.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Transform.hpp>
#include <string>

class Model;
class VertexShader;
class GeometryShader;
class PixelShader;
class Skeleton;
struct CustomMesh;

enum class ModelRenderMode : short
{
	Deferred,
	DeferredInstanced,
	Forward,
	Wireframe
};

class ModelInstance
{
public:
	static void InsertionSortInstanced(CU::GrowingArray<ModelInstance*, int>& aUnsortedArray);
	static void InsertionSortInstanced(CU::GrowingArray<ModelInstance*, int>& aUnsortedArray, CU::GrowingArray<int, int>& outModelStartIndexList);

	ModelInstance();
	ModelInstance(const std::string& aPath);
	ModelInstance(const std::string& aPath, const std::string& aSkeletonPath);
	ModelInstance(const ModelInstance& aInstance);
	ModelInstance& operator=(const ModelInstance& aInstance);
	~ModelInstance();

	void Init(const std::string& aPath);
	void Init(const std::string& aPath, const std::string& aSkeletonPath);
	void InitCustom(const std::string& aName);
	void Load();
	void Release();

	void UpdateAnimation(const float aDeltaTime);
	void SetAnimationTime(const float aTime);
	void AddAnimation(const std::string& aPath);
	void SetAnimation(const std::string& aName);
	void SetAnimation(const int aAnimationIndex);
	int GetAnimationIndex(const std::string& aName);
	Skeleton* GetSkeleton();

	CustomMesh* GetCustomMesh();
	void MarkCustomMeshForUpdate();

	void Move(const CU::Vector3f& aTranslation);
	void Rotate(const CU::Vector3f& aRotationInRadians);
	void SetTransform(const CU::Transform& aTransform);
	void SetRotation(const CU::Quaternionf& aRotation);
	void SetRotation(const CU::Vector3f& aRotationInRadians);
	void SetPosition(const CU::Vector3f& aPosition);
	void SetScale(const CU::Vector3f& aScale);
	void SetColor(const CU::Vector4f& aColor);
	void SetColor(const CU::Vector3f& aColor);
	void SetAlpha(const float aAlpha);
	void SetRenderMode(const ModelRenderMode aRenderMode);
	void SetRenderDeferred(const bool aRenderDeferred);
	//Can only render instanced when it renders deferred
	void SetRenderInstanced(const bool aRenderInstanced);
	void SetRenderOnMap(const bool aRenderOnMap);
	void SetUseAlbedo(const bool aUseAlbedo);
	void SetVertexShader(const std::string& aShaderPath);
	void SetGeometryShader(const std::string& aShaderPath);
	void SetPixelShader(const std::string& aShaderPath);

	CU::Transform& GetTransform();
	const CU::Transform& GetTransform() const;
	const CU::Quaternionf& GetRotation() const;
	const CU::Vector3f& GetPosition() const;
	const CU::Vector3f& GetScale() const;
	const CU::Vector4f& GetColor() const;
	float GetAlpha() const;
	float GetEncapsulationRange() const;
	ModelRenderMode GetRenderMode() const;
	bool ShouldRenderOnMap() const;
	bool HasCalledLoad() const;
	bool HasModel() const;
	bool IsDeferred() const;
	bool IsInstanced() const;
	bool IsSkinned() const;
	bool IsCustom() const;

private:
	friend class Engine;
	friend class Scene;
	friend struct ModelRenderCommand;

	Model* GetModel();

private:
	CU::Transform myTransform;
	CU::Vector4f myColor;
	CU::Vector3f myScale;
	std::string myPath;
	std::string mySkeletonPath;
	Model* myModel;
	ModelRenderMode myRenderMode;
	float myAnimationTime;
	int myAnimationIndex;
	bool myLoaded;
	bool myRenderOnMap;
	bool myUseAlbedo;
	bool myIsSkinned;
	bool myIsCustom;
};