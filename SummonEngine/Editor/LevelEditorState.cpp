#include "stdafx.h"
#include "LevelEditorState.h"
#include "EngineProxy.h"
#include "SpriteUnits.h"

#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"
#include "Components/DynamicCollisionComponent.h"

#include "CustomModel.h"
#include <Input.hpp>

LevelEditorState::LevelEditorState(ComponentManager& aComponentManager) : myComponentManager(aComponentManager)
{
}

CU::StateAction LevelEditorState::Update(const float aDeltaTime)
{
	CU::Input* input = CU::Input::Get();

	EngineProxy::GetActiveScene()->RemoveInstance(&myHighlightedTriangle);
	EngineProxy::GetActiveScene()->RemoveInstance(&myHighlightedEdge);
	EngineProxy::GetActiveScene()->RemoveInstance(&myHighlightedVertex);
	EngineProxy::GetActiveScene()->RemoveInstance(&myGizmoDirectionModel);
	HideGizmoMove();
	switch (myTerrainEditTool)
	{
	case TerrainEditTool::Select:
		UpdateSelect();
		break;
	case TerrainEditTool::Move:
		UpdateMove(aDeltaTime);
		break;
	case TerrainEditTool::Rotate:
		UpdateRotate(aDeltaTime);
		break;
	case TerrainEditTool::Scale:
		UpdateScale(aDeltaTime);
		break;
	case TerrainEditTool::MergeVertex:
	{
		const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
		CU::Vector3f rayHitPoint;
		const int vertexHit = myTerrain.GetRayHitVertexIndex(ray.point, ray.direction * ray.range, rayHitPoint);
		if (vertexHit != -1)
		{
			const int hitFoundIndex = mySelectedVertexIndices.Find(vertexHit);
			if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
			{
				if (mySelectedVertexIndices.Size() == 0)
				{
					mySelectedVertexIndices.Add(vertexHit);
					mySelectedVertexModels[0].SetPosition(myTerrain.GetVertices()[vertexHit]);
					EngineProxy::GetActiveScene()->AddInstance(&mySelectedVertexModels[0]);
				}
				else
				{
					if (hitFoundIndex != -1)
					{
						mySelectedVertexIndices.RemoveCyclicAtIndex(hitFoundIndex);
						EngineProxy::GetActiveScene()->RemoveInstance(&mySelectedVertexModels[mySelectedVertexIndices.Size()]);
					}
					for (int index = 0; index < mySelectedVertexIndices.Size(); index++)
					{
						EngineProxy::GetActiveScene()->RemoveInstance(&mySelectedVertexModels[index]);
						myTerrain.MergeVertices(mySelectedVertexIndices[index], vertexHit);
					}
					mySelectedVertexIndices.RemoveAll();
					UpdateTerrainMesh();
				}
			}
			else if (hitFoundIndex == -1)
			{
				myHighlightedVertex.SetPosition(myTerrain.GetVertices()[vertexHit]);
				myHighlightedVertex.SetColor(CU::Vector3f(0.0f, 1.0f, 0.0f));
				EngineProxy::GetActiveScene()->AddInstance(&myHighlightedVertex);
			}
		}
		break;
	}
	case TerrainEditTool::SplitEdge:
	{
		const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
		CU::Vector3f rayHitPoint;
		const int edgeHit = myTerrain.GetRayHitEdgeIndex(ray.point, ray.direction * ray.range, rayHitPoint);
		if (edgeHit != -1)
		{
			if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
			{
				myTerrain.AddEdgeVertex(rayHitPoint, edgeHit);
				UpdateTerrainMesh();
			}
			else
			{
				/*const CU::StaticArray<CU::Vector3f, 2> edge(myTerrain.GetEdgeVertices(edgeHit));
				myHighlightedEdge.myFirstPosition = CU::Vector4f(edge[0], 1.0f);
				myHighlightedEdge.mySecondPosition = CU::Vector4f(edge[1], 1.0f);
				EngineProxy::Draw(myHighlightedEdge);*/
				myHighlightedVertex.SetPosition(rayHitPoint);
				myHighlightedVertex.SetColor(CU::Vector3f(0.0f, 1.0f, 0.0f));
				EngineProxy::GetActiveScene()->AddInstance(&myHighlightedVertex);
			}
		}
		break;
	}
	case TerrainEditTool::DivideTriangle:
	{
		const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
		CU::Vector3f rayHitPoint;
		const int triangleHit = myTerrain.GetRayHitTriangleIndex(ray.point, ray.direction * ray.range, rayHitPoint);
		if (triangleHit != -1)
		{
			if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
			{
				myTerrain.AddTriangleVertex(rayHitPoint, triangleHit);
				UpdateTerrainMesh();
			}
			else
			{
				const CU::Vector3f down(0.0f, -0.01f, 0.0f);
				const CU::Vector3f up(0.0f, 0.01f, 0.0f);
				const CU::StaticArray<CU::Vector3f, 3> triangle(myTerrain.GetTriangleVertices(triangleHit));
				CU::GrowingArray<CU::Vector4f>& vertices = myHighlightedTriangle.GetCustomMesh()->myVertices;
				vertices[0] = CU::Vector4f(triangle[0] + down, 1.0f);
				vertices[1] = CU::Vector4f(triangle[0] + up, 1.0f);
				vertices[2] = CU::Vector4f(triangle[1] + down, 1.0f);
				vertices[3] = CU::Vector4f(triangle[1] + up, 1.0f);
				vertices[4] = CU::Vector4f(triangle[2] + down, 1.0f);
				vertices[5] = CU::Vector4f(triangle[2] + up, 1.0f);
				myHighlightedTriangle.MarkCustomMeshForUpdate();
				myHighlightedTriangle.SetColor(CU::Vector3f(0.0f, 1.0f, 0.0f));
				myHighlightedVertex.SetPosition(rayHitPoint);
				myHighlightedVertex.SetColor(CU::Vector3f(0.0f, 1.0f, 0.0f));
				EngineProxy::GetActiveScene()->AddInstance(&myHighlightedTriangle);
				EngineProxy::GetActiveScene()->AddInstance(&myHighlightedVertex);
			}
		}
		break;
	}
	case TerrainEditTool::ExtrudeTriangle:
		UpdateExtrude(aDeltaTime);
		break;
	default:
		break;
	}

	if (input->GetKeyDown(CU::Input::KeyCode::W) || input->GetXboxDown(CU::Input::XboxButton::DPadUp))
	{
		myCamera.Move(CU::Vector3f(0.0f, 0.0f, 2.0f * aDeltaTime));
	}
	if (input->GetKeyDown(CU::Input::KeyCode::S) || input->GetXboxDown(CU::Input::XboxButton::DPadDown))
	{
		myCamera.Move(CU::Vector3f(0.0f, 0.0f, -2.0f * aDeltaTime));
	}
	if (input->GetKeyDown(CU::Input::KeyCode::A) || input->GetXboxDown(CU::Input::XboxButton::DPadLeft))
	{
		myCamera.Move(CU::Vector3f(-2.0f * aDeltaTime, 0.0f, 0.0f));
	}
	if (input->GetKeyDown(CU::Input::KeyCode::D) || input->GetXboxDown(CU::Input::XboxButton::DPadRight))
	{
		myCamera.Move(CU::Vector3f(2.0f * aDeltaTime, 0.0f, 0.0f));
	}
	if (input->GetKeyDown(CU::Input::KeyCode::Up))
	{
		myCamera.Move(CU::Vector3f(0.0f, 2.0f * aDeltaTime, 0.0f));
	}
	if (input->GetKeyDown(CU::Input::KeyCode::Down))
	{
		myCamera.Move(CU::Vector3f(0.0f, -2.0f * aDeltaTime, 0.0f));
	}
	if (input->GetKeyDown(CU::Input::KeyCode::Left))
	{
		myCamera.Rotate(CU::Vector3f(0.0f, 1.5f * aDeltaTime, 0.0f));
	}
	if (input->GetKeyDown(CU::Input::KeyCode::Right))
	{
		myCamera.Rotate(CU::Vector3f(0.0f, -1.5f * aDeltaTime, 0.0f));
	}

	if (input->GetKeyPressed(CU::Input::KeyCode::NumPad1))
	{
		HideSelectedVertexModels();
		HideSelectedEdgeModels();
		HideSelectedTriangleModels();
		ShowSelectedVertexModels();
		mySelectedEdgeIndices.RemoveAll();
		mySelectedTriangleIndices.RemoveAll();
		myDragAxis = -1;

		myTerrainEditMode = TerrainEditMode::Vertex;
		myModeText.Init("Mode: Vertex", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::NumPad2))
	{
		HideSelectedVertexModels();
		HideSelectedEdgeModels();
		HideSelectedTriangleModels();
		ShowSelectedEdgeModels();
		mySelectedVertexIndices.RemoveAll();
		mySelectedTriangleIndices.RemoveAll();
		myDragAxis = -1;

		myTerrainEditMode = TerrainEditMode::Edge;
		myModeText.Init("Mode: Edge", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::NumPad3) || input->GetKeyPressed(CU::Input::KeyCode::Num8))
	{
		HideSelectedVertexModels();
		HideSelectedEdgeModels();
		HideSelectedTriangleModels();
		ShowSelectedTriangleModels();
		mySelectedVertexIndices.RemoveAll();
		mySelectedEdgeIndices.RemoveAll();
		myDragAxis = -1;

		myTerrainEditMode = TerrainEditMode::Triangle;
		myModeText.Init("Mode: Triangle", "font");
	}
	
	if (input->GetKeyPressed(CU::Input::KeyCode::Num1))
	{
		myDragAxis = -1;

		myTerrainEditTool = TerrainEditTool::Select;
		myToolText.Init("Tool: Select", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::Num2))
	{
		myDragAxis = -1;

		myTerrainEditTool = TerrainEditTool::Move;
		myToolText.Init("Tool: Move", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::Num3))
	{
		myDragAxis = -1;

		myTerrainEditTool = TerrainEditTool::Rotate;
		myToolText.Init("Tool: Rotate", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::Num4))
	{
		myDragAxis = -1;

		myTerrainEditTool = TerrainEditTool::Scale;
		myToolText.Init("Tool: Scale", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::Num5))
	{
		HideSelectedVertexModels();
		HideSelectedEdgeModels();
		HideSelectedTriangleModels();
		ShowSelectedVertexModels();
		mySelectedEdgeIndices.RemoveAll();
		mySelectedTriangleIndices.RemoveAll();
		myDragAxis = -1;

		myTerrainEditTool = TerrainEditTool::MergeVertex;
		myToolText.Init("Tool: MergeVertex", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::Num6))
	{
		HideSelectedVertexModels();
		HideSelectedEdgeModels();
		HideSelectedTriangleModels();
		mySelectedVertexIndices.RemoveAll();
		mySelectedEdgeIndices.RemoveAll();
		mySelectedTriangleIndices.RemoveAll();
		myDragAxis = -1;

		myTerrainEditTool = TerrainEditTool::SplitEdge;
		myToolText.Init("Tool: SplitEdge", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::Num7))
	{
		HideSelectedVertexModels();
		HideSelectedEdgeModels();
		HideSelectedTriangleModels();
		mySelectedVertexIndices.RemoveAll();
		mySelectedEdgeIndices.RemoveAll();
		mySelectedTriangleIndices.RemoveAll();
		myDragAxis = -1;

		myTerrainEditTool = TerrainEditTool::DivideTriangle;
		myToolText.Init("Tool: DivideTriangle", "font");
	}
	else if (input->GetKeyPressed(CU::Input::KeyCode::Num8))
	{
		myDragAxis = -1;
		myHasExtruded = false;

		myTerrainEditTool = TerrainEditTool::ExtrudeTriangle;
		myToolText.Init("Tool: ExtrudeTriangle", "font");
	}

	if (input->GetKeyPressed(CU::Input::KeyCode::NumPad5))
	{
		myTerrain.LogState();
	}

	//EngineProxy::Draw(&myDebugText);
	EngineProxy::Draw(&myModeText);
	EngineProxy::Draw(&myToolText);
	//EngineProxy::Draw(myRay);

	return CU::StateAction::None;
}

