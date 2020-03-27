#pragma once
#include <Vector3.hpp>

struct RaycastHitResult
{
	CU::Vector3f myPosition;
	CU::Vector3f myNormal;
	float myDistance = 0.0f;
	unsigned int myIdentityFlag = 0;
};