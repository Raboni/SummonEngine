#include "stdafx.h"
#include "CustomModel.h"

CustomMesh& CustomModel::GetCustomMesh()
{
	return myCustomMesh;
}
const CustomMesh& CustomModel::GetCustomMesh() const
{
	return myCustomMesh;
}

void CustomModel::MarkForUpdate()
{
	myShouldUpdateMesh = true;
}

CustomModel::CustomModel() : Model()
{
	myIsCustom = true;
	myShouldUpdateMesh = false;
}