void LevelEditorState::Load()
{
	myCamera.SetFoV(3.14f / 3);
	myCamera.SetFarPlane(400.0f);
	myCamera.SetAspectRatio(static_cast<float>(EngineProxy::GetWindowSize().x) / static_cast<float>(EngineProxy::GetWindowSize().y));
	myCamera.SetPosition(CU::Vector3f(0.0f, 3.0f, -10.0f));

	const int manipulationGizmoID = myComponentManager.GetNewObjectID();
	myComponentManager.AddComponent<TransformComponent>(manipulationGizmoID);
	myComponentManager.AddComponent<ModelComponent>(manipulationGizmoID);
	//myComponentManager.AddComponent<DynamicCollisionComponent>(manipulationGizmoID);

	myTerrain.Init();
	//myTerrain.AddTriangleVertex(CU::Vector3f(0.5f, 0.1f, 0.5f), 1);
	//myTerrain.AddTriangleVertex(CU::Vector3f(0.2f, 0.0f, 0.2f), 1);
	myTerrainModel.InitCustom("Terrain");
	myTerrainModel.Load();
	myTerrainModel.SetRenderMode(ModelRenderMode::Deferred);
	//myTerrainModel.SetScale(CU::Vector3f::One * 2.0f);
	myTerrainModel.SetPosition(CU::Vector3f(0.0f, 0.0f, 0.0f));
	//myTerrainModel.Rotate(CU::Vector3f(3.14f, 0.0f, 0.0f));
	myTerrainModel.SetGeometryShader("Shaders\\CustomMeshGeometry.cso");
	//myTerrainModel.SetGeometryShader("Shaders\\FlatGeometry.cso");
	UpdateTerrainMesh();

	mySelectedVertexIndices.Init(8);
	mySelectedVertexModels.Init(8);
	GrowSelectedVertexModels(8);

	mySelectedEdgeIndices.Init(8);
	mySelectedEdgeModels.Init(8);
	GrowSelectedEdgeModels(8);

	mySelectedTriangleIndices.Init(8);
	mySelectedTriangleModels.Init(8);
	GrowSelectedTriangleModels(8);

	myHighlightedVertex.Init("Cube");
	myHighlightedVertex.SetRenderMode(ModelRenderMode::Forward);
	myHighlightedVertex.SetScale(CU::Vector3f::One * 0.1f);
	myHighlightedVertex.SetColor(CU::Vector4f(0.0f, 1.0f, 0.0f, 0.5f));
	myHighlightedVertex.Load();

	myHighlightedEdge.Init("Cube");
	myHighlightedEdge.SetRenderMode(ModelRenderMode::Forward);
	myHighlightedEdge.SetScale(CU::Vector3f::One * 0.1f);
	myHighlightedEdge.SetColor(CU::Vector4f(0.0f, 1.0f, 0.0f, 0.5f));
	myHighlightedEdge.Load();

	myHighlightedTriangle.InitCustom("SelectedTriangle");
	myHighlightedTriangle.Load();
	myHighlightedTriangle.SetColor(CU::Vector4f(0.0f, 1.0f, 0.0f, 0.5f));
	myHighlightedTriangle.SetRenderMode(ModelRenderMode::Forward);
	myHighlightedTriangle.SetGeometryShader("Shaders\\FlatGeometry.cso");
	myHighlightedTriangle.MarkCustomMeshForUpdate();
	CustomMesh* selectedTriangleMesh = myHighlightedTriangle.GetCustomMesh();
	if (selectedTriangleMesh != nullptr)
	{
		selectedTriangleMesh->myVertices.Resize(6);
		CU::GrowingArray<unsigned int>& indices(selectedTriangleMesh->myIndices);
		indices.Resize(24);
		indices[0] = 0;
		indices[1] = 4;
		indices[2] = 2;

		indices[3] = 1;
		indices[4] = 3;
		indices[5] = 5;

		indices[6] = 0;
		indices[7] = 2;
		indices[8] = 3;
		indices[9] = 0;
		indices[10] = 3;
		indices[11] = 1;

		indices[12] = 2;
		indices[13] = 4;
		indices[14] = 5;
		indices[15] = 2;
		indices[16] = 5;
		indices[17] = 3;

		indices[18] = 4;
		indices[19] = 0;
		indices[20] = 1;
		indices[21] = 4;
		indices[22] = 1;
		indices[23] = 5;
	}

	myGizmoMoveXModel.Init("Cube");
	myGizmoMoveXModel.SetRenderMode(ModelRenderMode::Forward);
	myGizmoMoveXModel.SetScale(CU::Vector3f(0.4f, 0.05f, 0.05f));
	myGizmoMoveXModel.SetColor(CU::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	myGizmoMoveXModel.Load();

	myGizmoMoveYModel.Init("Cube");
	myGizmoMoveYModel.SetRenderMode(ModelRenderMode::Forward);
	myGizmoMoveYModel.SetScale(CU::Vector3f(0.05f, 0.4f, 0.05f));
	myGizmoMoveYModel.SetColor(CU::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	myGizmoMoveYModel.Load();

	myGizmoMoveZModel.Init("Cube");
	myGizmoMoveZModel.SetRenderMode(ModelRenderMode::Forward);
	myGizmoMoveZModel.SetScale(CU::Vector3f(0.05f, 0.05f, 0.4f));
	myGizmoMoveZModel.SetColor(CU::Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	myGizmoMoveZModel.Load();

	myGizmoDirectionModel.Init("Cube");
	myGizmoDirectionModel.SetRenderMode(ModelRenderMode::Forward);
	myGizmoDirectionModel.SetScale(CU::Vector3f(0.05f, 0.05f, 0.4f));
	myGizmoDirectionModel.SetColor(CU::Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	myGizmoDirectionModel.Load();

	myDebugText.Init("-1", "font");
	myDebugText.SetPivot(CU::Vector2f::Zero);
	myDebugText.SetPosition(CU::Vector2f(SpriteUnits::GetBorderLeft(), SpriteUnits::GetBorderTop() + 0.12f));
	myDebugText.SetScale(CU::Vector2f::One * 1.5f);

	myModeText.Init("Mode: Vertex", "font");
	myModeText.SetPivot(CU::Vector2f::Zero);
	myModeText.SetPosition(CU::Vector2f(SpriteUnits::GetBorderLeft(), SpriteUnits::GetBorderTop() + 0.05f));
	myModeText.SetScale(CU::Vector2f::One * 1.5f);

	myToolText.Init("Tool: Select", "font");
	myToolText.SetPivot(CU::Vector2f::Zero);
	myToolText.SetPosition(CU::Vector2f(SpriteUnits::GetBorderLeft(), SpriteUnits::GetBorderTop() + 0.12f));
	myToolText.SetScale(CU::Vector2f::One * 1.5f);

	myTerrainEditMode = TerrainEditMode::Vertex;
	myTerrainEditTool = TerrainEditTool::Select;
}
void LevelEditorState::Unload()
{
}

void LevelEditorState::GainFocus()
{
	EngineProxy::GetActiveScene()->SetMainCamera(&myCamera);
	EngineProxy::GetActiveScene()->AddInstance(&myTerrainModel);

	CU::Input::Get()->UpdateConnected();
}
void LevelEditorState::LoseFocus()
{
	EngineProxy::GetActiveScene()->RemoveInstance(&myTerrainModel);
}

void LevelEditorState::UpdateTerrainMesh()
{
	CustomMesh* terrainMesh = myTerrainModel.GetCustomMesh();
	terrainMesh->myVertices = myTerrain.GetVertices();
	terrainMesh->myIndices = myTerrain.GetIndices();
	myTerrainModel.MarkCustomMeshForUpdate();
}

void LevelEditorState::UpdateSelect()
{
	CU::Input* input = CU::Input::Get();
	CU::GrowingArray<int>& indexArray = GetEditModeSelectedIndices(myTerrainEditMode);
	CU::GrowingArray<ModelInstance>& modelArray = GetEditModeSelectedModels(myTerrainEditMode);

	if (indexArray.Size() > 0 && input->GetKeyPressed(CU::Input::KeyCode::Delete))
	{
		for (int index = 0; index < indexArray.Size(); index++)
		{
			EngineProxy::GetActiveScene()->RemoveInstance(&modelArray[index]);
			myTerrain.RemoveVertex(indexArray[index]);
		}
		indexArray.RemoveAll();
		UpdateTerrainMesh();
		return;
	}

	const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
	CU::Vector3f rayHitPoint;
	int indexHit = -1;
	switch (myTerrainEditMode)
	{
	case TerrainEditMode::Vertex:
		indexHit = myTerrain.GetRayHitVertexIndex(ray.point, ray.direction * ray.range, rayHitPoint);
		break;
	case TerrainEditMode::Edge:
		indexHit = myTerrain.GetRayHitEdgeIndex(ray.point, ray.direction * ray.range, rayHitPoint);
		break;
	case TerrainEditMode::Triangle:
		indexHit = myTerrain.GetRayHitTriangleIndex(ray.point, ray.direction * ray.range, rayHitPoint);
		break;
	default:
		return;
	}
	
	if (indexHit != -1)
	{
		const int hitFoundIndex = indexArray.Find(indexHit);
		if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
		{
			if (hitFoundIndex != -1)
			{
				if (hitFoundIndex < indexArray.Size() - 1)
				{
					modelArray[hitFoundIndex] = modelArray[indexArray.Size() - 1];
				}
				indexArray.RemoveCyclicAtIndex(hitFoundIndex);
				EngineProxy::GetActiveScene()->RemoveInstance(&modelArray[indexArray.Size()]);
			}
			else
			{
				if (indexArray.Size() >= modelArray.Size())
				{
					GrowSelectedModels(myTerrainEditMode, indexArray.Size());
				}
				ModelInstance& selectModel = modelArray[indexArray.Size()];
				UpdateSelectModel(myTerrainEditMode, selectModel, indexHit);
				EngineProxy::GetActiveScene()->AddInstance(&selectModel);
				indexArray.Add(indexHit);
			}
		}
		else if (hitFoundIndex == -1)
		{
			ModelInstance& highlightedModel = GetEditModeHighlightedModel(myTerrainEditMode);
			UpdateSelectModel(myTerrainEditMode, highlightedModel, indexHit);
			EngineProxy::GetActiveScene()->AddInstance(&highlightedModel);
		}
	}
}

void LevelEditorState::UpdateMove(const float aDeltaTime)
{
	CU::Input* input = CU::Input::Get();
	CU::GrowingArray<int>& indexArray = GetEditModeSelectedIndices(myTerrainEditMode);
	CU::GrowingArray<ModelInstance>& modelArray = GetEditModeSelectedModels(myTerrainEditMode);

	if (indexArray.Size() > 0)
	{
		const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
		const CU::Vector3f middlePoint(GetEditModeMiddlePoint(myTerrainEditMode, indexArray));

		ShowGizmoMove(middlePoint);
		if (myDragAxis != -1)
		{
			switch (myDragAxis)
			{
			case 0:
				myGizmoMoveXModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			case 1:
				myGizmoMoveYModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			case 2:
				myGizmoMoveZModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			default:
				break;
			}
			if (input->GetMouseButtonDown(CU::Input::MouseButton::Left))
			{
				const CU::Intersection::LineRay3D dragRayStart(myCamera.GetRay(100.0f, myDragLocation));
				const CU::Intersection::LineRay3D dragRayEnd(myCamera.GetRay(100.0f, input->GetCursorDXSpace()));

				const CU::Vector3f dragStartVector(dragRayStart.point + dragRayStart.direction * dragRayStart.range);
				const CU::Vector3f dragEndVector(dragRayEnd.point + dragRayEnd.direction * dragRayEnd.range);
				const CU::Vector3f dragVector(dragEndVector - dragStartVector);

				CU::Vector3f axisDirection;
				switch (myDragAxis)
				{
				case 0:
					axisDirection = CU::Vector3f(1.0f, 0.0f, 0.0f);
					break;
				case 1:
					axisDirection = CU::Vector3f(0.0f, 1.0f, 0.0f);
					break;
				case 2:
					axisDirection = CU::Vector3f(0.0f, 0.0f, 1.0f);
					break;
				default:
					break;
				}
				const float dot = dragVector.Dot(axisDirection);
				const CU::Vector3f movement(axisDirection * dot * aDeltaTime * 3.0f);

				for (int index = 0; index < indexArray.Size(); index++)
				{
					ModelInstance& selectModel = modelArray[index];
					selectModel.SetPosition(selectModel.GetPosition() + movement);
					const int selectIndex = indexArray[index];
					switch (myTerrainEditMode)
					{
					case TerrainEditMode::Vertex:
						myTerrain.MoveVertex(movement, selectIndex);
						break;
					case TerrainEditMode::Edge:
						myTerrain.MoveEdge(movement, selectIndex);
						UpdateSelectModel(myTerrainEditMode, selectModel, selectIndex);
						break;
					case TerrainEditMode::Triangle:
						myTerrain.MoveTriangle(movement, selectIndex);
						break;
					default:
						break;
					}
				}
				UpdateTerrainMesh();

				myDragLocation = input->GetCursorDXSpace();
			}
			else
			{
				myDragAxis = -1;
			}
		}
		if (myDragAxis == -1)
		{
			const int hitGizmo = GetRayHitGizmoMoveAxis(ray);
			if (hitGizmo != -1)
			{
				if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
				{
					myDragAxis = hitGizmo;
					myDragLocation = input->GetCursorDXSpace();
				}
				else
				{
					switch (hitGizmo)
					{
					case 0:
						myGizmoMoveXModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					case 1:
						myGizmoMoveYModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					case 2:
						myGizmoMoveZModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

void LevelEditorState::UpdateRotate(const float aDeltaTime)
{
	CU::Input* input = CU::Input::Get();
	CU::GrowingArray<int>& indexArray = GetEditModeSelectedIndices(myTerrainEditMode);
	CU::GrowingArray<ModelInstance>& modelArray = GetEditModeSelectedModels(myTerrainEditMode);

	if (indexArray.Size() > 0)
	{
		const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
		const CU::Vector3f middlePoint(GetEditModeMiddlePoint(myTerrainEditMode, indexArray));

		ShowGizmoMove(middlePoint);
		if (myDragAxis != -1)
		{
			switch (myDragAxis)
			{
			case 0:
				myGizmoMoveXModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			case 1:
				myGizmoMoveYModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			case 2:
				myGizmoMoveZModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			default:
				break;
			}
			if (input->GetMouseButtonDown(CU::Input::MouseButton::Left))
			{
				const CU::Intersection::LineRay3D dragRayStart(myCamera.GetRay(100.0f, myDragLocation));
				const CU::Intersection::LineRay3D dragRayEnd(myCamera.GetRay(100.0f, input->GetCursorDXSpace()));

				const CU::Vector3f dragStartVector(dragRayStart.point + dragRayStart.direction * dragRayStart.range);
				const CU::Vector3f dragEndVector(dragRayEnd.point + dragRayEnd.direction * dragRayEnd.range);
				const CU::Vector3f dragVector(dragEndVector - dragStartVector);

				CU::Vector3f axisDirection;
				switch (myDragAxis)
				{
				case 0:
					axisDirection = CU::Vector3f(1.0f, 0.0f, 0.0f);
					break;
				case 1:
					axisDirection = CU::Vector3f(0.0f, 1.0f, 0.0f);
					break;
				case 2:
					axisDirection = CU::Vector3f(0.0f, 0.0f, 1.0f);
					break;
				default:
					break;
				}
				const float dot = dragVector.Dot(axisDirection);
				const float rotation = dot * aDeltaTime * 2.0f;

				for (int index = 0; index < indexArray.Size(); index++)
				{
					ModelInstance& selectModel = modelArray[index];
					const int selectIndex = indexArray[index];
					RotateAroundPoint(middlePoint, axisDirection, rotation, myTerrainEditMode, selectIndex);
					UpdateSelectModel(myTerrainEditMode, selectModel, selectIndex);
				}
				UpdateTerrainMesh();

				myDragLocation = input->GetCursorDXSpace();
			}
			else
			{
				myDragAxis = -1;
			}
		}
		if (myDragAxis == -1)
		{
			const int hitGizmo = GetRayHitGizmoMoveAxis(ray);
			if (hitGizmo != -1)
			{
				if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
				{
					myDragAxis = hitGizmo;
					myDragLocation = input->GetCursorDXSpace();
				}
				else
				{
					switch (hitGizmo)
					{
					case 0:
						myGizmoMoveXModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					case 1:
						myGizmoMoveYModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					case 2:
						myGizmoMoveZModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

void LevelEditorState::UpdateScale(const float aDeltaTime)
{
	CU::Input* input = CU::Input::Get();
	CU::GrowingArray<int>& indexArray = GetEditModeSelectedIndices(myTerrainEditMode);
	CU::GrowingArray<ModelInstance>& modelArray = GetEditModeSelectedModels(myTerrainEditMode);

	if (indexArray.Size() > 0)
	{
		const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
		const CU::Vector3f middlePoint(GetEditModeMiddlePoint(myTerrainEditMode, indexArray));

		ShowGizmoMove(middlePoint);
		if (myDragAxis != -1)
		{
			switch (myDragAxis)
			{
			case 0:
				myGizmoMoveXModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			case 1:
				myGizmoMoveYModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			case 2:
				myGizmoMoveZModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));
				break;
			default:
				break;
			}
			if (input->GetMouseButtonDown(CU::Input::MouseButton::Left))
			{
				const CU::Intersection::LineRay3D dragRayStart(myCamera.GetRay(100.0f, myDragLocation));
				const CU::Intersection::LineRay3D dragRayEnd(myCamera.GetRay(100.0f, input->GetCursorDXSpace()));

				const CU::Vector3f dragStartVector(dragRayStart.point + dragRayStart.direction * dragRayStart.range);
				const CU::Vector3f dragEndVector(dragRayEnd.point + dragRayEnd.direction * dragRayEnd.range);
				const CU::Vector3f dragVector(dragEndVector - dragStartVector);

				CU::Vector3f axisDirection;
				switch (myDragAxis)
				{
				case 0:
					axisDirection = CU::Vector3f(1.0f, 0.0f, 0.0f);
					break;
				case 1:
					axisDirection = CU::Vector3f(0.0f, 1.0f, 0.0f);
					break;
				case 2:
					axisDirection = CU::Vector3f(0.0f, 0.0f, 1.0f);
					break;
				default:
					break;
				}
				const float dot = dragVector.Dot(axisDirection);
				const float scale = 1.0f + dot * aDeltaTime * 3.0f;

				for (int index = 0; index < indexArray.Size(); index++)
				{
					ModelInstance& selectModel = modelArray[index];
					const int selectIndex = indexArray[index];
					ScaleFromPoint(middlePoint, axisDirection, scale, myTerrainEditMode, selectIndex);
					UpdateSelectModel(myTerrainEditMode, selectModel, selectIndex);
				}
				UpdateTerrainMesh();

				myDragLocation = input->GetCursorDXSpace();
			}
			else
			{
				myDragAxis = -1;
			}
		}
		if (myDragAxis == -1)
		{
			const int hitGizmo = GetRayHitGizmoMoveAxis(ray);
			if (hitGizmo != -1)
			{
				if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
				{
					myDragAxis = hitGizmo;
					myDragLocation = input->GetCursorDXSpace();
				}
				else
				{
					switch (hitGizmo)
					{
					case 0:
						myGizmoMoveXModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					case 1:
						myGizmoMoveYModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					case 2:
						myGizmoMoveZModel.SetColor(CU::Vector3f(1.0f, 1.0f, 0.0f));
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

void LevelEditorState::UpdateExtrude(const float aDeltaTime)
{
	CU::Input* input = CU::Input::Get();
	CU::GrowingArray<int>& indexArray = GetEditModeSelectedIndices(TerrainEditMode::Triangle);
	CU::GrowingArray<ModelInstance>& modelArray = GetEditModeSelectedModels(TerrainEditMode::Triangle);

	if (indexArray.Size() > 0)
	{
		const CU::Intersection::LineRay3D ray(myCamera.GetRay(1000.0f, input->GetCursorDXSpace()));
		const CU::Vector3f middlePoint(GetEditModeMiddlePoint(TerrainEditMode::Triangle, indexArray));

		const CU::Vector3f normal(GetTriangleNormal(indexArray[0]));
		CU::Quaternionf normalRotation;
		normalRotation.LookTowards(normal);

		myGizmoDirectionModel.SetPosition(middlePoint);
		myGizmoDirectionModel.SetColor(CU::Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		myGizmoDirectionModel.SetRotation(normalRotation);
		myGizmoDirectionModel.Move(CU::Vector3f(0.0f, 0.0f, 0.22f));
		EngineProxy::GetActiveScene()->AddInstance(&myGizmoDirectionModel);
		if (myDragAxis == 0)
		{
			myGizmoDirectionModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.7f));

			if (input->GetMouseButtonDown(CU::Input::MouseButton::Left))
			{
				const CU::Intersection::LineRay3D dragRayStart(myCamera.GetRay(100.0f, myDragLocation));
				const CU::Intersection::LineRay3D dragRayEnd(myCamera.GetRay(100.0f, input->GetCursorDXSpace()));

				const CU::Vector3f dragStartVector(dragRayStart.point + dragRayStart.direction * dragRayStart.range);
				const CU::Vector3f dragEndVector(dragRayEnd.point + dragRayEnd.direction * dragRayEnd.range);
				const CU::Vector3f dragVector(dragEndVector - dragStartVector);

				const float dot = dragVector.Dot(normal);
				const float extrusion = dot * aDeltaTime * 3.0f;

				if (!myHasExtruded)
				{
					myHasExtruded = true;
					myTerrain.ExtrudeTriangle(normal * extrusion, indexArray);
				}
				else
				{
					CU::GrowingArray<int> movedVertexArray(indexArray.Size() * 3);
					for (int index = 0; index < indexArray.Size(); index++)
					{
						//myTerrain.MoveTriangle(normal * extrution, indexArray[index]);
						CU::StaticArray<int, 3> vertexIndices(myTerrain.GetTriangleVertexIndices(indexArray[index]));
						for (int i = 0; i < 3; i++)
						{
							if (movedVertexArray.Find(vertexIndices[i]) == -1)
							{
								movedVertexArray.Add(vertexIndices[i]);
							}
						}
					}
					for (int index = 0; index < movedVertexArray.Size(); index++)
					{
						const int vertexIndex = movedVertexArray[index];
						const CU::Vector3f vertex(myTerrain.GetVertices()[vertexIndex]);
						myTerrain.SetVertex(vertex + normal * extrusion, vertexIndex);
					}
				}
				for (int index = 0; index < indexArray.Size(); index++)
				{
					ModelInstance& selectModel = modelArray[index];
					const int selectIndex = indexArray[index];
					UpdateSelectModel(myTerrainEditMode, selectModel, selectIndex);
				}
				UpdateTerrainMesh();

				myDragLocation = input->GetCursorDXSpace();
			}
			else
			{
				myDragAxis = -1;
			}
		}
		if (myDragAxis == -1)
		{
			int hitGizmo = -1;
			const CU::Vector3f zPos(myGizmoDirectionModel.GetPosition());
			const CU::Vector3f zScale(myGizmoDirectionModel.GetScale());
			//const CU::Intersection::AABB3D zBox(zPos - zScale * 0.5f, zPos + zScale * 0.5f);
			if (CU::Intersection::IntersectionAABBLineRotated(zPos, zScale * 0.5f, ray, myGizmoDirectionModel.GetRotation()))
			{
				hitGizmo = 0;
			}

			if (hitGizmo == 0)
			{
				if (input->GetMouseButtonPressed(CU::Input::MouseButton::Left))
				{
					myDragAxis = hitGizmo;
					myDragLocation = input->GetCursorDXSpace();
					myHasExtruded = false;
				}
				else
				{
					myGizmoDirectionModel.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
	}
}

void LevelEditorState::UpdateSelectModel(const TerrainEditMode aMode, ModelInstance& aSelectModel, const int aSelectIndex)
{
	switch (aMode)
	{
	case TerrainEditMode::Vertex:
		aSelectModel.SetPosition(myTerrain.GetVertices()[aSelectIndex]);
		break;
	case TerrainEditMode::Edge:
	{
		const CU::StaticArray<CU::Vector3f, 2> edge(myTerrain.GetEdgeVertices(aSelectIndex));
		const CU::Vector3f edgeVector = edge[1] - edge[0];
		CU::Quaternionf edgeRotation;
		edgeRotation.LookTowards(edgeVector);
		aSelectModel.SetRotation(edgeRotation);
		aSelectModel.SetPosition(edge[0] + edgeVector * 0.5f);
		aSelectModel.SetScale(CU::Vector3f(0.05f, 0.05f, edgeVector.Length()));
		break;
	}
	case TerrainEditMode::Triangle:
	{
		const CU::Vector3f normal(GetTriangleNormal(aSelectIndex));
		const CU::Vector3f down(normal * -0.02f);
		const CU::Vector3f up(normal * 0.02f);
		const CU::StaticArray<CU::Vector3f, 3> triangle(myTerrain.GetTriangleVertices(aSelectIndex));
		aSelectModel.SetPosition(CU::Vector3f::Zero);
		CU::GrowingArray<CU::Vector4f>& vertices = aSelectModel.GetCustomMesh()->myVertices;
		vertices[0] = CU::Vector4f(triangle[0] + down, 1.0f);
		vertices[1] = CU::Vector4f(triangle[0] + up, 1.0f);
		vertices[2] = CU::Vector4f(triangle[1] + down, 1.0f);
		vertices[3] = CU::Vector4f(triangle[1] + up, 1.0f);
		vertices[4] = CU::Vector4f(triangle[2] + down, 1.0f);
		vertices[5] = CU::Vector4f(triangle[2] + up, 1.0f);
		aSelectModel.MarkCustomMeshForUpdate();
		break;
	}
	default:
		return;
	}
}

void LevelEditorState::RotateAroundPoint(const CU::Vector3f aPoint, const CU::Vector3f aRotationAxis, const float aRotation, const TerrainEditMode aMode, const int aSelectIndex)
{
	const CU::Quaternionf rotation(aRotationAxis, aRotation);
	switch (aMode)
	{
	case TerrainEditMode::Vertex:
	{
		CU::Vector3f newPoint = CU::Vector3f(myTerrain.GetVertices()[aSelectIndex]) - aPoint;
		rotation.RotateVector(newPoint);
		newPoint += aPoint;
		myTerrain.SetVertex(newPoint, aSelectIndex);
		break;
	}
	case TerrainEditMode::Edge:
	{
		CU::StaticArray<int, 2> vertexIndices(myTerrain.GetEdgeVertexIndices(aSelectIndex));
		for (int i = 0; i < 2; i++)
		{
			const int vertexIndex = vertexIndices[i];
			CU::Vector3f newPoint = CU::Vector3f(myTerrain.GetVertices()[vertexIndex]) - aPoint;
			rotation.RotateVector(newPoint);
			newPoint += aPoint;
			myTerrain.SetVertex(newPoint, vertexIndex);
		}
		break;
	}
	case TerrainEditMode::Triangle:
	{
		CU::StaticArray<int, 3> vertexIndices(myTerrain.GetTriangleVertexIndices(aSelectIndex));
		for (int i = 0; i < 3; i++)
		{
			const int vertexIndex = vertexIndices[i];
			CU::Vector3f newPoint = CU::Vector3f(myTerrain.GetVertices()[vertexIndex]) - aPoint;
			rotation.RotateVector(newPoint);
			newPoint += aPoint;
			myTerrain.SetVertex(newPoint, vertexIndex);
		}
		break;
	}
	default:
		break;
	}
}

void LevelEditorState::ScaleFromPoint(const CU::Vector3f aPoint, const CU::Vector3f aScaleDirection, const float aScale, const TerrainEditMode aMode, const int aSelectIndex)
{
	const CU::Vector3f invertedScaleDirection(CU::Vector3f::One - aScaleDirection);
	switch (aMode)
	{
	case TerrainEditMode::Vertex:
	{
		const CU::Vector3f distanceVector(CU::Vector3f(myTerrain.GetVertices()[aSelectIndex]) - aPoint);
		const CU::Vector3f scaledDistance(distanceVector * aScaleDirection * aScale);
		myTerrain.SetVertex(aPoint + scaledDistance + distanceVector * invertedScaleDirection, aSelectIndex);
		break;
	}
	case TerrainEditMode::Edge:
	{
		CU::StaticArray<int, 2> vertexIndices(myTerrain.GetEdgeVertexIndices(aSelectIndex));
		for (int i = 0; i < 2; i++)
		{
			const int vertexIndex = vertexIndices[i];
			const CU::Vector3f distanceVector(CU::Vector3f(myTerrain.GetVertices()[vertexIndex]) - aPoint);
			const CU::Vector3f scaledDistance(distanceVector * aScaleDirection * aScale);
			myTerrain.SetVertex(aPoint + scaledDistance + distanceVector * invertedScaleDirection, vertexIndex);
		}
		break;
	}
	case TerrainEditMode::Triangle:
	{
		CU::StaticArray<int, 3> vertexIndices(myTerrain.GetTriangleVertexIndices(aSelectIndex));
		for (int i = 0; i < 3; i++)
		{
			const int vertexIndex = vertexIndices[i];
			const CU::Vector3f distanceVector(CU::Vector3f(myTerrain.GetVertices()[vertexIndex]) - aPoint);
			const CU::Vector3f scaledDistance(distanceVector * aScaleDirection * aScale);
			myTerrain.SetVertex(aPoint + scaledDistance + distanceVector * invertedScaleDirection, vertexIndex);
		}
		break;
	}
	default:
		break;
	}
}

CU::Vector3f LevelEditorState::GetTriangleNormal(const int aTriangleIndex)
{
	const CU::StaticArray<CU::Vector3f, 3> triangle(myTerrain.GetTriangleVertices(aTriangleIndex));
	CU::Vector3f normal((triangle[1] - triangle[0]).Cross(triangle[2] - triangle[0]));
	normal.Normalize();
	return normal;
}

CU::GrowingArray<int>& LevelEditorState::GetEditModeSelectedIndices(const TerrainEditMode aMode)
{
	switch (aMode)
	{
	default:
	case TerrainEditMode::Vertex:
		return mySelectedVertexIndices;
	case TerrainEditMode::Edge:
		return mySelectedEdgeIndices;
	case TerrainEditMode::Triangle:
		return mySelectedTriangleIndices;
	}
}
CU::GrowingArray<ModelInstance>& LevelEditorState::GetEditModeSelectedModels(const TerrainEditMode aMode)
{
	switch (aMode)
	{
	default:
	case TerrainEditMode::Vertex:
		return mySelectedVertexModels;
	case TerrainEditMode::Edge:
		return mySelectedEdgeModels;
	case TerrainEditMode::Triangle:
		return mySelectedTriangleModels;
	}
}
ModelInstance& LevelEditorState::GetEditModeHighlightedModel(const TerrainEditMode aMode)
{
	switch (aMode)
	{
	default:
	case TerrainEditMode::Vertex:
		return myHighlightedVertex;
	case TerrainEditMode::Edge:
		return myHighlightedEdge;
	case TerrainEditMode::Triangle:
		return myHighlightedTriangle;
	}
}
CU::Vector3f LevelEditorState::GetEditModeMiddlePoint(const TerrainEditMode aMode, const CU::GrowingArray<int>& aIndexArray)
{
	CU::Vector3f middle;
	switch (aMode)
	{
	case TerrainEditMode::Vertex:
		for (int index = 0; index < aIndexArray.Size(); index++)
		{
			middle += CU::Vector3f(myTerrain.GetVertices()[aIndexArray[index]]);
		}
		if (aIndexArray.Size() > 1)
		{
			middle /= static_cast<float>(aIndexArray.Size());
		}
		break;
	case TerrainEditMode::Edge:
		for (int index = 0; index < aIndexArray.Size(); index++)
		{
			const CU::StaticArray<CU::Vector3f, 2> edgeVertices(myTerrain.GetEdgeVertices(aIndexArray[index]));
			middle += edgeVertices[0];
			middle += edgeVertices[1];
		}
		if (aIndexArray.Size() > 0)
		{
			middle /= static_cast<float>(aIndexArray.Size() * 2);
		}
		break;
	case TerrainEditMode::Triangle:
		for (int index = 0; index < aIndexArray.Size(); index++)
		{
			const CU::StaticArray<CU::Vector3f, 3> triangleVertices(myTerrain.GetTriangleVertices(aIndexArray[index]));
			middle += triangleVertices[0];
			middle += triangleVertices[1];
			middle += triangleVertices[2];
		}
		if (aIndexArray.Size() > 0)
		{
			middle /= static_cast<float>(aIndexArray.Size() * 3);
		}
		break;
	default:
		break;
	}
	return middle;
}

void LevelEditorState::GrowSelectedModels(const TerrainEditMode aMode, const int aSize)
{
	switch (aMode)
	{
	case TerrainEditMode::Vertex:
		GrowSelectedVertexModels(aSize);
		break;
	case TerrainEditMode::Edge:
		GrowSelectedEdgeModels(aSize);
		break;
	case TerrainEditMode::Triangle:
		GrowSelectedTriangleModels(aSize);
		break;
	default:
		return;
	}
}

void LevelEditorState::GrowSelectedVertexModels(const int aSize)
{
	while (mySelectedVertexModels.Capacity() < aSize)
	{
		mySelectedVertexModels.Grow();
	}

	while (mySelectedVertexModels.Size() < mySelectedVertexModels.Capacity())
	{
		ModelInstance& model = mySelectedVertexModels.Add();
		model.Init("Cube");
		model.SetRenderMode(ModelRenderMode::Forward);
		model.SetScale(CU::Vector3f::One * 0.1f);
		model.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.5f));
	}
}
void LevelEditorState::GrowSelectedEdgeModels(const int aSize)
{
	while (mySelectedEdgeModels.Capacity() < aSize)
	{
		mySelectedEdgeModels.Grow();
	}

	while (mySelectedEdgeModels.Size() < mySelectedEdgeModels.Capacity())
	{
		ModelInstance& model = mySelectedEdgeModels.Add();
		model.Init("Cube");
		model.SetRenderMode(ModelRenderMode::Forward);
		model.SetScale(CU::Vector3f::One * 0.1f);
		model.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.5f));
	}
}
void LevelEditorState::GrowSelectedTriangleModels(const int aSize)
{
	while (mySelectedTriangleModels.Capacity() < aSize)
	{
		mySelectedTriangleModels.Grow();
	}

	while (mySelectedTriangleModels.Size() < mySelectedTriangleModels.Capacity())
	{
		ModelInstance& model = mySelectedTriangleModels.Add();
		model.InitCustom(std::string("SelectedTriangle") + std::to_string(mySelectedTriangleModels.Size()));
		model.Load();
		model.SetRenderMode(ModelRenderMode::Forward);
		model.SetColor(CU::Vector4f(1.0f, 1.0f, 0.0f, 0.5f));
		model.SetGeometryShader("Shaders\\FlatGeometry.cso");
		model.MarkCustomMeshForUpdate();
		CustomMesh* selectedTriangleMesh = model.GetCustomMesh();
		if (selectedTriangleMesh != nullptr)
		{
			selectedTriangleMesh->myVertices.Resize(6);
			CU::GrowingArray<unsigned int>& indices(selectedTriangleMesh->myIndices);
			indices.Resize(24);
			indices[0] = 0;
			indices[1] = 4;
			indices[2] = 2;

			indices[3] = 1;
			indices[4] = 3;
			indices[5] = 5;

			indices[6] = 0;
			indices[7] = 2;
			indices[8] = 3;
			indices[9] = 0;
			indices[10] = 3;
			indices[11] = 1;

			indices[12] = 2;
			indices[13] = 4;
			indices[14] = 5;
			indices[15] = 2;
			indices[16] = 5;
			indices[17] = 3;

			indices[18] = 4;
			indices[19] = 0;
			indices[20] = 1;
			indices[21] = 4;
			indices[22] = 1;
			indices[23] = 5;
		}
	}
}

void LevelEditorState::ShowSelectedVertexModels()
{
	for (int index = 0; index < mySelectedVertexIndices.Size(); index++)
	{
		EngineProxy::GetActiveScene()->AddInstance(&mySelectedVertexModels[index]);
	}
}
void LevelEditorState::ShowSelectedEdgeModels()
{
	for (int index = 0; index < mySelectedEdgeIndices.Size(); index++)
	{
		EngineProxy::GetActiveScene()->AddInstance(&mySelectedEdgeModels[index]);
	}
}
void LevelEditorState::ShowSelectedTriangleModels()
{
	for (int index = 0; index < mySelectedTriangleIndices.Size(); index++)
	{
		EngineProxy::GetActiveScene()->AddInstance(&mySelectedTriangleModels[index]);
	}
}
void LevelEditorState::ShowGizmoMove(const CU::Vector3f aPosition)
{
	myGizmoMoveXModel.SetPosition(aPosition + CU::Vector3f(0.22f, 0.0f, 0.0f));
	myGizmoMoveXModel.SetColor(CU::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	myGizmoMoveYModel.SetPosition(aPosition + CU::Vector3f(0.0f, 0.22f, 0.0f));
	myGizmoMoveYModel.SetColor(CU::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	myGizmoMoveZModel.SetPosition(aPosition + CU::Vector3f(0.0f, 0.0f, 0.22f));
	myGizmoMoveZModel.SetColor(CU::Vector4f(0.0f, 0.0f, 1.0f, 1.0f));

	EngineProxy::GetActiveScene()->AddInstance(&myGizmoMoveXModel);
	EngineProxy::GetActiveScene()->AddInstance(&myGizmoMoveYModel);
	EngineProxy::GetActiveScene()->AddInstance(&myGizmoMoveZModel);
}
void LevelEditorState::HideSelectedVertexModels()
{
	for (int index = 0; index < mySelectedVertexIndices.Size(); index++)
	{
		EngineProxy::GetActiveScene()->RemoveInstance(&mySelectedVertexModels[index]);
	}
}
void LevelEditorState::HideSelectedEdgeModels()
{
	for (int index = 0; index < mySelectedEdgeIndices.Size(); index++)
	{
		EngineProxy::GetActiveScene()->RemoveInstance(&mySelectedEdgeModels[index]);
	}
}
void LevelEditorState::HideSelectedTriangleModels()
{
	for (int index = 0; index < mySelectedTriangleIndices.Size(); index++)
	{
		EngineProxy::GetActiveScene()->RemoveInstance(&mySelectedTriangleModels[index]);
	}
}
void LevelEditorState::HideGizmoMove()
{
	EngineProxy::GetActiveScene()->RemoveInstance(&myGizmoMoveXModel);
	EngineProxy::GetActiveScene()->RemoveInstance(&myGizmoMoveYModel);
	EngineProxy::GetActiveScene()->RemoveInstance(&myGizmoMoveZModel);
}

int LevelEditorState::GetRayHitGizmoMoveAxis(const CU::Intersection::LineRay3D& aRay)
{
	const CU::Vector3f xPos(myGizmoMoveXModel.GetPosition());
	const CU::Vector3f xScale(myGizmoMoveXModel.GetScale());
	const CU::Intersection::AABB3D xBox(xPos - xScale * 0.5f, xPos + xScale * 0.5f);
	if (CU::Intersection::IntersectionAABBLine(xBox, aRay))
	{
		return 0;
	}
	const CU::Vector3f yPos(myGizmoMoveYModel.GetPosition());
	const CU::Vector3f yScale(myGizmoMoveYModel.GetScale());
	const CU::Intersection::AABB3D yBox(yPos - yScale * 0.5f, yPos + yScale * 0.5f);
	if (CU::Intersection::IntersectionAABBLine(yBox, aRay))
	{
		return 1;
	}
	const CU::Vector3f zPos(myGizmoMoveZModel.GetPosition());
	const CU::Vector3f zScale(myGizmoMoveZModel.GetScale());
	const CU::Intersection::AABB3D zBox(zPos - zScale * 0.5f, zPos + zScale * 0.5f);
	if (CU::Intersection::IntersectionAABBLine(zBox, aRay))
	{
		return 2;
	}
	return -1;
}