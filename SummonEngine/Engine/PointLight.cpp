#include "stdafx.h"
#include "PointLight.h"

const CU::Vector3f& PointLight::GetColor() const
{
	return myColor;
}

const float PointLight::GetRange() const
{
	return myRange;
}

PointLight::PointLight()
{
	//myUserCount = 0;
	myRange = 0.0f;
}

/*void PointLight::AddUser()
{
	myUserCount++;
}
void PointLight::RemoveUser()
{
	myUserCount--;
}
unsigned int PointLight::GetUserCount()
{
	return myUserCount;
}*/