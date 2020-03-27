#pragma once
#include "ComponentManager.h"
#include "ModelInstance.h"
#include "TextInstance.h"
#include "LineRenderCommand.h"
#include "Terrain.h"

#include <Camera.hpp>
#include <State.hpp>
#include <Vector.hpp>

class ComponentManager;

enum class TerrainEditTool
{
	Select,
	Move,
	Rotate,
	Scale,
	MergeVertex,
	SplitEdge,
	DivideTriangle,
	ExtrudeTriangle
};
enum class TerrainEditMode
{
	Vertex,
	Edge,
	Triangle
};

class LevelEditorState : public CU::State
{
public:
	LevelEditorState(ComponentManager& aComponentManager);
	~LevelEditorState() = default;

protected:
	CU::StateAction Update(const float aDeltaTime) override;

	void Load() override;
	void Unload() override;
	void GainFocus() override;
	void LoseFocus() override;

private:
	void UpdateTerrainMesh();

	void UpdateSelect();
	void UpdateMove(const float aDeltaTime);
	void UpdateRotate(const float aDeltaTime);
	void UpdateScale(const float aDeltaTime);
	void UpdateExtrude(const float aDeltaTime);

	void UpdateSelectModel(const TerrainEditMode aMode, ModelInstance& aSelectModel, const int aSelectIndex);

	void RotateAroundPoint(const CU::Vector3f aPoint, const CU::Vector3f aRotationAxis, const float aRotation, const TerrainEditMode aMode, const int aSelectIndex);
	void ScaleFromPoint(const CU::Vector3f aPoint, const CU::Vector3f aScaleDirection, const float aScale, const TerrainEditMode aMode, const int aSelectIndex);
	CU::Vector3f GetTriangleNormal(const int aTriangleIndex);

	CU::GrowingArray<int>& GetEditModeSelectedIndices(const TerrainEditMode aMode);
	CU::GrowingArray<ModelInstance>& GetEditModeSelectedModels(const TerrainEditMode aMode);
	ModelInstance& GetEditModeHighlightedModel(const TerrainEditMode aMode);
	CU::Vector3f GetEditModeMiddlePoint(const TerrainEditMode aMode, const CU::GrowingArray<int>& aIndexArray);

	void GrowSelectedModels(const TerrainEditMode aMode, const int aSize);
	void GrowSelectedVertexModels(const int aSize);
	void GrowSelectedEdgeModels(const int aSize);
	void GrowSelectedTriangleModels(const int aSize);

	void ShowSelectedVertexModels();
	void ShowSelectedEdgeModels();
	void ShowSelectedTriangleModels();
	void ShowGizmoMove(const CU::Vector3f aPosition);

	void HideSelectedVertexModels();
	void HideSelectedEdgeModels();
	void HideSelectedTriangleModels();
	void HideGizmoMove();

	int GetRayHitGizmoMoveAxis(const CU::Intersection::LineRay3D& aRay);

private:
	ComponentManager& myComponentManager;
	CU::Camera myCamera;
	Terrain myTerrain;
	CU::GrowingArray<int> mySelectedVertexIndices;
	CU::GrowingArray<int> mySelectedEdgeIndices;
	CU::GrowingArray<int> mySelectedTriangleIndices;
	CU::GrowingArray<ModelInstance> mySelectedVertexModels;
	CU::GrowingArray<ModelInstance> mySelectedEdgeModels;
	CU::GrowingArray<ModelInstance> mySelectedTriangleModels;
	ModelInstance myTerrainModel;
	ModelInstance myHighlightedTriangle;
	ModelInstance myHighlightedEdge;
	ModelInstance myHighlightedVertex;

	ModelInstance myGizmoMoveXModel;
	ModelInstance myGizmoMoveYModel;
	ModelInstance myGizmoMoveZModel;
	ModelInstance myGizmoDirectionModel;

	TextInstance myDebugText;
	TextInstance myModeText;
	TextInstance myToolText;
	LineRenderCommand myRay;
	CU::Vector2f myDragLocation;
	int myDragAxis;
	TerrainEditMode myTerrainEditMode;
	TerrainEditTool myTerrainEditTool;
	bool myHasExtruded;
};